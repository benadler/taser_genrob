#ifndef BATTERY_H
#define BATTERY_H

#include <candevice.h>

/// @class Battery
/// @brief The Battery-class represents the battery, which is connected to the CAN
/// This class only offers one meaningful method, which is getVoltage().

class Battery : protected CanDevice
{
	public:
		Battery ();
		~Battery ();
		
		///
		/// This method asks the IOBoard on the CAN for the battery-voltage,
		/// converts the received number to a meaningful value (volts) and
		/// returns it as a float.
		/// @return Battery Voltage in volts
		///
		float getVoltage (void);
};

#endif
