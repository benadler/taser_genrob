#include "beeperthread.h"

BeeperThread::BeeperThread(void) : QThread()
{
	logger = Logger::instance();
	logger->BeeperThread("BeeperThread::BeeperThread()");

	timer = new QTimer(this);
	timer->setInterval(600 + 500);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotBeep()));

	consoleHandle = -1;

	if((consoleHandle = open("/dev/console", O_WRONLY)) == -1)
	{
//		abort("BeeperThread::BeeperThread(): could not open /dev/console for writing, exiting.\n");
//		exit(1);
	}
}

BeeperThread::~BeeperThread(void)
{
	logger->BeeperThread("BeeperThread::~BeeperThread()");
}

void BeeperThread::run(void)
{
	// start the event loop!
	logger->BeeperThread("BeeperThread::run(): starting event loop, thread-id is %d.", syscall(SYS_gettid));
	exec();
}

void BeeperThread::slotBeep(void)
{
	logger->BeeperThread("BeeperThread::slotBeep(): running in thread-id %d", syscall(SYS_gettid));

	// only beep if the robot is moving.
	if(Robot::instance()->getStatus() != moving)
		return;

	logger->BeeperThread("BeeperThread::slotBeep(): robot is rotating or moving, beeping.");

	// alternatively, try this:
	//write(fileno (stderr), "\007", 1);

	// If you're scared by the following, ask google for beep.c. The divisor is the desired frequency.
	if(ioctl(consoleHandle, KIOCSOUND, (int)(1193180/1300)) < 0)
	{
		printf("\a");  /* Output the only beep we can, in an effort to fall back on usefulness */
		perror("BeeperThread::slotBeep(): couldn't beep: ioctl");
	}

	// wait
	usleep(500000);

	// stop the beep
	ioctl(consoleHandle, KIOCSOUND, 0);
	logger->BeeperThread("BeeperThread::slotBeep(): done.");
}

void BeeperThread::slotCollisionAvoidanceStatusChanged(bool active)
{
	if(active)
	{
		logger->BeeperThread("BeeperThread::slotCollisionAvoidanceStatusChanged() to active, stopping beeper timer in thread %d.", syscall(SYS_gettid));
		timer->stop();
	}
	else
	{
		logger->BeeperThread("BeeperThread::slotCollisionAvoidanceStatusChanged() to INactive, starting beeper timer in thread %d.", syscall(SYS_gettid));
		timer->start();
	}
}
