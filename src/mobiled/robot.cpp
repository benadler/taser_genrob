#include "robot.h"

// Initialize the singleton-pointer to NULL
QMutex Robot::singletonMutex;
Robot* Robot::singleton = NULL;

Robot* Robot::instance()
{
	QMutexLocker locker(&singletonMutex);

	if(singleton == NULL)
		singleton = new Robot();

	return singleton;
}

Robot::Robot(void)
{
	logger = Logger::instance();
	logger->Robot("Robot::Robot()");

	mutexThread = new QMutex(QMutex::NonRecursive);
	mutexRobot = new QMutex(QMutex::NonRecursive);
	config = Configuration::instance();

	canSocket = new QTcpSocket(this);
	canSocket->connectToHost(QHostAddress::LocalHost, config->getCanServerPort());
	connect(canSocket, SIGNAL(readyRead()), this, SLOT(slotIncomingCanPacket()));
	connect(canSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotSocketStateChanged(QAbstractSocket::SocketState)));

	qRegisterMetaType<RobotStatus>("RobotStatus");

	brakesApplied = true;
	timeStamp = QTime::currentTime();

	// Don't use slotSetStatus(idle), as that would fire an event. That's probably
	// not harmful, but still useless.
	currentRobotStatus = idle;
}

Robot::~Robot(void)
{
	logger->Robot("Robot::~Robot()");

	canSocket->close();
	delete canSocket;
	delete mutexThread;
	delete mutexRobot;
}

void Robot::slotSocketStateChanged(QAbstractSocket::SocketState state)
{
	QMutexLocker locker(mutexThread);

	if(state == QTcpSocket::UnconnectedState)
		abort("Robot::slotSocketStateChanged(): lost connection to CAN-Server, exiting.");

	if(state == QTcpSocket::ConnectedState)
		logger->Robot("Robot::slotSocketStateChanged(): connection to CAN-Server established.");
}

void Robot::setWheelSpeeds(const float speedL, const float speedR)
{
	QMutexLocker locker(mutexThread);

	Q_ASSERT(currentRobotStatus != idle);
	Q_ASSERT(currentRobotStatus != stalled);
	Q_ASSERT(currentRobotStatus != movementSetup);

	// If the brakes are applied, we shouldn't be told to set a speed other than zero.
	Q_ASSERT(! (brakesApplied && (fabs(speedL) > 0.00001 || fabs(speedR) > 0.00001)));

// 	logger->Robot("Robot::setWheelSpeeds(): setting wheelspeeds to %.2F / %.2F.", speedL, speedR);

	Packet setSpeed(CAN_REQUEST | CAN_SET_WHEELSPEEDS);
	setSpeed.pushS32((int)(speedL * 1000000));	// left speed
	setSpeed.pushS32((int)(speedR * 1000000));	// right speed
	setSpeed.send(canSocket);

	// We'd like to know how far the robot really moved. We'll update localization when we get a reply.
	Packet getAdvances(CAN_REQUEST | CAN_WHEELADVANCES);
	getAdvances.send(canSocket);

	// Update localization with STELLwert values. This is probably not a good idea.
	//Localization::instance()->updatePoseUsingWheelSpeeds(speedL, speedR);
}

void Robot::slotSetStatus(const RobotStatus status)
{
	QMutexLocker locker(mutexThread);

	currentRobotStatus = status;

	logger->Robot("Robot::slotSetStatus(): setting currentRobotStatus to %s.", qPrintable(getStatusText()));

	setBrakesToCurrentRobotState();

	emit statusChanged(status);
}

bool Robot::testAndSetStatus(const RobotStatus statusExpected, const RobotStatus statusNew)
{
	QMutexLocker locker(mutexThread);

	if(currentRobotStatus == statusExpected)
	{
		logger->Robot("Robot::testAndSetStatus(): changing status from the expected %s to %s.", qPrintable(getStatusText(statusExpected)), qPrintable(getStatusText(statusNew)));
		currentRobotStatus = statusNew;
		setBrakesToCurrentRobotState();
		return true;
	}

	logger->Robot("Robot::testAndSetStatus(): expected status %s but it was %s instead, not changing to %s.", qPrintable(getStatusText(statusExpected)), qPrintable(getStatusText(currentRobotStatus)), qPrintable(getStatusText(statusNew)));
	return false;
}

void Robot::setBrakesToCurrentRobotState()
{
	// This method is private, so it does not need a mutex-lock. The callers should have taken care of that.

	if(currentRobotStatus == idle || currentRobotStatus == stalled)
	{
		// The robot is NOT moving, apply the brakes.
		setBrakes(true);
	}
	else if(currentRobotStatus == moving)
	{
		// The robot IS moving, release the brakes.
		setBrakes(false);
	}
	else if(currentRobotStatus == aborting || currentRobotStatus == movementSetup)
	{
		// If we're entering one of these states, we need not change the brake-status. Why?
		// For Abortion:
		// - if we WERE moving before, we're now slowing down, so we still need the brakes loose.
		//
		// - if we were NOT moving, we're not going to start moving in the aborting state, so we
		//   can leave the brakes applied.
		// For movementSetup, we only go there from an idle state, so the brakes are applied already.
		logger->Robot("Robot::setBrakesToCurrentRobotState(): NOT changing brake-status, as we've entered aborting or movementSetup-state.");
	}
}

RobotStatus Robot::getStatus(void) const
{
	QMutexLocker locker(mutexThread);

// 	logger->Robot("Robot::getStatus(): returning currentRobotStatus %s.", qPrintable(getStatusText()));
	return currentRobotStatus;
}

QString Robot::getStatusText(void) const
{
	if(currentRobotStatus == idle)
		return "idle";
	if(currentRobotStatus == stalled)
		return "stalled";
	if(currentRobotStatus == movementSetup)
		return "movementSetup";
	if(currentRobotStatus == moving)
		return "moving";
	if(currentRobotStatus == aborting)
		return "aborting";

	return "ERROR";
}

QString Robot::getStatusText(RobotStatus status) const
{
	if(status == idle)
		return "idle";
	if(status == stalled)
		return "stalled";
	if(status == movementSetup)
		return "movementSetup";
	if(status == moving)
		return "moving";
	if(status == aborting)
		return "aborting";

	return "ERROR";
}

void Robot::slotIncomingCanPacket()
{
	QMutexLocker locker(mutexThread);

	// Commands have at least 4 words (=16 byte)
	if(canSocket->bytesAvailable() < 16)
		return;

	// There might be multiple packets waiting in the buffer. Do NOT copy the whole
	// buffer into ONE packet. We have to look for the packetLength first, which is
	// located at bytes 4,5,6,7 (see packet.h documentation for header layout).
	QByteArray packetHeader = canSocket->read(8);
	unsigned int packetLength = *(unsigned int*)(packetHeader.constData() + 4);

	//logger->NetworkServer("CanServer::slotDataReceived(): the packet being received is %d bytes long.", packetLength);

	// we can now read the whole packet into a buffer.
	QByteArray packetComplete(packetHeader);
	packetComplete.append(canSocket->read(packetLength - 8));

	Packet packet;
	packet.setData((const unsigned char*)packetComplete.data(), packetComplete.size());

	// If there is more data waiting (for another packet), then make sure its fetched ASAP.
	if(canSocket->bytesAvailable() > 0)
	{
		logger->Robot("Robot::slotIncomingCanPacket(): There's %d more bytes waiting, re-scheduling myself.", canSocket->bytesAvailable());
		QTimer::singleShot(0, this, SLOT(slotIncomingCanPacket()));
	}
	
	if(!packet.isValid())
	{
		logger->Robot("Robot::slotIncomingCanPacket(): received invalid can-packet, returning.");
		return;
	}

	// what packet is it?
	if(packet.getCommand() == (CAN_REPLY | CAN_BATTERYVOLTAGE))
	{
		float batteryVoltage = packet.popF32();

		logger->Robot("Robot::slotIncomingCanPacket(): received batteryVoltage %.2F from CanServer.", batteryVoltage);
		emit newBatteryVoltageReady(batteryVoltage);
	}

	if(packet.getCommand() == (CAN_REPLY | CAN_MOTORTEMPS))
	{
		float motorTempL = packet.popF32();
		float motorTempR = packet.popF32();

		logger->Robot("Robot::slotIncomingCanPacket(): received motorTemps %.2F/%.2F from CanServer.", motorTempL, motorTempR);
		emit newDriveTempsReady(motorTempL, motorTempR);
	}

	if(packet.getCommand() == (CAN_REPLY | CAN_WHEELADVANCES))
	{
		// We received a packet from the CAN telling us how many micrometers the robot's wheels advanced.
		const int advanceL = packet.popS32();
		const int advanceR = packet.popS32();

		// Convert these advances into speeds. For this, we need a time difference.
		const double timeDiff = (timeStamp.msecsTo(QTime::currentTime()) / 1000.0);
		timeStamp = QTime::currentTime();

		// When the robot has been idle for a long time, timeDiff will be big in this first
		// iteration. Thats not a problem, because that leads to very small speed values.
		// Also, localization checks for unreasonable timeDiffs by itself.
		const int speedL = (int)((double)advanceL / timeDiff);
		const int speedR = (int)((double)advanceR / timeDiff);

		// Localization uses micrometers per second, too.
		Localization::instance()->updatePoseUsingWheelSpeeds(speedL, speedR);
	}

	if(packet.getCommand() == (CAN_REPLY | CAN_BRAKES_ENABLE))
	{
		if(packet.popS32() != 0)
			abort("Robot::slotIncomingCanPacket(): enabling brakes failed, aborting.");
	}

	if(packet.getCommand() == (CAN_REPLY | CAN_BRAKES_DISABLE))
	{
		if(packet.popS32() != 0)
			abort("Robot::slotIncomingCanPacket(): disabling brakes failed, aborting.");
	}
}

void Robot::setBrakes(const bool enable)
{
	// This method remembers the current brake status in "brakesApplied" in order
	// to keep sending can-packets to a minimum. This method is called everytime
	// setStatus() is called, so that would cause a few packets otherwise.

	if((brakesApplied && enable) || (!brakesApplied && !enable))
		return;

	Packet setEmergencyStop;

	if(enable)
	{
			setEmergencyStop.setCommand(CAN_REQUEST | CAN_EMERGENCY_STOP_ENABLE);
			brakesApplied = true;
			logger->Robot("Robot::setBrakes(): brakes applied.");
	}
	else
	{
			setEmergencyStop.setCommand(CAN_REQUEST | CAN_EMERGENCY_STOP_DISABLE);
			brakesApplied = false;
			logger->Robot("Robot::setBrakes(): brakes released.");
	}

	if(! setEmergencyStop.send(canSocket)) abort("Robot::setBrakes(): couldn't send brakes packet, aborting");
}

void Robot::waitUntilIdle(void)
{
	// Do NOT lock the mutex. Not only is there no data to protect in this method, but
	// if we lock the mutexThread here (e.g. from NetworkServer), we cannot call
	// getStatus() anymore. And we need that in MobileD::slotStepMotion().
	//QMutexLocker locker(mutexThread);

	logger->MobileD("Robot::waitUntilIdle(): waiting until robot is idle in thread %d.", syscall(SYS_gettid));

	mutexRobot->lock();

	logger->MobileD("Robot::waitUntilIdle(): got the idle-lock, now unlocking it in thread %d.", syscall(SYS_gettid));

	mutexRobot->unlock();

	logger->MobileD("Robot::waitUntilIdle(): robot is now idle, returning in thread %d.", syscall(SYS_gettid));
}

void Robot::slotGetBatteryVoltage(void) const
{
	QMutexLocker locker(mutexThread);

	// ask via CAN for the battery voltage
	Packet packet(CAN_REQUEST | CAN_BATTERYVOLTAGE);
	logger->MobileD("Robot::getBatteryVoltage(): now asking canserver for voltage in thread %d.", syscall(SYS_gettid));
	packet.send(canSocket);

	// We'll get a reply packet for this eventually and handlePacket()
	// will then emit a signal with the new values.
}

void Robot::slotGetDriveTemperatures() const
{
	QMutexLocker locker(mutexThread);

	// ask via CAN for the temperatures
	Packet packet(CAN_REQUEST | CAN_MOTORTEMPS);
	packet.send(canSocket);

	// We'll get a reply packet for this eventually and handlePacket()
	// will then emit a signal with the new values.
}

void Robot::lock(void)
{
	QMutexLocker locker(mutexThread);
	logger->Robot("Robot::lock() in thread %d.", syscall(SYS_gettid));
	mutexRobot->lock();
}

bool Robot::tryLock(void)
{
	QMutexLocker locker(mutexThread);
	bool success = mutexRobot->tryLock();

// 	if(success)
// 		logger->Robot("Robot::tryLock(): got the lock in thread %d.", syscall(SYS_gettid));
// 	else
// 		logger->Robot("Robot::tryLock(): mutex is already locked in thread %d.", syscall(SYS_gettid));

	return success;
}

void Robot::unlock(void)
{
	QMutexLocker locker(mutexThread);
	logger->MobileD("Robot::unlock() in thread %d.", syscall(SYS_gettid));
	mutexRobot->unlock();
}
