#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <candevice.h>

/// Messages from the remotecontrol
/// signed char x = message.getData(0); x is the steering, -35 is left, 35 is right
/// signed char y = message.getData(1); y is the speed, -35 is full speed (pulling the trigger), 35 is right.

class RemoteControl : protected CanDevice
{
	public:
		RemoteControl();
		~RemoteControl();
		
		// speed is -46656 to 46656 cubic, steering is -36 to 36.
		void getPositions(signed int &steering, signed int &speed);
};

#endif
