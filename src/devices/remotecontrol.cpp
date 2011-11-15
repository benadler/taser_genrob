#include "remotecontrol.h"

RemoteControl::RemoteControl()
{
	logger->RemoteControl("RemoteControl::RemoteControl()");
}


RemoteControl::~RemoteControl()
{
	logger->RemoteControl("RemoteControl::~RemoteControl()");
}

void RemoteControl::getPositions(signed int &steering, signed int &speed)
{
	can->lock();

	can->send(CanMessage(CAN_ID_IO_CMD, CMD_IOBOARD_GETJOYVAL));

	// Warning: the original source in canplatform.cc didn't specify ID or CMD for reply packet!
	CanMessage message(CAN_ID_IO_REPLY, CMD_IOBOARD_GETJOYVAL);
	can->receiveTimeout(&message);

	can->unlock();
	
	signed char charSteering = message.getData(0);
	signed char charSpeed = message.getData(1);
	
	steering = (int)charSteering;
	speed = (int)charSpeed * -1;

	// avoid creeping due to r/c calibration errors.
	if(abs(speed) < 4) speed = 0;
	if(abs(steering) < 4) steering = 0;
	
	// speed to the power-of-three for better handling.
	speed = speed * speed * speed;
	//steering = steering * steering * steering;

	logger->RemoteControl("RemoteControl::getPositions(): steering %d, speed %d", steering, speed);
}
