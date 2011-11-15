#include "signalthread.h"

SignalThread::SignalThread(void) : QThread()
{
	logger = Logger::instance();
	logger->SignalThread("SignalThread::SignalThread()");
}

SignalThread::~SignalThread(void)
{
	logger->SignalThread("SignalThread::~SignalThread()");
}

void SignalThread::run(void)
{
	sigset_t signalSet;
	int signal;

	sigaddset(&signalSet, SIGINT);
	sigfillset(&signalSet);

	logger->SignalThread("SignalThread::run(): now waiting for signal in thread %d.", syscall(SYS_gettid));
	sigwait(&signalSet, &signal);
	logger->SignalThread("SignalThread::run(): got signal %d in thread %d.", signal, syscall(SYS_gettid));
	emit shutdown();
}
