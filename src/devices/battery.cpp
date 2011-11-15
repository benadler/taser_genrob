#include "battery.h"

Battery::Battery()
{
	logger->Battery("Battery::Battery()");
}

Battery::~Battery()
{
	logger->Battery("Battery::~Battery()");
}

float Battery::getVoltage (void)
{
	can->lock();

	can->send(CanMessage(CAN_ID_IO_CMD, CMD_IOBOARD_GETVBATT));

	CanMessage message(CAN_ID_IO_REPLY, CMD_IOBOARD_GETVBATT);
	can->receiveTimeout(&message);

	can->unlock();

	int voltageRaw = (message.getData(0) << 8) | (message.getData(1));

	// this conversion was found in canplatform.cc and simply copied.
	double voltage = (-0.00000010537 * voltageRaw * voltageRaw) + (0.012578 * voltageRaw) - 314;

	logger->Battery("Battery::getVoltage(): voltageRaw is %d, real voltage is %2.2f V", voltageRaw, voltage);

	return voltage;
}
