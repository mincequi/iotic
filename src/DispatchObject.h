#pragma once

#include <QObject>

class DispatchThread;

/*
 * Help object used to dispatch an event into a Qt event loop.
 */
class DispatchObject : public QObject {
    Q_OBJECT
public:
  DispatchObject(DispatchThread *thread);

  void propagateEvent();

signals:
  void doEvent();

private slots:
  void onEvent();

private:
  DispatchThread *thread_;
};
