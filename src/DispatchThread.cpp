#include "WQApplication.h"

#include "DispatchThread.h"

#include "DispatchObject.h"

DispatchThread::DispatchThread(WQApplication *app)
    : QThread(),
      app_(app) {
}

void DispatchThread::run() {
    dispatchObject_ = std::make_unique<DispatchObject>(this);
    signalDone();
    exec();
    dispatchObject_ = nullptr;
    signalDone();
}

void DispatchThread::myPropagateEvent() {
    {
        std::unique_lock<std::mutex> lock(newEventMutex_);
        newEvent_ = true;
    }
    newEventCondition_.notify_one();
}

void DispatchThread::signalDone() {
    Wt::log("debug") << "WQApplication: [thread] signaling event done";
    {
        std::unique_lock<std::mutex> lock(doneMutex_);
        done_ = true;
    }
    doneCondition_.notify_one();
}

void DispatchThread::waitDone() {
    std::unique_lock<std::mutex> lock(doneMutex_);

    while (!done_) {
        doneCondition_.wait(lock);
    }
}

void DispatchThread::notify(const Wt::WEvent& event) {
    if (event_) {
        app_->realNotify(event);
    } else {
        event_ = &event;

        done_ = false;

        if (dispatchObject_)
            dispatchObject_->propagateEvent();
        else
            myPropagateEvent();

        waitDone();

        event_ = 0;
    }
}

void DispatchThread::destroy() {
    QThread::exit();
}

void DispatchThread::resetException() {
    exception_ = false;
}

void DispatchThread::doEvent() {
    Wt::log("debug") << "WQApplication: [thread] handling event";
    app_->attachThread(true);

    try {
        app_->realNotify(*event_);
    } catch (std::exception& e) {
        Wt::log("error") << "WQApplication: [thread] Caught exception: " << e.what();
        exception_ = true;
    } catch (...) {
        Wt::log("error") << "WQApplication: [thread] Caught exception";
        exception_ = true;
    }
    app_->attachThread(false);
    Wt::log("debug") << "WQApplication: [thread] done handling event";

    signalDone();
}
