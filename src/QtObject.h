#pragma once

#include <QThread>

class HelloApplication;

class QtObject : public QObject {
  Q_OBJECT

public:
  QtObject(HelloApplication* wt);

  void passGreet(const QString&);

signals:
  void greet(const QString&);

public slots:
  void doGreet(const QString&);

private:
  HelloApplication* wt_;
};
