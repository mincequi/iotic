#pragma once

#include "WQApplication.h"

class QtObject;
class QString;

using namespace Wt;

namespace Wt {
  class WLineEdit;
  class WText;
}

class HelloApplication : public WQApplication
{
public:
  HelloApplication(const WEnvironment& env);

  void doGreet(const QString&);

  virtual void create();
  virtual void destroy();

private:
  WLineEdit *nameEdit_;
  WText     *greeting_;

  std::unique_ptr<QtObject>  qtSender_, qtReceiver_;

  void propagateGreet();
};
