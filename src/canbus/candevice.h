#ifndef CANDEVICE_H
#define CANDEVICE_H

#include <can.h>
#include <canmessage.h>

#include <cmd_ioboard.h>

/// @class CanDevice
/// @brief The CanDevice class is a base class for all devices connected to the CAN
/// All devices connected to the CAN (Battery, Drive, RemoteControl) inherit from this class.
/// When these inheriting classes are instantiated, they call CanDevice's constructor which
/// sets the can member variable to point to the CAN bus.
/// This class is not meant to be instantiated (=is meant to be abstract).

class CanDevice
{
	protected:
		Logger* logger;
		Can* can;	///< this is a pointer to the CAN, initialized in the constructor.

	public:
		CanDevice();
		~CanDevice();
};

#endif
