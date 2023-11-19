#include "LibUvEventDispatcher.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QThread>
#include <QTimerEvent>
#include <QVariant>

#include "uv.h"

namespace iz::Eventing
{
LibUvEventDispatcher::LibUvEventDispatcher(QObject* parent)
    : QAbstractEventDispatcher(parent)
{
    m_uvLoop = new uv_loop_t();
    ::uv_loop_init(m_uvLoop);

    m_wakeupHandle = new uv_async_t();
    ::uv_async_init(m_uvLoop, m_wakeupHandle, nullptr);

    // from docs:
    //  The libuv event loop (if run in the default mode) will run until there are no active and referenced handles left.
    //  The user can force the loop to exit early by unreferencing handles which are active,
    //  for example by calling uv_unref() after calling uv_timer_start().
    ::uv_unref(reinterpret_cast<uv_handle_t*>(m_wakeupHandle));
}

LibUvEventDispatcher::~LibUvEventDispatcher()
{
    ::uv_loop_close(m_uvLoop);

    delete m_uvLoop;
    delete m_wakeupHandle;
}

uv_loop_t* LibUvEventDispatcher::rawLoop() const
{
    return m_uvLoop;
}

void LibUvEventDispatcher::interrupt()
{
    m_interrupt = true;
}

bool LibUvEventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    // we are awake!
    emit awake();

    // zero out processed callbacks
    m_processedCallbacks = 0;

    // time to send posted events
    QCoreApplication::sendPostedEvents();

    // will we block on libuv run?
    const bool willWait = (flags & QEventLoop::WaitForMoreEvents) && !m_interrupt;

    // handle potential interrupt
    if (m_interrupt) {
        m_interrupt = false;

        return false;
    }

    // run libuv poll depending on willWait value
    if (willWait) {
        // we will block! sigalize it
        emit aboutToBlock();

        // run loop - block if no events
        ::uv_run(m_uvLoop, UV_RUN_ONCE);
    } else {
        // run loop once, do not block on no events
        ::uv_run(m_uvLoop, UV_RUN_NOWAIT);
    }

    // return true if we processed something
    return m_processedCallbacks > 0;
}

void LibUvEventDispatcher::registerSocketNotifier(QSocketNotifier* notifier)
{
    // transform QSocketNotifier::Type to uv_poll_event
    int events = qtouv(notifier->type());
    if (events == -1) {
        return;
    }

    // libuv poller handler
    uv_poll_t* poller{ nullptr };

    // try to find existing socket poller
    // we will reuse it if one exists already
    // optionaly setting additional flag on it to listen for more event types
    auto it = m_socketPollers.find(notifier->socket());

    if (it == m_socketPollers.end()) {
        // no socket poller found for this descriptor
        poller = new uv_poll_t();

        // create and assign new poller data struct
        auto pollerData = std::make_unique<PollerData>(notifier->socket(), poller, 0, this);

        // set poller data
        poller->data = pollerData.get();

        // initialize uv poller
        ::uv_poll_init_socket(m_uvLoop, poller, notifier->socket());

        // actualize pollers index
        m_socketPollers.insert(std::make_pair(notifier->socket(), std::move(pollerData)));

    } else {
        // we got poller we can reuse!
        poller = it->second->uvPollData;
    }

    // update poller events
    static_cast<PollerData*>(poller->data)->events |= events;

    // setup read and write notifiers
    if (events == UV_READABLE) {
        static_cast<PollerData*>(poller->data)->readNotifier = notifier;
    }

    // set notifiers
    if (events == UV_WRITABLE) {
        static_cast<PollerData*>(poller->data)->writeNotifier = notifier;
    }

    // attach our custom data to QSocketNotifier
    notifier->setProperty("socketData", QVariant::fromValue(static_cast<PollerData*>(poller->data)));

    // start libuv's polling
    ::uv_poll_start(poller, static_cast<PollerData*>(poller->data)->events, LibUvEventDispatcher::socketCallback);
}

void LibUvEventDispatcher::socketCallback(uv_poll_s* w, int /*status*/, int events)
{
    // get our data from libuv's handler
    auto pollerData = static_cast<PollerData*>(w->data);
    pollerData->context->m_processedCallbacks++;

    // send required events
    if ((events & UV_READABLE) == UV_READABLE) {
        QEvent e(QEvent::SockAct);
        QCoreApplication::sendEvent(pollerData->readNotifier, &e);
    }

    if ((events & UV_WRITABLE) == UV_WRITABLE) {
        QEvent e(QEvent::SockAct);
        QCoreApplication::sendEvent(pollerData->writeNotifier, &e);
    }
}

void LibUvEventDispatcher::unregisterSocketNotifier(QSocketNotifier* notifier)
{
    // transform QSocketNotifier::Type to uv_poll_event
    int events = qtouv(notifier->type());
    if (events == -1) {
        return;
    }

    // get our data from libuv handler and actualize events flags
    auto pollerData = notifier->property("socketData").value<PollerData*>();
    pollerData->events &= ~events;

    // bne event types left? schedule deletion of libuv's poller
    if (pollerData->events == 0) {
        // we can delete this now
        notifier->setProperty("socketData", QVariant());
        m_socketPollers.erase(notifier->socket());

        ::uv_close(( uv_handle_t* )pollerData->uvPollData, [](uv_handle_t* handle) { delete handle; });
    } else {
        // actualize libuv polling
        ::uv_poll_start(pollerData->uvPollData, pollerData->events, LibUvEventDispatcher::socketCallback);
    }
}

void LibUvEventDispatcher::enableSocketNotifier(QSocketNotifier* notifier)
{
    // transform QSocketNotifier::Type to uv_poll_event
    int events = qtouv(notifier->type());
    if (events == -1) {
        return;
    }

    // get our data from libuv' handler and actualize events flags
    auto pollerData = notifier->property("socketData").value<PollerData*>();
    pollerData->events |= events;

    // start libuv's polling
    ::uv_poll_start(pollerData->uvPollData, pollerData->events, LibUvEventDispatcher::socketCallback);
}

void LibUvEventDispatcher::disableSocketNotifier(QSocketNotifier* notifier)
{
    int events = qtouv(notifier->type());
    if (events == -1) {
        return;
    }

    // get our data from libuv's handler and actualize events flags
    auto pollerData = notifier->property("socketData").value<PollerData*>();
    pollerData->events &= ~events;

    // if no flags are set stop polling
    // some are set? start libuv's polling with remaining ones
    if (pollerData->events == 0) {
        ::uv_poll_stop(pollerData->uvPollData);
    } else {
        ::uv_poll_start(pollerData->uvPollData, pollerData->events, LibUvEventDispatcher::socketCallback);
    }
}

int LibUvEventDispatcher::remainingTime(int timerId)
{
    auto it = m_timers.find(timerId);
    if (it != m_timers.end()) {
        int shouldFireAtDelta = (it->second->lastFired + it->second->interval) - (::uv_hrtime() / 1000000);

        // overdue timer
        if (shouldFireAtDelta < 0) {
            return 0;
        }

        return shouldFireAtDelta;
    }

    // non existent timer
    return -1;
}

void LibUvEventDispatcher::registerTimer(int timerId, qint64 interval, Qt::TimerType timerType, QObject* object)
{
    auto* timer = new uv_timer_t();
    auto timerData = new TimerData(timerId, interval, object, timerType, timer, this);
    timer->data = timerData;

    ::uv_timer_init(m_uvLoop, timer);

    auto it = m_objectTimerIndex.find(object);
    if (it == m_objectTimerIndex.end()) {
        m_objectTimerIndex.emplace(object, std::unordered_set<TimerData*>{ timerData });
    } else {
        it->second.insert(timerData);
    }

    m_timers.emplace(timerId, timerData);

    ::uv_timer_start(timer, LibUvEventDispatcher::timerCallback, interval, interval);
}

bool LibUvEventDispatcher::unregisterTimer(int timerId)
{
    auto it = m_timers.find(timerId);
    if (it != m_timers.end()) {
        auto object = it->second->qobject;
        auto idx = m_objectTimerIndex.find(object);
        idx->second.erase(it->second);

        if (idx->second.empty()) {
            m_objectTimerIndex.erase(object);
        }

        ::uv_timer_stop(it->second->timer);
        ::uv_close(( uv_handle_t* )it->second->timer, &LibUvEventDispatcher::timerDeleteCallback);

        m_timers.erase(it);

        return true;
    }

    return false;
}

QList<QAbstractEventDispatcher::TimerInfo> LibUvEventDispatcher::registeredTimers(QObject* object) const
{
    QList<QAbstractEventDispatcher::TimerInfo> res;

    auto it = m_objectTimerIndex.find(object);
    if (it != m_objectTimerIndex.end()) {
        res.reserve(static_cast<int>(it->second.size()));
        for (const auto timerData : it->second) {
            res.append({ timerData->timerID, static_cast<int>(timerData->interval), timerData->timerType });
        }
    }

    return res;
}

bool LibUvEventDispatcher::unregisterTimers(QObject* object)
{
    auto it = m_objectTimerIndex.find(object);
    if (it != m_objectTimerIndex.end()) {
        for (const auto idx : it->second) {
            // copy timerID
            int timerID = idx->timerID;

            // stop libuv timer
            ::uv_timer_stop(idx->timer);
            ::uv_close(( uv_handle_t* )idx->timer, &LibUvEventDispatcher::timerDeleteCallback);

            // erase timerID from m_timers index
            m_timers.erase(timerID);
        }

        // erase object from m_objectTimerIndex index
        m_objectTimerIndex.erase(it);

        return true;
    }

    return false;
}

void LibUvEventDispatcher::wakeUp()
{
    ::uv_async_send(m_wakeupHandle);
}

int LibUvEventDispatcher::qtouv(QSocketNotifier::Type qtEventType) const
{
    switch (qtEventType) {
    case QSocketNotifier::Read:
        return UV_READABLE;
    case QSocketNotifier::Write:
        return UV_WRITABLE;
    default:
        qCritical() << "Unsupported QSocketNotifier type.";
        return -1;
    }
}

void LibUvEventDispatcher::timerCallback(uv_timer_s* w)
{
    auto timerData = static_cast<TimerData*>(w->data);
    timerData->context->m_processedCallbacks++;

    // set last fired
    timerData->lastFired = ::uv_hrtime() / 1000000;

    QTimerEvent e(timerData->timerID);
    QCoreApplication::sendEvent(timerData->qobject, &e);
}

void LibUvEventDispatcher::timerDeleteCallback(uv_handle_s* w)
{
    auto* timer = ( uv_timer_s* )w;

    delete (( TimerData* )timer->data);
    delete timer;
}
}   // namespace iz::Eventing
