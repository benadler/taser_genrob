#include "mobiled.h"

MobileD::MobileD(int argc, char** argv) : QCoreApplication(argc, argv)
{
	logger = Logger::instance();
	logger->MobileD("MobileD::MobileD(): starting up, thread-id is %d", syscall(SYS_gettid));

	mutexThread = new QMutex();

	robot = Robot::instance();

	java = Java::instance();

	connect(this, SIGNAL(sendWayPointRequestToJava(Pose, Pose)), java, SLOT(slotGetWayPoints(Pose, Pose)), Qt::QueuedConnection);
	connect(java, SIGNAL(wayPointsReady(QList<QPointF>)), this, SLOT(slotNewWayPointsArrived(QList<QPointF>)), Qt::QueuedConnection);

	// for testing, put an obstacle into the first lab-door.
// 	QList<QPointF> obstacles;
// 	obstacles.append(QPointF(14.0, 10.0));
// 	obstacles.append(QPointF(14.5, 10.0));
// 	obstacles.append(QPointF(15.0, 10.0));
// 	QMetaObject::invokeMethod
// 	(
// 		java,
// 		"slotAddObstaclesToMap",
// 		Qt::QueuedConnection,
// 		Q_ARG(QList<QPointF>, obstacles)
// 	);

// 	sleep(4);
	//java->slotSetIoWarriorPort(3, true);
// 	logger->MobileD("MobileD::MobileD(): now calling slotSetIoWarriorPort");
// 	QMetaObject::invokeMethod
// 	(
// 		java,
// 		"slotSetIoWarriorPort",
// 		Qt::QueuedConnection,
// 		Q_ARG(unsigned int, 3),
// 		Q_ARG(bool, true)
// 	);
// 	logger->MobileD("MobileD::MobileD(): done.");

// 	QMetaObject::invokeMethod
// 	(
// 		java,
// 		"slotRemoveObstaclesFromMap",
// 		Qt::QueuedConnection,
// 		Q_ARG(QList<QPointF>, obstacles)
// 	);

	laserScanner = LaserScanner::instance();
	laserScanner->moveToThread(laserScanner);
	laserScanner->start();

	config = Configuration::instance();

	// start the motor fans
	QMetaObject::invokeMethod
	(
		java,
		"slotSetIoWarriorPort",
		Qt::QueuedConnection,
		Q_ARG(unsigned int, config->getIoWarriorPortMotorFan()),
		Q_ARG(bool, true)
	);

	localization = Localization::instance();

	collisionAvoidance = CollisionAvoidance::instance();
	collisionAvoidance->setMobileD(this);
	connect(collisionAvoidance, SIGNAL(alertPathBlocked()), this, SLOT(slotAlertPathBlocked()), Qt::QueuedConnection);

	networkServer = NetworkServer::instance();
	networkServer->setMobileD(this);

	poseTarget.setPose(0.0, 0.0, 0.0);

	pathIsBlocked = false;

	speedL = speedR = 0.0;

	stepMotionTimer = new QTimer();
	stepMotionTimer->setInterval(32);
	connect(stepMotionTimer, SIGNAL(timeout()), this, SLOT(slotStepMotion()));
	stepMotionTimer->start();

	motion = 0;

	// for debugging.
	//QTimer::singleShot(4000, this, SLOT(slotShutDown()));

gettimeofday (&start, 0x0);
}

MobileD::~MobileD(void)
{
	logger->MobileD("MobileD::~MobileD(): shutting down.");

	delete mutexThread;
	delete stepMotionTimer;
}

void MobileD::slotShutDown()
{
	logger->MobileD("MobileD::slotShutDown().");

	// stop the motion timer
	stepMotionTimer->stop();

	logger->MobileD("MobileD::slotShutDown(): stopping motor fans.");
	QMetaObject::invokeMethod
	(
		java,
		"slotSetIoWarriorPort",
		Qt::QueuedConnection,
		Q_ARG(int, config->getIoWarriorPortMotorFan()),
		Q_ARG(bool, false)
	);

	logger->MobileD("MobileD::slotShutDown(): shutting down laserScanner thread.");
	laserScanner->quit();
	laserScanner->wait();
	laserScanner->shutdown();
// 	delete laserScanner;

	logger->MobileD("MobileD::slotShutDown(): shutting down networkServer thread.");
	networkServer->quit();
	networkServer->wait();
	delete networkServer;

	logger->MobileD("MobileD::slotShutDown(): shutting down java thread.");
	java->quit();
	java->wait();
	delete java;

	// exit mobiled
	quit();
}

void MobileD::slotStepMotion(void)
{
	if(robot->getStatus() == idle) return;
/*
	static QMap<int,int> timingMap;

	if(timeInited)
	{
		struct timeval now;
		gettimeofday (&now, 0x0);

		int us =
			(now.tv_usec - start.tv_usec) +
			((now.tv_sec - start.tv_sec) > 0 ? 1000000 : 0);

		gettimeofday (&start, 0x0);

		static int usSum = 0, usCnt = 0;

		timingMap[us]++;

		usSum += us;
		usCnt++;

		int avg = usSum / usCnt;

// 		logger->MobileD("MobileD::stepMotion(): iteration %d had interval %d microsecs, avg is %d.", usCnt, us, avg);
	}
	else
	{
		gettimeofday (&start, 0x0);
		timeInited = true;
	}*/

	QMutexLocker locker(mutexThread);

	if(robot->getStatus() == movementSetup)
	{
		// The robot's status was set to movementSetup. In the case of a Rotation or a Translation,
		// there is nothing to set up, so we can immediately change to "moving". This is just useful
		// because another thread can set the status to "movementSetup", and this thread can then
		// change to "moving". This is necessary, because changing the status to "moving" releases
		// the brakes in Robot::setBrakes(), which sends a CAN-packet via TCP, which MUST be done
		// in the same thread that created the QTcpSocket object. Which is this MobileD thread. Whew.

		if(
			motion != 0
			&&
			! motion->isFinished()
			&&
			(motion->getType() == rotation || motion->getType() == translation)
		  )
		{
			robot->slotSetStatus(moving);
		}

		// In the case of a Spline-motion, things are different. We have just asked for a path, and
		// when slotNewWayPointsArrived(QList<QPointF> wayPoints) has a valid path, it'll build the
		// spline and set the status to "moving" by itself. Thus, do nothing.
	}

	if(robot->getStatus() == moving)
	{
// 		logger->MobileD("MobileD::stepMotion(): moving.");

		// Lock the robot if it isn't already.
		robot->tryLock();

		if(motion->step(localization->getPose(), speedL, speedR))
		{
			// Motion::step() returned true, so we're not done.

			int aLevel = std::max((int)laserScanner->getAlarmLevel(sideFront), (int)laserScanner->getAlarmLevel(sideRear));
			// alarmLevel goes from 0 (green) to 2 (red), offset by 1, so we can divide by that.
			aLevel++;
// 			speedL /= aLevel;
// 			speedR /= aLevel;

			robot->setWheelSpeeds(speedL, speedR);
		}
		else
		{
			// Motion::step() returned true, so we're done.
			logger->MobileD("MobileD::stepMotion(): done moving, setting wheelSpeeds to 0.0 and robot to idle.");
			robot->setWheelSpeeds(speedL, speedR);
			robot->slotSetStatus(idle);

			// unlock the robot's mutex!
			robot->tryLock();
			robot->unlock();

			// spit out timing stats at the end
// 				qDebug("time\tcount:");
// 				for(int i = 31000;i < 34000;i++)
// 					qDebug() << i << "\t" << timingMap[i];

		}
	}
	if(robot->getStatus() == aborting)
	{
// 		logger->MobileD("MobileD::stepMotion(): aborting...");

		// We are here because we aborted a motion. Independent of whether it was aborted due
		// to collision avoidance or due to the user pressing STOP, we do not want to continue
		// this motion ever again.
		// We need to set this motion to finished now. If we don't, the followig will happen:
		// This motion (e.g. Translation) is aborted (but not finished) and we go to idle. A
		// new SplineMove comes in, we go to movementSetup-Phase and wait for waypoints. Then
		// the timer starts slotStepMotion, which goes into movementSetup, sees an existing
		// AND unfinished Translation and thus goes to that Translations "moving" state. Instead,
		// we wanted to drive a spline!
		motion->setFinished(true);

		// How many meters/s should we decelerate in this iteration?
		const double speedDifference = config->getAbortMotionDeceleration() / 1000.0 * stepMotionTimer->interval();

		// If we've halted already (speed is smaller than what we would decelerate
		// in this iteration), apply brakes and stop movement loop.
		if(fabs(speedL) < speedDifference && fabs(speedR) < speedDifference)
		{
			logger->MobileD("MobileD::stepMotion(): wheelspeeds are below threshold, done aborting.");

			// We might have gone e.g. idle->movementSetup->aborting, without ever having moved, so the
			// brakes might still be set. But its okay to tell Robot to move with 0/0 when brakes are set.
			robot->setWheelSpeeds(0.0, 0.0);

			if(pathIsBlocked)
			{
				// The path is blocked, we were aborting and are now done. Go into stalled state.
				// Keep the timer going, as in the next shot we'll ask for a new path.
				logger->MobileD("MobileD::stepMotion(): aborting, pathIsBlocked, setting currentRobotStatus to stalled.");
				robot->slotSetStatus(stalled);

				// reset this value.
				pathIsBlocked = false;
			}
			else
			{
				// We were aborting, but the path is not blocked. Thus, we were simply told to abort. Go into idle state.
				logger->MobileD("MobileD::stepMotion(): aborting, !pathIsBlocked, setting currentRobotStatus to idle.");
				robot->slotSetStatus(idle);

				// Unlock the robotmutex in case it is locked. It might be unlocked e.g. when
				// going from idle to aborting, which can happen when the client sends a CMD_STOP
				// We may not unconditionally unlock it, as unlocking an unlocked mutex seems to give:
				// ASSERT failure in QMutex::unlock(): "A mutex must be unlocked in the same thread that locked it."
				robot->tryLock();
				robot->unlock();
			}
		}
		else
		{
			// Slow down further (proportionally)...
			// TODO: there's a DANGEROUS bug somewhere here, leading to funky wheelspeeds!!!
			// So this method should be cleaned up!
			/*
03:06:13.694576 Conversion::correctWheelSpeeds(): front left: factor angle 0.967, distance 0.01, combined 0.0090.
03:06:13.694586 Robot::setWheelSpeeds(): setting wheelspeeds to 353268 / 400718.
03:06:13.694693 Localization::updatePoseUsingOdometry(): timeDiff 0.0320s, distance: 0.012063m  angle: 0.146176deg
03:06:13.694737 Localization::updatePoseUsingOdometry(): Pose: x10.3940, y11.2380, angle55.9458, varX=1.422494, varY=0.322301, varT=0.102294 [rad]
03:06:13.717355 NetworkServer::stopMotion()
03:06:13.717394 Robot::waitUntilIdle(): waiting until robot is idle in thread 24433.
03:06:13.717437 Robot::slotSetStatus(): setting currentRobotStatus to aborting.
03:06:13.717448 Robot::setBrakesToCurrentRobotState(): NOT changing brake-status, as we've entered aborting-state.
03:06:13.726465 MobileD::stepMotion(): aborting...
03:06:13.726515 MobileD::stepMotion(): aborting, braking down to -2147.13 / 0.38 meters per second.
03:06:13.726555 Robot::setWheelSpeeds(): setting wheelspeeds to -2147130380 / 376718.
03:06:13.726668 Localization::updatePoseUsingOdometry(): timeDiff 0.0320s, distance: 34.050570m  angle: 90.496071deg
03:06:13.726715 Localization::updatePoseUsingOdometry(): Pose: x-8.6715, y-16.9746, angle146.4418, varX=62.500404, varY=40.509830, varT=0.102295 [rad]
03:06:13.726742 BeeperThread::slotCollisionAvoidanceStatusChanged() to active, stopping beeper timer in thread 24426.
03:06:13.758464 MobileD::stepMotion(): aborting...
03:06:13.758513 MobileD::stepMotion(): aborting, braking down to -2147.11 / 0.38 meters per second.
03:06:13.758531 Robot::setWheelSpeeds(): setting wheelspeeds to -2147106380 / 376714.
03:06:13.758631 Localization::updatePoseUsingOdometry(): timeDiff 0.0320s, distance: 34.050186m  angle: 90.496077deg
03:06:13.758677 Localization::updatePoseUsingOdometry(): Pose: x19.7048, y-35.7948, angle236.9379, varX=98.741592, varY=7.721491, varT=0.102295 [rad]
03:06:13.758712 BeeperThread::slotCollisionAvoidanceStatusChanged() to active, stopping beeper timer in thread 24426.
03:06:13.790465 MobileD::stepMotion(): aborting...
03:06:13.790522 MobileD::stepMotion(): aborting, braking down to -2147.08 / 0.38 meters per second.
03:06:13.790541 Robot::setWheelSpeeds(): setting wheelspeeds to -2147082380 / 376710.
03:06:13.790655 Localization::updatePoseUsingOdometry(): timeDiff 0.0320s, distance: 34.049802m  angle: 90.496082deg
03:06:13.790702 Localization::updatePoseUsingOdometry(): Pose: x38.2785, y-7.2570, angle327.4340, varX=182.043549, varY=43.019470, varT=0.102296 [rad]
03:06:13.790738 BeeperThread::slotCollisionAvoidanceStatusChanged() to active, stopping beeper timer in thread 24426.
			*/

			// get the wheelSpeed ratios. For really small speed, use 1.0
			// instead of dealing with INF and other ugly values.
			// BUGGY, as 0.0000000001 / 5 shouldn't lead to ratio 1.0!
			// BUGGY, as 5 / 0.0000000001 shouldn't lead to ratio 1.0!
			double speedRatio;
			if(fabs(speedL) > 0.000001 && fabs(speedR) > 0.000001)
				speedRatio = fabs(speedL) / fabs(speedR);
			else
				speedRatio = 1.0;

			double speedFactorRatioL = 1.0, speedFactorRatioR = 1.0;

			// If the two speeds are really similar, then
			if(speedRatio > 1.0)
			{
				// left goes $speedRatio times faster
				speedFactorRatioR = 1.0 / speedRatio;
				qDebug("MobileD::stepMotion(): abrtng, speedRatio is > 1.0");
			}
			else
			{
				// right goes $speedRatio times faster
				speedFactorRatioL = 1.0 / speedRatio;
				qDebug("MobileD::stepMotion(): abrtng, speedRatio <= 1.0");
			}

			float speedNewL, speedNewR;

			if(fabs(speedL) < 0.00001)
				speedNewL = 0.0;
			else if(speedL > 0.0)
				speedNewL = speedL - speedDifference * speedFactorRatioL;
			else if(speedL < 0.0)
				speedNewL = speedL + speedDifference * speedFactorRatioL;

			if(fabs(speedR) < 0.00001)
				speedNewR = 0.0;
			else if(speedR > 0.0)
				speedNewR = speedR - speedDifference * speedFactorRatioR;
			else if(speedR < 0.0)
				speedNewR = speedR + speedDifference * speedFactorRatioR;

			qDebug
			(
				"MobileD::stepMotion(): spdDiff %.4F, spdRatio %.4F, RatioL %.4F, RatioR %.4F, braking %.4F / %.4F to %.4F / %.4F",
				speedDifference,
				speedRatio,
    				speedFactorRatioL,
				speedFactorRatioR,
				speedL,
				speedR,
				speedNewL,
				speedNewR
			);

			// We might set one wheel from a very small speed to a very small negative speed. Keep that from happening.
			if(getSign(speedL) != getSign(speedNewL))
			{
				qDebug("aborting changed sign of wheel L, old %.8F, new %.8F, setting new speedL to 0.", speedL, speedNewL);
				speedNewL = 0.0;
			}

			if(getSign(speedR) != getSign(speedNewR))
			{
				qDebug("aborting changed sign of wheel R, old %.8F, new %.8F, setting new speedR to 0.", speedR, speedNewR);
				speedNewR = 0.0;
			}

			// This should alleviate the weird bug from above.
			if(!(fabs(speedNewL) <= fabs(speedL)))
				abort("aborting didn't decelerate wheel L, old %.8F, new %.8F.", speedL, speedNewL);
			if(!(fabs(speedNewR) <= fabs(speedR)))
				abort("aborting didn't decelerate wheel R, old %.8F, new %.8F.", speedR, speedNewR);

			speedL = speedNewL;
			speedR = speedNewR;

			robot->setWheelSpeeds(speedL, speedR);
		}
	}
	else if(robot->getStatus() == stalled)
	{
		// ask Java / genPath for a new list of waypoints. The answer
		// will call slotNewWayPointsArrived() and we continue there.
		if(motion->getType() == spline)
		{
			logger->MobileD("MobileD::stepMotion(): stalled, waiting for obstacles to be put into map.");
			sleep(5);
			slotSendWayPointRequest();

			// After sending the request, go into state movementSetup IF we're still in state "stalled"
			// This might have changed between the last robot-status-check and now, e.g. due to the
			// user pressing CMD_STOP
			if(robot->testAndSetStatus(stalled, movementSetup))
			{
				logger->MobileD("MobileD::stepMotion(): stalled, asked wayPointServer, setting currentRobotStatus to movementSetup.");
			}
			else
			{
				logger->MobileD("MobileD::stepMotion(): asked wayPointServer, but status wasn't stalled anymore, staying in status %s.", qPrintable(robot->getStatusText()));
			}
		}
		else
		{
			// TODO: what to do when the other motions have stalled? we need to continue somewhen!
		}
	}
}

void MobileD::slotSendWayPointRequest(void)
{
	// Ask the wayPointServer for waypoints from current pose to target pose.
	// This method is a slot without arguments, so it can be called from a QTimer,
	// which happens when the wayPointServer cannot find a path and is asked again
	// after a certain timeout.

	// The caller is responsible for setting currentRobotStatus to "movementSetup"!
	if(robot->getStatus() == stalled || robot->getStatus() == idle || robot->getStatus() == movementSetup)
	{
		logger->MobileD("MobileD::slotSendWayPointRequest(): requesting waypoints from genPath using Java/JNI...");
		emit sendWayPointRequestToJava(localization->getPose(), poseTarget);
	}
	else
	{
		qDebug("MobileD::slotSendWayPointRequest(): \n\nWARNING, I am in state %s\n\n", qPrintable(robot->getStatusText()));
	}
}

void MobileD::slotNewWayPointsArrived(QList<QPointF> wayPoints)
{
	QMutexLocker locker(mutexThread);

	// If the wayPointServer could not find a path, the list will
	// be empty. In that case, ask again until we get interrupted.
	if(wayPoints.size() == 0)
	{
		// Only retry if we haven't aborted meanwhile.
		if(robot->getStatus() == movementSetup)
		{
			logger->MobileD("MobileD::slotNewWayPointsArrived(): received empty waypoint-list, no path was found. Retrying.");
			QTimer::singleShot(config->getPathPlannerPollInterval() * 1000, this, SLOT(slotSendWayPointRequest()));
		}
		return;
	}

	foreach(const QPointF &wayPoint, wayPoints)
	{
		logger->MobileD("received waypoint: %.2F / %.2F", wayPoint.x(), wayPoint.y());
	}

	// delete the old motion if its defined
	if(motion != 0)
	{
		delete motion;
		motion = 0;
	}

	// Create a spline (takes time!) from the given waypoints ONLY IF the status is still in movementSetup.
	// The status might also be idle, because we might have aborted and become idle in the meantime.
	if(robot->getStatus() == movementSetup)
	{
		logger->MobileD("MobileD::slotNewWayPointsArrived(): received wayPoints are valid, status is still movementSetup, building spline...");
		motion = new SplineCubic(wayPoints, poseTarget.getAngleDeg());
// 		((SplineHermite*)motion)->setParameters(0.75, 0);
		logger->MobileD(
			"MobileD::slotNewWayPointsArrived(): spline length is %.2F, angle at start is %.2F, point at start is %.2F/%.2F.",
		                 ((Spline*)motion)->getLength(),
		                 ((Spline*)motion)->getAngleDeg(0.0),
		                 ((Spline*)motion)->evaluate0(0.0).x(),
		                 ((Spline*)motion)->evaluate0(0.0).y()
		);
	}
	else
	{
		logger->MobileD("MobileD::slotNewWayPointsArrived(): got waypoints, but we're not waiting for a path anymore. Discarding.");
		return;
	}

	// At this point, we have constructed a new Spline. Thats great, but during the long time it took to create the
	// spline, the NetworkServer might have been called to abort the motion and thus sent a queued event to Robot's
	// slotSetStatus()-slot in order to set the status to aborting.
	// Because we would shortly set the robot's status to moving (loosening the brakes), we should now check
	// to make sure there's no event waiting for Robot to abort the motion. Because if there is, we MUST NOT losen
	// the brakes. Why? After queuing the abort-event to Robot's slotSetStatus()-slot, NetworkServer's stopMotion()
	// method returns a CMD_STOP-acknowledgement-packet to the client. Thereafter, the robot MUST NEVER MOVE.
	//
	// So, we call processEvents() so that Robot's setStatus()-slot gets a chance to receive the queued event and
	// set the status to aborting. As processEvents() could cause the stepMotionTimer to fire first, we first stop
	// and later re-start the motionTimer.

	logger->MobileD("MobileD::slotNewWayPointsArrived(): allowing robot to change status from queued event before commencing...");
	stepMotionTimer->stop();
	processEvents();
	stepMotionTimer->start();

	// If - after building the spline - we still haven't aborted, initiate the motion. There's one problem:
	// Imagine we ask robot for its current status and it says "movementSetup". Thus, we move on to set its
	// status to "moving" and think everything is fine.
	// But what if NetworkServer (different thread) sets the RobotStatus to "aborting" EXACTLY between us
	// asking and us setting the new status? Its action would get lost, which means the robot would be moving
	// along the whole path although it should not. That'd be a potential catastrophe...
	// To fix this, we use an atomic test-and-set. Yes, I'm feeling smart now :)
	if(robot->testAndSetStatus(movementSetup, moving))
	{
		logger->MobileD("MobileD::slotNewWayPointsArrived(): spline-motion built successfully, status still was movementSetup, started motion.");
	}
	else
	{
		logger->MobileD("MobileD::slotNewWayPointsArrived(): spline-motion built successfully, but we're not waiting for a path anymore. Discarding.");
	}
}

bool MobileD::moveTranslate(const float distance)
{
	QMutexLocker locker(mutexThread);

	if(robot->testAndSetStatus(idle, movementSetup))
	{
		motion = new Translation(distance);
		logger->MobileD("MobileD::moveTranslate(): successfully created translation, set robot from idle to moving.");
		return true;
	}
	else
	{
		logger->MobileD("MobileD::moveTranslate(): cannot move, RobotStatus was NOT idle.");
		return false;
	}
}

bool MobileD::moveRotate(const float angleRad)
{
	QMutexLocker locker(mutexThread);

	if(robot->testAndSetStatus(idle, movementSetup))
	{
		motion = new Rotation(angleRad);
		logger->MobileD("MobileD::moveRotate(): successfully created rotation, set robot from idle to moving.");
		return true;
	}
	else
	{
		logger->MobileD("MobileD::moveRotate(): cannot move, RobotStatus was NOT idle.");
		return false;
	}
}

bool MobileD::moveSpline(const Pose target)
{
	QMutexLocker locker(mutexThread);

	if(robot->testAndSetStatus(idle, movementSetup))
	{
		poseTarget = target;
		slotSendWayPointRequest();
		logger->MobileD("MobileD::moveSpline(): returning true.");
		return true;
	}
	else
	{
		logger->MobileD("MobileD::moveSpline(): cannot move, RobotStatus was NOT idle.");
		return false;
	}
}

bool MobileD::moveSpline(const QList<QPointF> &wayPoints, const float targetOrientationRad)
{
	QMutexLocker locker(mutexThread);

	// This is only allowed when the robot is idle.
	if(robot->getStatus() == idle)
	{
		robot->unlock();

		// Construct the target pose from the last wayPoint.
		poseTarget.setX(wayPoints.last().x());
		poseTarget.setY(wayPoints.last().y());
		poseTarget.setAngleRad(targetOrientationRad);

		// Call the slot that is normally called when a new path arrived from the waypointserver.
		// We'll just pretend to be that waypointserver, the effect hopefully is the same.
		slotNewWayPointsArrived(wayPoints);

		return true;
	}
	else
	{
		logger->MobileD("MobileD::moveSpline(): cannot move, RobotStatus is NOT idle.");
		return false;
	}
}

void MobileD::slotAbortCurrentMotion(void)
{
	// No need for a mutex, as Robot's lock()-methods are mutexed, and we don't touch anything else.
	//QMutexLocker locker(mutexThread);

	if(robot->getStatus() != idle)
	{
		logger->MobileD("MobileD::slotAbortCurrentMotion(): aborting current motion.");
		robot->slotSetStatus(aborting);
	}
	else
	{
		logger->MobileD("MobileD::slotAbortCurrentMotion(): robot is idle, no need to abort.");
	}

	logger->MobileD("MobileD::slotAbortCurrentMotion(): returning.");
}


void MobileD::slotAlertPathBlocked(void)
{
	QMutexLocker locker(mutexThread);

	logger->MobileD("MobileD::alertPathBlocked(): setting pathIsBlocked to true, RobotStatus to aborting.");
	pathIsBlocked = true;
	robot->slotSetStatus(aborting);
}

Motion* MobileD::getCurrentMotion(void) const
{
	QMutexLocker locker(mutexThread);

	// this might be a null-pointer!
	return motion;
}

bool MobileD::getSign(float value)
{
	if(value >= 0)
		return true;
	else if(value < 0)
		return false;
	else
		abort("wow, value %.20F is neither pos nor neg!", value);
}

// We create mobiled here and not in main, so that we can delete it in catch_int()
// This means that MobileD::~MobileD() will be called, which cleanly shuts down.
MobileD* mobileD;

int main(int argc, char* argv[])
{
	Logger::setupLogging(argc, argv);
	mobileD = new MobileD(argc, argv);
	return mobileD->exec();
}

