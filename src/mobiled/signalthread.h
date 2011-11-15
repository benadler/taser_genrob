#ifndef SIGNALTHREAD_H
#define SIGNALTHREAD_H

#include <QTimer>
#include <QThread>

#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "logger.h"

/// @brief This thread receives unix signals and forwards them to mobileD.
///
/// MobileD shut shut down cleanly when the user kills it using CTRL-C. As mobileD
/// is a multithreaded application, using "signal(SIGINT, catch_int);" and friends
/// is not enough. In the first (mobiled-)thread, we block all signals, so this
/// setting is inherited by all spawned threads. Only in this thread, we enable
/// catching these unix-signals, so that we can then emit a shutdown qt-signal. Whew.
///
/// http://lists.trolltech.com/qt-interest/2006-01/thread00455-0.html

class SignalThread : public QThread
{
	Q_OBJECT

	private:
		Logger* logger;

	signals:
		void shutdown();

	public:
		/// Constructor.
		SignalThread(void);

		/// Destruktor.
		~SignalThread(void);

		void run(void);


};

#endif
