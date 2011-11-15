#include "logger.h"

// Initialize the singleton-pointer to NULL
QMutex Logger::singletonMutex;
Logger* Logger::singleton = NULL;

// Initialize static member-variables.
bool Logger::logBeeperThread = false;
bool Logger::logCan = false;
bool Logger::logCanDevice = false;
bool Logger::logCollisionAvoidance = false;
bool Logger::logConversion = false;
bool Logger::logCorrespondence = false;
bool Logger::logBattery = false;
bool Logger::logDrive = false;
bool Logger::logJava = false;
bool Logger::logLaserMark = false;
bool Logger::logLaserScan = false;
bool Logger::logLaserScanner = false;
bool Logger::logLocalization = false;
bool Logger::logMobileD = false;
bool Logger::logMobileDClient = false;
bool Logger::logMotion = false;
bool Logger::logNetworkServer = false;
bool Logger::logPacket = false;
bool Logger::logRobot = false;
bool Logger::logSignalThread = false;
bool Logger::logSpline = false;
bool Logger::logCanServer = false;
bool Logger::logSimulator = false;
bool Logger::logUdpClient = false;
bool Logger::logRemoteControl = false;
bool Logger::logWayPointServer = false;

Logger* Logger::instance()
{
	QMutexLocker locker(&singletonMutex);

	if(singleton == NULL)
		singleton = new Logger();

	return singleton;
}

Logger::Logger(void)
{
	mutex = new QMutex(QMutex::NonRecursive);
	log("Logger::Logger(): starting up.");
}

Logger::~Logger(void)
{
	log("Logger::~Logger()");
}

void Logger::setupLogging(int argc, char* argv[])
{
	char logAreas[1024] = {0};
	bool checkForLogging = false;

	for(int i=1;i<argc;i++)
	{
		//printf("now parsing argument: %s.\n", argv[i]);
		// is this a log parameter?
		if(strcasecmp(argv[i], "-d") == 0)
		{
			strncpy(logAreas, argv[i+1], 1024);
			checkForLogging = true;
			break;
		}
	}

	if(checkForLogging)
	{
		char* currentLogArea = strtok(logAreas, ",");

		char outputstring[512] = {0};
		strcat(outputstring, "enabled logareas: ");

		while(currentLogArea != NULL)
		{
			if(strcasecmp("BeeperThread", currentLogArea) == 0)
			{
				logBeeperThread = true;
				strcat(outputstring, "BeeperThread ");
			}
			if(strcasecmp("Can", currentLogArea) == 0)
			{
				logCan = true;
				strcat(outputstring, "Can ");
			}
			if(strcasecmp("CanDevice", currentLogArea) == 0)
			{
				logCanDevice = true;
				strcat(outputstring, "CanDevice ");
			}
			if(strcasecmp("CollisionAvoidance", currentLogArea) == 0)
			{
				logCollisionAvoidance = true;
				strcat(outputstring, "CollisionAvoidance ");
			}
			if(strcasecmp("Conversion", currentLogArea) == 0)
			{
				logConversion = true;
				strcat(outputstring, "Conversion ");
			}
			if(strcasecmp("Correspondence", currentLogArea) == 0)
			{
				logCorrespondence = true;
				strcat(outputstring, "Correspondence ");
			}
			if(strcasecmp("Battery", currentLogArea) == 0)
			{
				logBattery = true;
				strcat(outputstring, "Battery ");
			}
			if(strcasecmp("Drive", currentLogArea) == 0)
			{
				logDrive = true;
				strcat(outputstring, "Drive ");
			}
			if(strcasecmp("Java", currentLogArea) == 0)
			{
				logJava = true;
				strcat(outputstring, "Java ");
			}
			if(strcasecmp("LaserMark", currentLogArea) == 0)
			{
				logLaserMark = true;
				strcat(outputstring, "LaserMark ");
			}
			if(strcasecmp("LaserScan", currentLogArea) == 0)
			{
				logLaserScan = true;
				strcat(outputstring, "LaserScan ");
			}

			if(strcasecmp("LaserScanner", currentLogArea) == 0)
			{
				logLaserScanner = true;
				strcat(outputstring, "LaserScanner ");
			}
			if(strcasecmp("Localization", currentLogArea) == 0)
			{
				logLocalization = true;
				strcat(outputstring, "Localization ");
			}
			if(strcasecmp("MobileD", currentLogArea) == 0)
			{
				logMobileD = true;
				strcat(outputstring, "MobileD ");
			}
			if(strcasecmp("MobileDClient", currentLogArea) == 0)
			{
				logMobileDClient = true;
				strcat(outputstring, "MobileDClient ");
			}
			if(strcasecmp("Motion", currentLogArea) == 0)
			{
				logMotion = true;
				strcat(outputstring, "Motion ");
			}
			if(strcasecmp("NetworkServer", currentLogArea) == 0)
			{
				logLocalization = true;
				strcat(outputstring, "NetworkServer ");
			}
			if(strcasecmp("Packet", currentLogArea) == 0)
			{
				logPacket = true;
				strcat(outputstring, "Packet ");
			}
			if(strcasecmp("Robot", currentLogArea) == 0)
			{
				logRobot = true;
				strcat(outputstring, "Robot ");
			}
			if(strcasecmp("SignalThread", currentLogArea) == 0)
			{
				logSignalThread = true;
				strcat(outputstring, "SignalThread ");
			}
			if(strcasecmp("Spline", currentLogArea) == 0)
			{
				logSpline = true;
				strcat(outputstring, "Spline ");
			}
			if(strcasecmp("UdpClient", currentLogArea) == 0)
			{
				logUdpClient = true;
				strcat(outputstring, "UdpClient ");
			}
			if(strcasecmp("CanServer", currentLogArea) == 0)
			{
				logCanServer = true;
				strcat(outputstring, "CanServer ");
			}
			if(strcasecmp("Simulator", currentLogArea) == 0)
			{
				logSimulator = true;
				strcat(outputstring, "Simulator ");
			}
			if(strcasecmp("RemoteControl", currentLogArea) == 0)
			{
				logRemoteControl = true;
				strcat(outputstring, "RemoteControl ");
			}
			if(strcasecmp("WayPointServer", currentLogArea) == 0)
			{
				logWayPointServer = true;
				strcat(outputstring, "WayPointServer ");
			}

			if(strcasecmp("All", currentLogArea) == 0)
			{
				logBeeperThread = true;
				logCan = true;
				logCanDevice = true;
				logCollisionAvoidance = true;
				logConversion = true;
				logCorrespondence = true;
				logBattery = true;
				logDrive = true;
				logJava = true;
				logLaserMark = true;
				logLaserScan = true;
				logLaserScanner = true;
				logLocalization = true;
				logMobileD = true;
				logMobileDClient = true;
				logMotion = true;
				logNetworkServer = true;
				logPacket = true;
				logRobot = true;
				logSignalThread = true;
				logSpline = true;
				logCanServer = true;
				logSimulator = true;
				logUdpClient = true;
				logRemoteControl = true;
				logWayPointServer = true;

				strcat(outputstring, "All ");
			}

			currentLogArea = strtok(NULL, ",");
		}

		log("Logger::setupLogging(): %s", outputstring);
	}
	else
		log("Logger::setupLogging(): logging disabled.");
}

void Logger::log(const char *fmt, ...)
{
	char buf[256], *ptr = buf;
	int size = 0, remaining = sizeof (buf) - 7;   // strlen(" (...)")+1

	struct timeval now;
	gettimeofday(&now, 0x0);
	struct tm *tm = localtime(&now.tv_sec);

	size = sprintf (buf, "%02i:%02i:%02i.%06i ",
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec,
		(int)now.tv_usec);

	ptr += size;
	remaining -= size;

	// now the variable part
	va_list args;

	va_start (args, fmt);
	int inc = vsnprintf (ptr, remaining, fmt, args);
	va_end (args);

	if(inc > remaining)   // `man vsnprintf'
	{
		inc = remaining;
	}

	ptr += inc;
	size += inc;
	remaining -= inc;

	if(remaining < 1)
	{
		size += sprintf (ptr, " (...)");
	}

	write(fileno (stderr), buf, size);
	printf("\n");
}

void Logger::print(const char *fmt, va_list args)
{
	char buf[256], *ptr = buf;
	int size = 0, remaining = sizeof (buf) - 7;   // strlen(" (...)")+1

	struct timeval now;
	gettimeofday(&now, 0x0);
	struct tm *tm = localtime(&now.tv_sec);

	size = sprintf (buf, "%02i:%02i:%02i.%06i ",
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec,
		(int)now.tv_usec);

	ptr += size;
	remaining -= size;


	int inc = vsnprintf(ptr, remaining, fmt, args);

	if(inc > remaining)
	{
		inc = remaining;
	}

	ptr += inc;
	size += inc;
	remaining -= inc;

	if(remaining < 1)
	{
		size += sprintf (ptr, " (...)");
	}

	write(fileno (stderr), buf, size);
	printf("\n");
}

// Not a member of the Logger-class!
void abort(const char *fmt, ...)
{
	char buf[256], *ptr = buf;
	int size = 0, remaining = sizeof (buf) - 7;   // strlen(" (...)")+1

	struct timeval now;
	gettimeofday (&now, 0x0);
	struct tm *tm = localtime (&now.tv_sec);


	size = sprintf (buf, "%02i:%02i:%02i.%06i ",
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec,
		(int)now.tv_usec);

	ptr += size;
	remaining -= size;

	// now the variable part
	va_list args;

	va_start (args, fmt);
	int inc = vsnprintf (ptr, remaining, fmt, args);
	va_end (args);

	if (inc > remaining)   // `man vsnprintf'
	{
		inc = remaining;
	}

	ptr += inc;
	size += inc;
	remaining -= inc;

	if(remaining < 1)
	{
		size += sprintf (ptr, " (...)");
	}

	write (fileno (stderr), buf, size);
	printf("\n");

	exit(1);
}
