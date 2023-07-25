#pragma once

#include <QThread>
#include <mutex>
#ifndef Q_MOC_RUN // https://bugreports.qt.io/browse/QTBUG-22829
#include <thread>
#include <condition_variable>
#endif

class WQApplication;
class DispatchObject;
class DispatchThread;

namespace Wt {
class WEvent;
}

/*
 * Thread in which all interaction with Qt objects is done.
 *
 * If constructed <i>withEventLoop</i>, then QThread::exec() is
 * called, starting a new Qt event loop, and signal/slot events can be
 * delivered within the event loop handling. Otherwise, plain thread
 * synchronization is implemented.
 */
class DispatchThread : public QThread {
public:
  DispatchThread(WQApplication* app);

  void run() override;

  std::unique_lock<std::mutex> *eventLock() { return eventLock_; }

  void notify(const Wt::WEvent& event);
  void destroy();
  bool exception() const { return exception_; }
  void resetException();

  void waitDone();

private:
  WQApplication                  *app_;
  std::unique_ptr<DispatchObject> dispatchObject_;
  const Wt::WEvent                   *event_ = nullptr;
  bool                            exception_ = false;

  std::mutex                      doneMutex_;
  bool                            done_ = false;
  std::condition_variable         doneCondition_;

  std::mutex                      newEventMutex_;
  bool                            newEvent_ = false;
  std::condition_variable         newEventCondition_;
  std::unique_lock<std::mutex>   *eventLock_ = nullptr;

  void doEvent();

  void signalDone();
  void myPropagateEvent();

  friend class DispatchObject;
};
