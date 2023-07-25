#include "DispatchObject.h"

#include "DispatchThread.h"

DispatchObject::DispatchObject(DispatchThread *thread)
  : thread_(thread) {
  connect(this, SIGNAL(doEvent()), this, SLOT(onEvent()));
}

void DispatchObject::propagateEvent()
{
  emit doEvent();
}

void DispatchObject::onEvent()
{
  thread_->doEvent();
}
