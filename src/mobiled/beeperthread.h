#ifndef BEEPERTHREAD_H
#define BEEPERTHREAD_H

#include <QTimer>
#include <QThread>

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/kd.h>

#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "logger.h"
#include "robot.h"

/// @brief This thread uses the pc speaker to beep periodically.
///
/// The periodic beeping is used to alarm the environment when collisionAvoidance
/// is disabled. So at least you get to hear that the robot is out to get you...

class BeeperThread : public QThread
{
	Q_OBJECT

	private:
		Logger* logger;
		QTimer* timer;
		int consoleHandle;

	public:
		/// Constructor.
		BeeperThread(void);

		/// Destruktor.
		~BeeperThread(void);

		void run(void);

	private slots:
		void slotBeep(void);

	public slots:
		void slotCollisionAvoidanceStatusChanged(bool active);
};

#endif
