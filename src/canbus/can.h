#ifndef CAN_H
#define CAN_H

#define CAN_BAUD_1M     0x0014  //   1 MBit/s
#define CAN_BAUD_500K   0x001C  // 500 kBit/s
#define CAN_BAUD_250K   0x011C  // 250 kBit/s
#define CAN_BAUD_125K   0x031C  // 125 kBit/s
#define CAN_BAUD_100K   0x432F  // 100 kBit/s
#define CAN_BAUD_50K    0x472F  //  50 kBit/s
#define CAN_BAUD_20K    0x532F  //  20 kBit/s
#define CAN_BAUD_10K    0x672F  //  10 kBit/s
#define CAN_BAUD_5K     0x7F7F  //   5 kBit/s

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <time.h>
#include <QString>	//used for human-readable debug output, adds a qt-dependency...
#include <QDir>

#include <pcan.h>
#include <logger.h>

#include "canmessage.h"
#include "cmd_motctrl.h"
#include "cmd_ioboard.h"

/// @class Can
/// @brief The Can-class represents the CAN bus, connected using a parallel-port adapter.
/// This class is a singleton, meaning  that only ONE instance of it can be created. Calling
/// Can::instance() returns a pointer to this single instance. This class is currently NOT
/// thread-safe, as there are no mutexes yet. Then again, we don't use threads yet...
/// The device-file /dev/pcan24 is required for this class to work.
///
/// Warning from tschere: Do not send two commands to ID_IO too quickly.

class Can
{
	private:
		static Can* singleton;	///< pointer to the only instance of Can (which is a singleton class).
		static QMutex singletonMutex;

		QMutex* mutex;		///< used for thread-synchronization in member methods.

		Logger* logger;
		int _fileDescriptor;	///< file descriptor for the /dev/pcan24 file.

		///
		/// The default constructor does not take any arguments and is private, since Can is a singleton.
		///
		Can(void);

		///
		/// The default destructor does not take any arguments and is private, since Can is a singleton.
		///
		~Can(void);

		///
		/// Pass this method a CanMessage-Object and it will write the next received packet into it.
		/// No checks are done.
		///
		// currently unused
		//void receiveNow(CanMessage &message);

		///
		/// This method converts an integer CAN-ID into a meaningful string like MOTOR_LEFT, IOBOARD etc.
		/// @param id the CAN-ID
		/// @return a string describing the CAN-ID in human-readable form.
		///
		QString canIdToString(int id);

		///
		/// This method converts an integer CAN-Command into a meaningful string, as found in cmd_motctrl.h
		/// @param id the CAN-Device-ID. We need this, as both IOBoard and Motors have enums with the same values.
		/// @param command the CAN-Command
		/// @return a string describing the CAN-Command in human-readable form.
		///
		QString canCommandToString(int id, int command);

	public:
		///
		/// This static method returns a pointer to the only Can-object in existence. If there is no
		/// Can-object yet, one will be created before returning a pointer to it.
		///
		static Can* instance(void);

		///
		/// This method locks the can bus. Every device (battery, drive,...) using the can needs to
		/// lock the bus before using it, so that no two devices can use the bus at the same time.
		/// Else, it might be possible that multiple classes send packets and then wait for replies
		/// in the wrong order.
		///
		void lock(void);

		///
		/// This method unlocks the can bus. See lock() for a verbose description of the idea behind it.
		///
		void unlock(void);

		///
		/// This method sends a CanMessage over the CAN bus.
		///
		bool send(CanMessage message);

		///
		/// This method will wait for the next received CAN-packet and write it into the given CanMessage.
		/// If this method times out, it will wipe the given CanMessage with zeroes before returning.
		/// This method does NOT check and compare IDs or CMDs, it simply takes the next received message from CAN!
		/// @param message the CanMessage that the packet should be written into.
		/// @param timeout the time to wait (in milliseconds) for a packet to arrive before giving up. Default is 200ms.
		/// @param omitWarnings whether to print a warning when there was no packet received within the timeout.
		/// @return true if a packet was received before timing out, else false.
		///
		bool receiveNextTimeout(
				CanMessage *message = NULL,
				const unsigned int timeout = 200,
				const bool omitWarnings = false);

		///
		/// This method waits up to timeout milliseconds to receive a CAN packet for us. It then checks the given
		/// CanMessage for its ID and CMD and compares these two values against what it has received from CAN.
		/// Only if these values match, it will write the packet into the given CanMessage. If they do not match
		/// it will receive another package and try it, as long as timeout has not run out.
		/// If a matching packet could not be received within the timeout, it reset()s the given CanMessage and
		/// returns false
		/// @param message the message containing the expected ID and CMD. Will be overwritten with the received CAN-message
		/// @param timeout the timeout in milliseconds
		/// @return true when a matching packet was received within time, else false.
		bool receiveTimeout(
				CanMessage *message,
				const unsigned int timeout = 200);


};

#endif
