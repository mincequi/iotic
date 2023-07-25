#pragma once

#include <Wt/WApplication.h>
#include <thread>

#include "DispatchThread.h"

class QString;

/*! \class WQApplication WQApplication WQApplication
 *  \brief An application class that provides interopability between
 *         Wt and Qt.
 *
 * This class provides interopability between the Wt's multi threading
 * model and Qt's threading requirements for QObject. This is needed
 * because Qt's object model, which defines a hierarchy of QObjects,
 * requires that every QObject in the hierarchy is created from within
 * the same thread. In addition, Qt's signal/slot system is
 * thread-aware and behaves very differently when a signal is emitted
 * from within a different thread than the thread in which the
 * receiver object lives.
 *
 * Wt on the other hand does not guarantee that every event is
 * dispatched within the same thread. This is a side effect of the
 * fact that Wt uses thread pools in combination with asynchronous I/O
 * to be able to serve multiple connections simultaneously without
 * requiring a high number of threads.
 *
 * Therefore, you cannot manipulate a QObject hierarchy, or propagate
 * events using Qt's signal/slot system, in a multi-threaded Wt
 * application server, since this is likely to violate Qt's
 * thread/object assumptions, without taking precautions (as are
 * implemented in this application class).
 *
 * This class spawns a QThread that is dedicated to a single
 * application instance, and used for event handling, after your
 * application is constructed. You should not create any Qt objects
 * from the constructor, but rather from the create() method, which
 * runs within the context of this thread. Likewise, you should not
 * destroy Qt objects from the application destructor, but from the
 * destroy() method, which also runs in this thread.
 *
 * You may enable a Qt event loop in this QThread, by setting the
 * option in the constructor. In this way, you can use QTcpSocket and
 * other Qt classes which rely on the presence of an event loop. Note
 * that Qt requires that you instantiate a QApplication object before
 * you can use a Qt event loop (only one is needed per process, so it
 * may be shared between multiple Wt sessions). You need to do this
 * yourself, and a convenient location could be within your main()
 * function.
 */
class WQApplication : public Wt::WApplication {
public:
  /*! \brief Constructor.
   *
   * Create a new application with Qt threading support.
   *
   * Note: you should not create Qt objects from within the
   * constructor. Instead, reimplement create(), which is called after
   * construction, from within the QThread.
   */
  WQApplication(const Wt::WEnvironment& env);

protected:

  /*! \brief Initialize Qt objects in your application within the
   *         QThread context.
   *
   * Reimplement this method to construct your Wt widget and Qt object
   * hierarchy within the context of the dedicatd QThread.
   *
   * This method is called from within the library after your
   * application is created.
   */
  virtual void create() = 0;

  /*! \brief Finalize your application within the QThread context.
   *
   * Reimplement this method to safely destroy Qt object hierarchy.
   *
   * This method is called from within the library before your
   * application is deleted.
   */
  virtual void destroy() = 0;

  /*! \brief Notify an event to the application within the QThread
   *         context.
   *
   * This method is the equivalent of WApplication::notify(), but runs
   * inside the QThread context. The default implementation simply
   * calls WApplication::notify().
   */
  virtual void realNotify(const Wt::WEvent& e);

  virtual void notify(const Wt::WEvent& e);
  virtual void initialize();
  virtual void finalize();
  virtual void waitForEvent();

private:
  std::unique_ptr<DispatchThread> thread_;
  bool finalized_ = false;
  bool recursiveEvent_ = false;

  friend class DispatchThread;
};

/*! \brief Conversion function from QString to WString
 *
 * Lossless conversion between these two unicode string classes.
 */
extern Wt::WString toWString(const QString& s);

/*! \brief Conversion function from WString to QString
 *
 * Lossless conversion between these two unicode string classes.
 */
extern QString toQString(const Wt::WString& s);
