#include "drive.h"

static const int encoderIncrementPerRevolution = 4096;
static const double dVelMeasPeriod = 500.0;
static const double dRevMotorPerRevGear = 1.0;
static const double dGearRatio = 37.0;

static const double dAngGearRadToAngEncIncr = encoderIncrementPerRevolution * dRevMotorPerRevGear * dGearRatio / (2.0 * M_PI);

static inline int radPerSecond_2_encoderIncrementPeriod(double radPerSecond)
{
	return (int)(radPerSecond * dAngGearRadToAngEncIncr / dVelMeasPeriod);
}

static inline double encoderAngle_2_rad(int iEncIncr)
{
	return (double)iEncIncr / dAngGearRadToAngEncIncr;
}

static inline double encoderIncrementPeriod_2_radPerSecond(int iEncVelIncrPeriod)
{
	return (double)iEncVelIncrPeriod / dAngGearRadToAngEncIncr * dVelMeasPeriod;
}

double Drive::meter_2_radiant(double meter, RobotSide side)
{
	return meter / config->getWheelCircumference(side) * 2 * M_PI;
}

double Drive::radiant_2_meter(double radiant, RobotSide side)
{
	return radiant * 2 * M_PI * config->getWheelCircumference(side);
}


Drive::Drive()
{
	_logger = Logger::instance();
	_logger->Drive("Drive::Drive()");

	config = Configuration::instance();

	// this was previously called MAGIC and was defined to be either 1 or 0.987
	_multiplier = 0.987;

	debugAngleRight = 0.0;debugPathLengthRight = 0.0;

// 	disconnectFromControllers();

	connectToControllers();

	_emergencyStopEnabled = false;
}

Drive::~Drive()
{
	_logger->Drive("Drive::~Drive()");

	disconnectFromControllers();
}

void Drive::shutdown(void)
{
	_logger->Drive("Drive::shutdown()");
	disconnectFromControllers();
}

void Drive::disconnectFromControllers(void)
{
	_logger->Drive("Drive::disconnectFromControllers()");

	// Before disconnecting, make sure brakes are applied...
	setBrakeEnabledLeft(true);
	setBrakeEnabledRight(true);

	// ... and motors disabled
	setMotorEnabledLeft(false, 0);
	setMotorEnabledRight(false, 0);

	can->lock();
	can->send(CanMessage(CAN_ID_IO_CMD, CMD_IOBOARD_DISCONNECT));
	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_DISCONNECT));
	can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_DISCONNECT));
	can->unlock();

	sleep (1);
}

void Drive::connectToControllers(void)
{
	_logger->Drive("Drive::connectToControllers()");

	can->lock();

	can->send(CanMessage(CAN_ID_IO_CMD, CMD_IOBOARD_CONNECT));
	CanMessage message1(CAN_ID_IO_REPLY, CMD_IOBOARD_CONNECT);
	can->receiveTimeout(&message1);

	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_CONNECT));
	CanMessage message2(CAN_ID_LEFT_REPLY, CMD_MOTCTRL_CONNECT);
	can->receiveTimeout(&message2);

	can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_CONNECT));
	CanMessage message3(CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_CONNECT);
	can->receiveTimeout(&message3);

	can->unlock();
}

void Drive::setBrakeEnabledLeft(const bool enable)
{
	_logger->Drive("Drive::setBrakeEnabledLeft(enable: %d)", enable);
	can->lock();
	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_DISABLEBRAKE, !enable));
	// Absorb the answer from CAN.
	can->receiveNextTimeout();
	can->unlock();
}

void Drive::setBrakeEnabledRight(const bool enable)
{
	_logger->Drive("Drive::setBrakeEnabledRight(enable: %d)", enable);
	can->lock();
	can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_DISABLEBRAKE, !enable));
	// Absorb the answer from CAN.
	can->receiveNextTimeout();
	can->unlock();
}


void Drive::setMotorEnabledLeft(const bool enable, const int torque)
{
	_logger->Drive("Drive::setMotorEnabledLeft(enable: %d)", enable);
	can->lock();
	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_ENABLEMOTOR, enable, torque));
	sleep(1);
	// Absorb the answer from CAN.
	can->receiveNextTimeout();
	can->unlock();
}

void Drive::setMotorEnabledRight(const bool enable, const int torque)
{
	_logger->Drive("Drive::setMotorEnabledRight(enable: %d)", enable);
	can->lock();
	can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_ENABLEMOTOR, enable, torque));
	sleep(1);
	// Absorb the answer from CAN.
	can->receiveNextTimeout();
	can->unlock();
}


void Drive::syncMotorLeft(const int torque)
{
	_logger->Drive("Drive::syncMotorLeft()");

	can->lock();
	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_SYNCHMOTOR, torque));
	// Warning: I'm not sure what ID/CMD the answer packet will have, need to find out!
	sleep(1);
	CanMessage message(CAN_ID_LEFT_REPLY, CMD_MOTCTRL_SYNCHMOTOR);
	can->receiveTimeout(&message, 5000);
	can->unlock();

	int iEncPosMeasured = (message.getData(0) << 8) | message.getData(1);
	int iEncPosExpected = (message.getData(2) << 8) | message.getData(3);

	_logger->Drive("Drive::syncMotorLeft(): iEncPosMeas = %i, iEncPosExp = %i", iEncPosMeasured, iEncPosExpected);
}


void Drive::syncMotorRight(const int torque)
{
	_logger->Drive("Drive::syncMotorRight()");

	can->lock();
	can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_SYNCHMOTOR, torque));
	// Warning: I'm not sure what ID/CMD the answer packet will have, need to find out!
	sleep(1);
	CanMessage message(CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_SYNCHMOTOR);
	can->receiveTimeout(&message, 5000);
	can->unlock();

	int iEncPosMeasured = (message.getData(0) << 8) | message.getData(1);
	int iEncPosExpected = (message.getData(2) << 8) | message.getData(3);

	_logger->Drive("Drive::syncMotorRight(): iEncPosMeas = %i, iEncPosExp = %i", iEncPosMeasured, iEncPosExpected);
}

void Drive::enableCommutationLeft(void)
{
	_logger->Drive("Drive::enableCommLeft()");
	can->lock();
	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_ENABLECOMM));
	can->receiveNextTimeout();
	can->unlock();
}

void Drive::enableCommutationRight(void)
{
	_logger->Drive("Drive::enableCommRight()");
	can->lock();
	can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_ENABLECOMM));
	can->receiveNextTimeout();
	can->unlock();
}

void Drive::startMotors(void)
{
	_logger->Drive("Drive::startMotors()");

	const int torque = 70;

	// init left side
	setBrakeEnabledLeft(false);
	usleep(500000);
	setMotorEnabledLeft(true, torque);
	usleep(500000);
	syncMotorLeft(torque);
	enableCommutationLeft();
	initializeAngleLeft();

	// init right side
	setBrakeEnabledRight(false);
	usleep(500000);
	setMotorEnabledRight(true, torque);
	usleep(500000);
	syncMotorRight(torque);
	enableCommutationRight();
	initializeAngleRight();

	_logger->Drive("Drive::startMotors(): done CANing, absorbing packets.");

	// It seems that after issuing all the commands above, we get some crap back from CAN. Absorb these messages.
	CanMessage message;
	can->lock();
	while(can->receiveNextTimeout(&message, 1000) == true)
	{
		unsigned int cmd = message.getData(7) >> 2;
		_logger->Drive("Drive::startMotors: bogus reply with cmd 0x%02x, id 0x%04x, error 0x%02x received.", cmd, message.getId(), message.getError());
	}
	can->unlock();
	_logger->Drive("Drive::startMotors(): done.");
}

bool Drive::setBrakesEnabled(const bool enable)
{
	#warning: have a look at the motor state diagram and startMotors(). we might need to re-int some stuff after disabling the brakes.
	setBrakeEnabledLeft(enable);
	setBrakeEnabledRight(enable);
	return true;
}

void Drive::setMotorSpeeds(int speedLeft, int speedRight)
{
	if(speedLeft < -4000000 || speedRight < -4000000 || speedLeft > 4000000 || speedRight > 4000000)
		abort("Drive::setMotorSpeeds(): left/right speeds must be between -4 and 4 m/s, they were %e/%e", speedLeft, speedRight);

	setMotorSpeedLeft(speedLeft);
	setMotorSpeedRight(speedRight);
}

void Drive::setMotorSpeedLeft(int &speed)
{
// 	_logger->Drive("Drive::setMotorSpeedLeft()");

	if(_emergencyStopEnabled)
	{
		_logger->Drive("Drive::setMotorSpeedLeft(): emergency stop enabled, returning.");
		speed = 0;
		return;
	}

	double speedMS = speed / 1000000.0;

// 	_logger->Drive("Drive::setMotorSpeedLeft(): setting speed to %.4F m/s.", speedMS);

	// convert the given speed from m/s to rad/s
	speedMS = meter_2_radiant(speedMS, sideLeft);

	int iSpeedEncoderIncrementPeriod = radPerSecond_2_encoderIncrementPeriod(-speedMS / _multiplier);

	if(iSpeedEncoderIncrementPeriod > 32767) iSpeedEncoderIncrementPeriod = -32767;

	if(iSpeedEncoderIncrementPeriod < -32768) iSpeedEncoderIncrementPeriod = -32768;

	speedMS = -encoderIncrementPeriod_2_radPerSecond(iSpeedEncoderIncrementPeriod) * _multiplier;
	speed = (int)(radiant_2_meter(speedMS, sideLeft) * 1000000.0);

	unsigned int hi = iSpeedEncoderIncrementPeriod >> 8;
	unsigned int lo = iSpeedEncoderIncrementPeriod & 0xff;

	can->lock();
	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_SETCMDVAL, 0, 0, 0, 0, hi, lo));

	// It seems that we'll get two answers for this:
	// - CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETSTATUS
	// - CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETPOSVEL
	// It also seems like we don't care about them ATM, so we just absorb them
	CanMessage message1(CAN_ID_LEFT_REPLY, CMD_MOTCTRL_GETSTATUS);
	can->receiveTimeout(&message1);

	CanMessage message2(CAN_ID_LEFT_REPLY, CMD_MOTCTRL_GETPOSVEL);
	can->receiveTimeout(&message2);
	can->unlock();
}

void Drive::setMotorSpeedRight(int &speed)
{
// 	_logger->Drive("Drive::setMotorSpeedRight()");

	if(_emergencyStopEnabled)
	{
		_logger->Drive("Drive::setMotorSpeedRight(): emergency stop enabled, returning.");
		speed = 0;
		return;
	}

	double speedMS = speed / 1000000.0;

// 	_logger->Drive("Drive::setMotorSpeedRight(): setting speed to %.4F m/s.", speedMS);

	// WARNING: it seems the right wheel's speed needs to be reversed
	speedMS *= -1.0;

	// convert the given speed from m/s to rad/s
	speedMS = meter_2_radiant(speedMS, sideRight);

	int iSpeedEncoderIncrementPeriod = radPerSecond_2_encoderIncrementPeriod(-speedMS / _multiplier);

	if(iSpeedEncoderIncrementPeriod > 32767) iSpeedEncoderIncrementPeriod = -32767;

	if(iSpeedEncoderIncrementPeriod < -32768) iSpeedEncoderIncrementPeriod = -32768;

	speedMS = -encoderIncrementPeriod_2_radPerSecond(iSpeedEncoderIncrementPeriod) * _multiplier;

	// again, reversing speed
	speed = -(int)(radiant_2_meter(speedMS, sideRight) * 1000000.0);

	unsigned int hi = iSpeedEncoderIncrementPeriod >> 8;
	unsigned int lo = iSpeedEncoderIncrementPeriod & 0xff;

	can->lock();
	can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_SETCMDVAL, 0, 0, 0, 0, hi, lo));

	// It seems that we'll get two answers for this:
	// - CAN_ID_RIGHT_REPLY,  CMD_MOTCTRL_GETSTATUS
	// - CAN_ID_RIGHT_REPLY,  CMD_MOTCTRL_GETPOSVEL
	// It also seems like we don't care about them ATM, so we just absorb them
	CanMessage message1(CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETSTATUS);
	can->receiveTimeout(&message1);

	CanMessage message2(CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETPOSVEL);
	can->receiveTimeout(&message2);
	can->unlock();
}

void Drive::initializeAngleLeft(void)
{
	_logger->Drive("Drive::initializeAngleLeft()");

	can->lock();
	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_GETPOSVEL));

	CanMessage message(CAN_ID_LEFT_REPLY, CMD_MOTCTRL_GETPOSVEL);
	can->receiveTimeout(&message);
	can->unlock();

	_iAngleLeft = -(
			(message.getData(0) << 24)
			|
			(message.getData(1) << 16)
			|
			(message.getData(2) << 8)
			|
			message.getData(3)
		       );

	_logger->Drive("Drive::initializeAngleLeft(): angle = %i (0x%08x)", _iAngleLeft, _iAngleLeft);
}

void Drive::initializeAngleRight(void)
{
	_logger->Drive("Drive::initializeAngleRight()");

	can->lock();
	can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_GETPOSVEL));

	CanMessage message(CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETPOSVEL);
	can->receiveTimeout(&message);
	can->unlock();

	_iAngleLeft = -(
			(message.getData(0) << 24)
			|
			(message.getData(1) << 16)
			|
			(message.getData(2) << 8)
			|
			message.getData(3)
		       );

	_logger->Drive("Drive::initializeAngleRight(): angle = %i (0x%08x)", _iAngleLeft, _iAngleLeft);
}

void Drive::getMotorSpeeds(int &speedLeft, int &speedRight)
{
	// speed in meters per second
	double speedMS;

	can->lock();

	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_GETPOSVEL));
	CanMessage message1(CAN_ID_LEFT_REPLY, CMD_MOTCTRL_GETPOSVEL);
	can->receiveTimeout(&message1);

	can->unlock();

	short iVelEncIncrPeriodLeft = -((message1.getData(4) << 8) | message1.getData(5));
	speedMS = encoderIncrementPeriod_2_radPerSecond(iVelEncIncrPeriodLeft) * _multiplier;
	speedLeft = (int)(radiant_2_meter(speedMS, sideLeft) * 1000000.0);

	can->lock();

	can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_GETPOSVEL));
	CanMessage message2(CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETPOSVEL);
	can->receiveTimeout(&message2);

	can->unlock();

	short iVelEncIncrPeriodRight = -((message2.getData(4) << 8) | message2.getData(5));
	speedMS = encoderIncrementPeriod_2_radPerSecond(iVelEncIncrPeriodRight) * _multiplier;
	speedRight = (int)(radiant_2_meter(speedMS, sideRight) * 1000000.0);

	_logger->Drive("Drive::getMotorSpeeds(): left %d micrometer/s, right %d micrometer/s", speedLeft, speedRight);
}

void Drive::getAngleDeltas(double &deltaAngleLeft, double &deltaAngleRight)
{
// 	_logger->Drive("Drive::getAngleDeltas()");

	// We time this operation. Take the start timestamp
	struct timeval start;
	gettimeofday(&start, 0x0);

	can->lock();

	CanMessage messagePosVelLeft(CAN_ID_LEFT_CMD, CMD_MOTCTRL_GETPOSVEL);
	can->send(messagePosVelLeft);
	// the sent message can be used for receiving, we just need to change the ID from CMD to REPLY
	messagePosVelLeft.setId(CAN_ID_LEFT_REPLY);
	can->receiveTimeout(&messagePosVelLeft);

	CanMessage messagePosVelRight(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_GETPOSVEL);
	can->send(messagePosVelRight);
	// the sent message can be used for receiving, we just need to change the ID from CMD to REPLY
	messagePosVelRight.setId(CAN_ID_RIGHT_REPLY);
	can->receiveTimeout(&messagePosVelRight);

	can->unlock();

	int iAngleLeft;

	iAngleLeft = (
		      	(messagePosVelLeft.getData(0) << 24)
			|
			(messagePosVelLeft.getData(1) << 16)
			|
			(messagePosVelLeft.getData(2) << 8)
			|
			messagePosVelLeft.getData(3)
		     );
	iAngleLeft = -iAngleLeft;

	// Shortly over 0x08000000 the encoder jumps to 0xf8000000, resulting in a very large delta in
	// one cycle. Try to detect the jump and modify the previous value so that the delta becomes
	// reasonable again.

	if((_iAngleLeft > 0x07000000) /*(positive)*/ && (iAngleLeft <= 0))
	{
		_iAngleLeft |= 0xf0000000;
		_logger->Drive("Drive::getAngleDeltas(): ************* JUMP CAUGHT (positive left) *************");
	}
	else if((_iAngleLeft < (int)0xf9000000) /*(negative)*/ && (iAngleLeft >= 0))
	{
		_iAngleLeft &= ~0xf0000000;
		_logger->Drive("Drive::getAngleDeltas(): ************* JUMP CAUGHT (negative left) *************");
	}

	int deltaLeft = iAngleLeft - _iAngleLeft;   // safe of wraps (must be `int')
	_iAngleLeft = iAngleLeft;
	deltaAngleLeft = encoderAngle_2_rad(deltaLeft);

	int iAngleRight;

	iAngleRight = (
		       (messagePosVelRight.getData(0) << 24)
			|
			(messagePosVelRight.getData(1) << 16)
			|
			(messagePosVelRight.getData(2) << 8)
			|
			messagePosVelRight.getData(3)
		      );

	if((_iAngleRight > 0x07000000) /*(positive)*/ && (iAngleRight <= 0))
	{
		_iAngleRight |= 0xf0000000;
		_logger->Drive("Drive::getAngleDeltas(): ************* JUMP CAUGHT (positive right) *************");
	}
	else if((_iAngleRight < (int)0xf9000000) /*(negative)*/ && (iAngleRight >= 0))
	{
		_iAngleRight &= ~0xf0000000;
		_logger->Drive("Drive::getAngleDeltas(): ************* JUMP CAUGHT (negative right) *************");
	}

	int deltaRight = iAngleRight - _iAngleRight;   // safe of wraps (must be `int')
	_iAngleRight = iAngleRight;
	deltaAngleRight = encoderAngle_2_rad(deltaRight);

	double degreesR = Conversion::rad2deg(deltaAngleRight);
	double degreesL = Conversion::rad2deg(deltaAngleLeft);

	debugAngleRight += degreesR;

	struct timeval stop;
	gettimeofday (&stop, 0x0);

	double milliseconds =
		(stop.tv_sec - start.tv_sec) * 1000.0 +
		(stop.tv_usec - start.tv_usec) / 1000.0;

	_logger->Drive("Drive::getAngleDeltas(): took %.3f millisecs, angleDeltas: l=%.4F  r=%.4F", milliseconds, degreesL, degreesR);
	_logger->Drive("Drive::getAngleDeltas(): right wheel rotated %.4F degrees in total", debugAngleRight);
	_logger->Drive("Drive::getAngleDeltas(): returning rad angle deltas %.4F / %.4F", deltaAngleLeft, deltaAngleRight);
}

void Drive::getMotorAdvances(int &advanceLeft, int &advanceRight)
{
// 	_logger->Drive("Drive::getMotorAdvances(): asking getAngleDeltas()...");
	double deltaAngleLeft, deltaAngleRight;
	getAngleDeltas(deltaAngleLeft, deltaAngleRight);

	advanceLeft  = (int)(config->getWheelCircumference(sideLeft ) * Conversion::rad2deg(deltaAngleLeft ) / 360.0 * 1000000);
	advanceRight = (int)(config->getWheelCircumference(sideRight) * Conversion::rad2deg(deltaAngleRight) / 360.0 * 1000000);

// 	advanceLeft  = (int)(radiant_2_meter(deltaAngleLeft, Side::left) * 1000000.0);
// 	advanceRight = (int)(radiant_2_meter(deltaAngleRight, Side::right) * 1000000.0);

	debugPathLengthRight += advanceRight / 1000000.0;

	_logger->Drive("Drive::getMotorAdvances(): we advanced %d / %d micrometers since the last call.", advanceLeft, advanceRight);
	_logger->Drive("Drive::getMotorAdvances(): right wheel moved %.4Fm in total.", debugPathLengthRight);
}

void Drive::getMotorTemperatures(float &left, float &right)
{
	_logger->Drive("Drive::getMotorTemperatures()");

	can->lock();

	// ask for the left temperature on the CAN
	can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_GETSTATUS));

	CanMessage messageLeft(CAN_ID_LEFT_REPLY,  CMD_MOTCTRL_GETSTATUS);
	can->receiveTimeout(&messageLeft);

	can->unlock();

	int tempLeft = (messageLeft.getData()[2] << 8) | messageLeft.getData()[3];

	// conversion of left temp
	left = ((-1.0 * tempLeft + 500) / 4.0 + 20.0);

	can->lock();

	// ask for the right temperature on the CAN
	can->send(CanMessage(CAN_ID_RIGHT_CMD,  CMD_MOTCTRL_GETSTATUS));

	CanMessage messageRight(CAN_ID_RIGHT_REPLY, CMD_MOTCTRL_GETSTATUS);
	can->receiveTimeout(&messageRight);

	can->unlock();

	int tempRight = (messageRight.getData()[2] << 8) | messageRight.getData()[3];

	// conversion of right temp
	right = ((-1.0 * tempRight + 500) / 4.0 + 20.0);

	_logger->Drive("Drive::getMotorTemperatures(): temps are %.2F, %.2F degrees celsius.", left, right);
}

bool Drive::setEmergencyStop(const bool enable)
{
	_logger->Drive("Drive::setEmergencyStop(enable: %d)", enable);

	CanMessage message;

	if (_emergencyStopEnabled == true && enable == false)
	{
		// DISABLE emergency stop
		can->lock();
		can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_RESETEMSTOP));
		can->receiveNextTimeout();

		can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_RESETEMSTOP));
		can->receiveNextTimeout();
		can->unlock();

		_emergencyStopEnabled = false;
	}

	if (_emergencyStopEnabled == false && enable == true)
	{
		// ENABLE emergency stop
		can->lock();
		can->send(CanMessage(CAN_ID_LEFT_CMD, CMD_MOTCTRL_SETEMSTOP));
		can->receiveNextTimeout();

		can->send(CanMessage(CAN_ID_RIGHT_CMD, CMD_MOTCTRL_SETEMSTOP));
		can->receiveNextTimeout();
		can->unlock();

		_emergencyStopEnabled = true;
	}

	return true;
}
