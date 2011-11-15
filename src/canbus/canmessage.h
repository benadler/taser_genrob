#ifndef CANMESSAGE_H
#define CANMESSAGE_H

#include <logger.h>
#include <cmd_ioboard.h>

/// @class CanMessage
/// @brief The CanMessage class represents a single message sent over the CAN bus.
/// This class is used to send and receive messages via CAN. Each message has an ID, a
/// command (CMD), error and data fields. The position of these fields seems to differ
/// between sent and received packets:
///
/// This class has no getCommand() method, because the message would then have to know
/// whether is was just created for being sent lateron, or whether it was just received
/// (because the Command-value is in different positions for sent & received packets).
/// We might add a private setThisMessageWasReceived()-method that would be called from
/// the CAN-class after this message is received. It would then know where to look for
/// the CMD-field.
///
/// Have a look at the "Can Interface Platform Ctrl"-document from neobotix, dated october 2003.
///
/// What we receive from CAN is:
/// - data (8 bytes)
///   - byte 0: data
///   - byte 1: data
///   - byte 2: data
///   - byte 3: data
///   - byte 4: data
///   - byte 5: data
///   - byte 6: The senders' CAN ID
///   - byte 7
///     - 6 bits: command that was sent from PC to CANDevice that caused this reply packet.
///     - 2 bits: error (both are set to 1 on error)
///
/// What we send to CAN is:
/// - an ID
/// - data (8 bytes)
///   - byte 0: command / cmd
///   - byte 1: data
///   - byte 2: data
///   - byte 3: data
///   - byte 4: data
///   - byte 5: data
///   - byte 6: data
///   - byte 7: data
/// It seems that when sending data via CAN, we only use bytes 6 and 7
/// (e.g. when setting motor speed), bytes 1-5 are unused.

class CanMessage
{
	private:
		unsigned short _id;	///< the CAN-message ID
		unsigned char _data[8];	///< the eight data bytes of the CAN-message

	public:
		CanMessage(void);
		CanMessage(
			unsigned short id,
			const unsigned char data1,
			const unsigned char data2 = 0,
			const unsigned char data3 = 0,
			const unsigned char data4 = 0,
			const unsigned char data5 = 0,
			const unsigned char data6 = 0,
			const unsigned char data7 = 0,
			const unsigned char data8 = 0);
		
		~CanMessage (void);
		
		int getId(void) const;
		void setId(int id);

		unsigned char* getData (void);
		unsigned char getData(int bytenumber);
		
		void setData(
			const unsigned char data0,
			const unsigned char data1 = 0,
			const unsigned char data2 = 0,
			const unsigned char data3 = 0,
			const unsigned char data4 = 0,
			const unsigned char data5 = 0,
			const unsigned char data6 = 0,
			const unsigned char data7 = 0);
		
		///
		/// This method returns the error of this CAN packet. This error-information originates
		/// in the CAN itself, so such messages should probably be discarded.
		/// @return 0 for no error, other values for other errors (pretty much undefined)
		int getError(void) const;
		
		///
		/// This method resets both the ID and all data bytes to zero.
		///
		void reset(void);
};

#endif
