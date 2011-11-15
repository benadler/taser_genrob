#include "candevice.h"

CanDevice::CanDevice()
{
	logger = Logger::instance();
	logger->CanDevice("CanDevice::CanDevice()");

	can = Can::instance();
}


CanDevice::~CanDevice()
{
	can = NULL;
}


