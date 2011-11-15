#include "can.h"

// Initialize the singleton-pointer to NULL
QMutex Can::singletonMutex;
Can* Can::singleton = NULL;

Can* Can::instance()
{
	QMutexLocker locker(&singletonMutex);

	if(singleton == NULL)
		singleton = new Can();

	return singleton;
}

Can::Can (void)
{
	logger = Logger::instance();
	logger->Can("Can::Can()");

	mutex = new QMutex(QMutex::NonRecursive);

	// Try to open all /dev/pcan* files until one of them works...
	QStringList nameFilter;
	nameFilter << "*pcan*";

	QDir deviceDirectory("/dev/");
	QStringList pcanDeviceFiles = deviceDirectory.entryList(nameFilter, QDir::System);
	if(pcanDeviceFiles.size() == 0)
		abort("Can::Can(): couldn't find any /dev/pcan* files, cannot acces CAN.");

	bool canInitialized = false;

	foreach(QString deviceFileName, pcanDeviceFiles)
	{
		deviceFileName.prepend("/dev/");
		if((_fileDescriptor = open (deviceFileName.toUtf8(), O_RDWR | O_SYNC)) >= 0)
		{
			logger->Can("Can::Can(): successfully opened device file %s.", qPrintable(deviceFileName));
			// initialize the can bus.
			TPCANInit init;

			init.wBTR0BTR1 = CAN_BAUD_500K;		// merged BTR0 and BTR1 register of the SJA100
			init.ucCANMsgType = MSGTYPE_STANDARD;	// 11 or 29 bits
			init.ucListenOnly = 0;			// listen only mode when != 0

			if((ioctl(_fileDescriptor, PCAN_INIT, &init)) < 0)
			{
				logger->Can("Can::Can(): initializing CAN on %s failed.", qPrintable(deviceFileName));
			}
			else
			{
				logger->Can("Can::Can(): initialized CAN successfully.");
				canInitialized = true;
				break;
			}
		}
		else
		{
			logger->Can("Can::Can(): failed to open device file %s.", qPrintable(deviceFileName));
		}
	}

	if(!canInitialized)
		abort("Can::Can(): failed to initialize CAN.");
}

Can::~Can (void)
{
	logger->Can("Can::~Can()");
	close(_fileDescriptor);
}

void Can::lock(void)
{
	mutex->lock();
}

void Can::unlock(void)
{
	mutex->unlock();
}

static void error(int count)
{
	FILE *fp = fopen("canbus.err", "a");
	if(fp)
	{
		time_t t = time(0);
		struct tm tm;
		localtime_r(&t, &tm);

		fprintf(fp, "%04i%02i%02i%02i%02i : ",
			tm.tm_year + 1900,
			tm.tm_mon + 1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min);

		fprintf (fp, "%i\n", count);
		fclose (fp);
	}
}

bool Can::send(CanMessage message)
{
	// Most of the time we send a message with only
	// - ID and
	// - CMD in the first byte

	if(mutex->tryLock())
	{
		logger->Can("Can::send(): I'm supposed to send although the mutex isn't locked!");
		mutex->unlock();
	}

	const TPCANMsg msg =
		{
			message.getId(),
			MSGTYPE_STANDARD,
			8,
			{
				message.getData(0),
				message.getData(1),
				message.getData(2),
				message.getData(3),
				message.getData(4),
				message.getData(5),
				message.getData(6),
				message.getData(7)
			}
		};

	logger->Can("Can::send(): %s, %s, 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
		qPrintable(canIdToString(message.getId())),
		qPrintable(canCommandToString(message.getId(), message.getData(0))),
		msg.DATA[0],
		msg.DATA[1],
		msg.DATA[2],
		msg.DATA[3],
		msg.DATA[4],
		msg.DATA[5],
		msg.DATA[6],
		msg.DATA[7]);

	int count = 1;

	while(ioctl (_fileDescriptor, PCAN_WRITE_MSG, &msg) < 0)
	{
		logger->Can("Can::Send: ioctl(PCAN_WRITE_MSG) failed, retrying...");
		count++;
	}

	if(count != 1)
	{
		error(count);
		return false;
	}

	return true;
}

/* unused
void Can::receiveNow(CanMessage &message)
{
	logger->Can("Can::receiveNow()");

	TPCANRdMsg msg;

	if(ioctl(_fileDescriptor, PCAN_READ_MSG, &msg) < 0)
	{
		abort("Can::Receive(): ioctl(PCAN_READ_MSG) failed");
	}

	unsigned char data[1024];
	memcpy(data, msg.Msg.DATA, 8);

	unsigned int ident = data[6];
	unsigned int error = data[7] & 0x03;
	unsigned int cmd = data[7] >> 2;

	message.setId(msg.Msg.ID);
	message.setData(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

	logger->Can("Can::receiveNow(): got ID 0x%04x, length %i : 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
		msg.Msg.ID,
		msg.Msg.LEN,
		data[0],
		data[1],
		data[2],
		data[3],
		data[4],
		data[5],
		data[6],
		data[7]);

	logger->Can("Can::receiveNow(): ident = 0x%02x, cmd = 0x%02x, error = 0x%02x",
		ident,
		cmd,
		error);
}*/

bool Can::receiveNextTimeout(
		CanMessage* message,
		const unsigned int timeout,
		const bool omitWarnings)
{
// 	logger->Can("Can::receiveNextTimeout(): awaiting 1 packet within the next %i milliseconds.", timeout);

	if(mutex->tryLock())
	{
		logger->Can("Can::receiveNextTimeout(): I'm supposed to receive although the mutex isn't locked!");
		mutex->unlock();
	}

	fd_set rfd;
	FD_ZERO(&rfd);
	FD_SET(_fileDescriptor, &rfd);

	struct timeval timeoutStruct = {timeout / 1000, (timeout % 1000) * 1000};

	int ret = select(_fileDescriptor + 1, &rfd, 0x0, 0x0, &timeoutStruct);

	if(ret < 1)
	{
		if(!omitWarnings)
		{
			//logger->Can("Can::receiveNextTimeout(): timeout");
		}

		// wipe message with zeroes before returning due to timeout.
		if(message)
			message->reset();
		return false;
	}

	if(!FD_ISSET(_fileDescriptor, &rfd))
	{
		abort("Can::ReceiveTimeout(): select() weirdness");
	}

	TPCANRdMsg msg;

	if(ioctl(_fileDescriptor, PCAN_READ_MSG, &msg) < 0)
	{
		abort("Can::receiveNextTimeout(): ioctl(PCAN_READ_MSG) failed");
	}

	unsigned char data[1024];
	memcpy(data, msg.Msg.DATA, 8);

	unsigned int ident = data[6];
	unsigned int error = data[7] & 0x03;
	unsigned int cmd = data[7] >> 2;

	// only write data into the message if we were passed one.
	if(message)
	{
		message->setId(msg.Msg.ID);
		message->setData(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
	}

// 	logger->Can("Can::receiveNextTimeout(): id %s, length %i, ident 0x%02x, cmd %s, err 0x%02x : 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",

	logger->Can("Can::receiveNextTimeout(): %s, %s, err 0x%02x : 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
		qPrintable(canIdToString(msg.Msg.ID)),
		qPrintable(canCommandToString(msg.Msg.ID, cmd)),
		error,
		data[0],
		data[1],
		data[2],
		data[3],
		data[4],
		data[5],
		data[6],
		data[7]);

	return true;
}

QString Can::canCommandToString(int id, int command)
{
	QString commandString;

	if(id == 0x200 || id == 0x300 || id == 0x201 || id == 0x301)
	{
		// Motor IDs -> Motor Commands
		switch(command)
		{
			// These are IDs defined in cmd_motctrl.h
			case CMD_MOTCTRL_CONNECT:
			{
				commandString = QString("CMD_MOTCTRL_CONNECT");
			}
			break;
			case CMD_MOTCTRL_DISCONNECT:
			{
				commandString = QString("CMD_MOTCTRL_DISCONNECT");
			}
			break;
			case CMD_MOTCTRL_SETDIGOUT:
			{
				commandString = QString("CMD_MOTCTRL_SETDIGOUT");
			}
			break;
			case CMD_MOTCTRL_GETDIGIN:
			{
				commandString = QString("CMD_MOTCTRL_GETDIGIN");
			}
			break;
			case CMD_MOTCTRL_GETANALOGIN:
			{
				commandString = QString("CMD_MOTCTRL_GETANALOGIN");
			}
			break;
			case CMD_MOTCTRL_SETMOTIONTYPE:
			{
				commandString = QString("CMD_MOTCTRL_SETMOTIONTYPE");
			}
			break;
			case CMD_MOTCTRL_GETPOSVEL:
			{
				commandString = QString("CMD_MOTCTRL_GETPOSVEL");
			}
			break;
			case CMD_MOTCTRL_GETSTATUS:
			{
				commandString = QString("CMD_MOTCTRL_GETSTATUS");
			}
			break;
			case CMD_MOTCTRL_DISABLEBRAKE:
			{
				commandString = QString("CMD_MOTCTRL_DISABLEBRAKE");
			}
			break;
			case CMD_MOTCTRL_ENABLEMOTOR:
			{
				commandString = QString("CMD_MOTCTRL_ENABLEMOTOR");
			}
			break;
			case CMD_MOTCTRL_SYNCHMOTOR:
			{
				commandString = QString("CMD_MOTCTRL_SYNCHMOTOR");
			}
			break;
			case CMD_MOTCTRL_ENABLECOMM:
			{
				commandString = QString("CMD_MOTCTRL_ENABLECOMM");
			}
			break;
			case CMD_MOTCTRL_SETCMDVAL:
			{
				commandString = QString("CMD_MOTCTRL_SETCMDVAL");
			}
			break;
			case CMD_MOTCTRL_SETCTRLPARA:
			{
				commandString = QString("CMD_MOTCTRL_SETCTRLPARA");
			}
			break;
			case CMD_MOTCTRL_GETCTRLPARA:
			{
				commandString = QString("CMD_MOTCTRL_GETCTRLPARA");
			}
			break;
			case CMD_MOTCTRL_SETPOSCTRL:
			{
				commandString = QString("CMD_MOTCTRL_SETPOSCTRL");
			}
			break;
			case CMD_MOTCTRL_SETEMSTOP:
			{
				commandString = QString("CMD_MOTCTRL_SETEMSTOP");
			}
			break;
			case CMD_MOTCTRL_RESETEMSTOP:
			{
				commandString = QString("CMD_MOTCTRL_RESETEMSTOP");
			}
			break;
			case CMD_MOTCTRL_ERROR_STOPMOTION:
			{
				commandString = QString("CMD_MOTCTRL_ERROR_STOPMOTION");
			}
			break;
			case CMD_MOTCTRL_UNKNOWN:
			{
				commandString = QString("CMD_MOTCTRL_UNKNOWN");
			}
			break;
		}
	}
	else
	{
		// Not a motor, must be IOBOARD
		switch(command)
		{

			// These are requests & replies from the IOBoard (IOWarrior).
			// Values DEFINEd in cmd_IOBOARD.h
			case CMD_IOBOARD_CONNECT:
			{
				commandString = QString("CMD_IOBOARD_CONNECT");
			}
			break;
			case CMD_IOBOARD_DISCONNECT:
			{
				commandString = QString("CMD_IOBOARD_DISCONNECT");
			}
			break;
			case CMD_IOBOARD_GETDIGIN:
			{
				commandString = QString("CMD_IOBOARD_GETDIGIN");
			}
			break;
			case CMD_IOBOARD_SETDIGOUT:
			{
				commandString = QString("CMD_IOBOARD_SETDIGOUT");
			}
			break;
			case CMD_IOBOARD_GETANALOGIN:
			{
				commandString = QString("CMD_IOBOARD_GETANALOGIN");
			}
			break;
			case CMD_IOBOARD_GETGYROVAL:
			{
				commandString = QString("CMD_IOBOARD_GETGYROVAL");
			}
			break;
			case CMD_IOBOARD_ZEROGYRO:
			{
				commandString = QString("CMD_IOBOARD_ZEROGYRO");
			}
			break;
			case CMD_IOBOARD_GETJOYVAL:
			{
				commandString = QString("CMD_IOBOARD_GETJOYVAL");
			}
			break;
			case CMD_IOBOARD_GETVBATT:
			{
				commandString = QString("CMD_IOBOARD_GETVBATT");
			}
			break;
			case CMD_IOBOARD_GETSTATUS:
			{
				commandString = QString("CMD_IOBOARD_GETSTATUS");
			}
			break;
			case CMD_IOBOARD_SETCTRLPARA:
			{
				commandString = QString("CMD_IOBOARD_SETCTRLPARA");
			}
			break;
			case CMD_IOBOARD_GETCTRLPARA:
			{
				commandString = QString("CMD_IOBOARD_GETCTRLPARA");
			}
			break;
			case CMD_IOBOARD_UNKNOWN:
			{
				commandString = QString("CMD_IOBOARD_UNKNOWN");
			}
			break;

			default:
				commandString = QString("CMD_UNKNOWN");
		}
	}

	QString commandHex = QString::number(id, 16).prepend("0x");

// 	logger->Can("CAN-ID %s/%s.", qPrintable(commandHex), qPrintable(commandString));

// 	commandString.prepend("/");
// 	commandString.prepend(idHex);

	return commandString;
}

QString Can::canIdToString(int id)
{
	QString idString;
	switch(id)
	{
		case 0x200:
		{
			idString = QString("ID_FROM_MOTOR_RIGHT");
		}
		break;
		case 0x201:
		{
			idString = QString("ID_TO_MOTOR_RIGHT");
		}
		break;
		case 0x300:
		{
			idString = QString("ID_FROM_MOTOR_LEFT");
		}
		break;
		case 0x301:
		{
			idString = QString("ID_TO_MOTOR_LEFT");
		}
		break;
		// These are requests & replies from the IOBoard (IOWarrior).
		// Values DEFINEd in cmd_IOBOARD.h
		case CAN_ID_IO_CMD:
		{
			idString = QString("ID_TO_IOBOARD");
		}
		break;
		case CAN_ID_IO_REPLY:
		{
			idString = QString("ID_FROM_IOBOARD");
		}
		break;

		default:
			idString = QString("ID_UNKNOWN");
	}

	QString idHex = QString::number(id, 16).prepend("0x");

// 	logger->Can("CAN-ID %s/%s.", qPrintable(idHex), qPrintable(idString));

// 	idString.prepend("/");
// 	idString.prepend(idHex);

	return idString;
}

bool Can::receiveTimeout(CanMessage *message, const unsigned int timeout)
{
// 	logger->Can("Can::receiveTimeout(): awaiting any packet within %i milliseconds", timeout);

	if(timeout > 999) logger->Can("Can::receiveTimeout(): timeout may not be > 999ms, expect problems!");

	if(mutex->tryLock())
	{
		logger->Can("Can::receiveTimeout(): I'm supposed to receive although the mutex isn't locked!");
		mutex->unlock();
	}

	// The given message contains the ID and CMD that we're expecting to receive from CAN.
	// Looking at can.h, you'll notice that for comparing the CMD, we'll read it from the
	// FIRST data byte of the given message, but CAN delivers it in the FIRST 6 bits of
	// the LAST byte. Whew.
	signed int idExpected = message->getId();
	unsigned char cmdExpected = message->getData(0);

	struct timeval start;
	gettimeofday(&start, 0x0);

	while(true)
	{
		struct timeval now;
		gettimeofday(&now, 0x0);

		int elapsed = now.tv_usec - start.tv_usec;

		if(elapsed < 0) elapsed += 1000000;
		int subTimeout = timeout - elapsed / 1000;
		if(subTimeout < 1) subTimeout = 1;

// 		unsigned char buf[8];
		CanMessage receivedMessage;

		if(! receiveNextTimeout(&receivedMessage, subTimeout))
		{
			// No packet was receive within the timeout.
			logger->Can("Can::receiveTimeout(): no packet received within timeout. Returning false.");
			message->reset();
			return false;
		}

// 		int error = buf[7] & 0x03;
		signed int idReceived = receivedMessage.getId();
		unsigned char cmdReceived = receivedMessage.getData(7) >> 2;

		if(idExpected == idReceived && cmdExpected == cmdReceived && !receivedMessage.getError())
		{
// 			logger->Can("Can::receiveTimeout(): command 0x%02x from 0x%04x with error 0x%02x received", cmdReceived, receivedMessage.getId(), receivedMessage.getError());

			// copy receivedMessage into message. No need to copy ID as it matches.
			message->setData(
				receivedMessage.getData(0),
				receivedMessage.getData(1),
				receivedMessage.getData(2),
				receivedMessage.getData(3),
				receivedMessage.getData(4),
				receivedMessage.getData(5),
				receivedMessage.getData(6),
				receivedMessage.getData(7)
			);

			return true;
		}

		// At this point, we DID receive a message, it just didn't match the requested ID and CMD.
		logger->Can("Can::receiveTimeout(): received an unexpected message with command %s, from %s, error 0x%02x.",
			qPrintable(canCommandToString(idReceived, cmdReceived)),
			qPrintable(canIdToString(idReceived)),
   			error
			);

		// Loop again and ask for the next message if we still have the time...
	}
}
