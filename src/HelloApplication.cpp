#include <iostream>

#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

#include "HelloApplication.h"
#include "QtObject.h"

// Needed when using WQApplication with Qt eventloop = true
//#include <QCoreApplication>

using namespace Wt;

HelloApplication::HelloApplication(const WEnvironment& env)
  : WQApplication(env) {
  /*
   * Note: do not create any Qt objects from here. Initialize your
   * application from within the virtual create() method.
   */
}

void HelloApplication::create()
{
  setTitle("Hello world");

  root()->addWidget(std::make_unique<WText>("Your name, please? "));
  nameEdit_ = root()->addWidget(std::make_unique<WLineEdit>());
  nameEdit_->setFocus();

  WPushButton *b = root()->addWidget(std::make_unique<WPushButton>("Greet me."));
  b->setMargin(5, Side::Left);

  root()->addWidget(std::make_unique<WBreak>());

  greeting_ = root()->addWidget(std::make_unique<WText>());

  b->clicked().connect(this, &Dictionary::propagateGreet);
  nameEdit_->enterPressed().connect(this, &Dictionary::propagateGreet);

  qtSender_ = std::make_unique<QtObject>(this);
  qtReceiver_ = std::make_unique<QtObject>(this);

  QObject::connect(qtSender_.get(), SIGNAL(greet(const QString&)),
                   qtReceiver_.get(), SLOT(doGreet(const QString&)));
}

void HelloApplication::destroy()
{
  /*
   * Note: Delete any Qt object from here.
   */
  qtSender_.reset();
  qtReceiver_.reset();
}

void HelloApplication::propagateGreet()
{
  qtSender_->passGreet(toQString(nameEdit_->text()));
}

void HelloApplication::doGreet(const QString& qname)
{
  greeting_->setText("Hello there, " + toWString(qname));
}

std::unique_ptr<WApplication> createApplication(const WEnvironment& env)
{
  return std::make_unique<Dictionary>(env);
}

int main(int argc, char **argv)
{
  // Needed for Qt's eventloop threads to work
  //QCoreApplication app(argc, argv);

  return WRun(argc, argv, &createApplication);
}

