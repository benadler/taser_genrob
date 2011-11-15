#include "canserver.h"

CanServer::CanServer(int argc, char** argv) : QCoreApplication(argc, argv)
{
	logger = Logger::instance();
	logger->CanServer("CanServer::CanServer()");
	config = Configuration::instance();

	tcpServer = new QTcpServer(this);
	if(!tcpServer->listen(QHostAddress::Any, config->getCanServerPort()))
		abort("CanServer::CanServer(): Could not open canServer on port %d, exiting.", config->getCanServerPort());

	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

	// startup motors first, even if the incoming commands don't require them. Otherwise, as soon
	// as we DO have a command that requires the motors, the server hangs during startMotors();
	drive.startMotors();

	// Instead of applying brakes (releasing them takes forever), we set Emergency Stop, which also
	// applies the brakes, but can be undone much faster.
	drive.setEmergencyStop(true);

	logger->CanServer("CanServer::CanServer(): brakes applied, ready and waiting.");
}

void CanServer::slotNewConnection(void)
{
	logger->CanServer("CanServer::slotNewConnection(): new client connected.");
	socket = tcpServer->nextPendingConnection();
	connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotSocketStateChanged(QAbstractSocket::SocketState)));
	connect(socket, SIGNAL(readyRead()), this, SLOT(slotDataReceived()));

	// close the server, one client is enough
	tcpServer->close();
}

void CanServer::slotSocketStateChanged(QAbstractSocket::SocketState state)
{
	if(state == QTcpSocket::UnconnectedState)
	{
		// the client has disconnected. Reopen the server so we can get a new client
		logger->CanServer("CanServer::slotSocketStateChanged(): client disconnected. Applying brakes, reopening server.");
		drive.setEmergencyStop(true);
		tcpServer->listen(QHostAddress::Any, config->getCanServerPort());
	}
}

void CanServer::slotDataReceived(void)
{
	unsigned int bytesAvailable = socket->bytesAvailable();

	// Commands have at least 4 words (=16 byte)
	if(bytesAvailable < 16)
		return;

	// There might be multiple packets waiting in the buffer. Do NOT copy the whole
	// buffer into ONE packet. We have to look for the packetLength first, which is
	// located at bytes 4,5,6,7 (see packet.h documentation for header layout).
	QByteArray packetHeader = socket->read(8);
	unsigned int packetLength = *(unsigned int*)(packetHeader.constData() + 4);

	//logger->NetworkServer("CanServer::slotDataReceived(): the packet being received is %d bytes long.", packetLength);

	// we can now read the whole packet into a buffer.
	QByteArray packetComplete(packetHeader);
	packetComplete.append(socket->read(packetLength - 8));

	Packet packet;
	packet.setData((const unsigned char*)packetComplete.data(), packetComplete.size());

	if(packet.isValid())
		handlePacket(&packet);
	else
		logger->CanServer("CanServer::slotDataReceived(): received an invalid packet.");

	// If there is more data waiting (for another packet), then make sure its fetched ASAP.
	if(socket->bytesAvailable() > 0)
	{
		logger->CanServer("CanServer::slotDataReceived(): There's %d more bytes waiting, re-scheduling myself.", socket->bytesAvailable());
		QTimer::singleShot(0, this, SLOT(slotDataReceived()));
	}
}

void CanServer::handlePacket(Packet* request)
{
	logger->CanServer("CanServer::handlePacket(): received a packet with command 0x%03x", request->getCommand());
	// see what the packet contains, then act.
	switch(request->getCommand())
	{
		case (CAN_REQUEST | CAN_BATTERYVOLTAGE):
		{
			float voltage = battery.getVoltage();
			logger->CanServer("CanServer::handlePacket(): request 101 / getVoltage: voltage is %2.2fV", voltage);

			Packet response(CAN_REPLY | CAN_BATTERYVOLTAGE);
			response.pushF32(voltage);
			response.send(socket);
		}
		break;
		case (CAN_REQUEST | CAN_SET_WHEELSPEEDS):
		{
			int speedL, speedR = 0;
			speedL= request->popS32();
			speedR = request->popS32();
			logger->CanServer("CanServer::handlePacket(): request 102 / setting speed left/right to %d / %d micrometer/s.", speedL, speedR);

			// We shouldn't get packets requesting a wheelspeed higher than this...
			// Not really true, see scpeedCorrectionFacot in Conversion-class.
			int maximumWheelSpeeds = config->getMaximumWheelSpeed() * 1000000 * 1.1;
			
			Q_ASSERT(abs(speedL) < maximumWheelSpeeds);
			Q_ASSERT(abs(speedR) < maximumWheelSpeeds);

			static bool driveInitialized = false;

			if(!driveInitialized)
			{
				// we start the motors in run() already.
				//drive.startMotors();
				usleep(20000);
				drive.setEmergencyStop(false);
				usleep(20000);
				driveInitialized = true;
			}

			drive.setMotorSpeeds(speedL, speedR);
		}
		break;
		case (CAN_REQUEST | CAN_REMOTECONTROL):
                {
			int speed, steering = 0;
			remoteControl.getPositions(steering, speed);
			logger->CanServer("CanServer::handlePacket(): returning speed %d, steering, %d.", speed, steering);

	                Packet response(CAN_REPLY | CAN_REMOTECONTROL);
			response.pushS32(steering);
			response.pushS32(speed);
	                response.send(socket);
                }
                break;
		case (CAN_REQUEST | CAN_MOTORTEMPS):
                {
			float left, right;
			drive.getMotorTemperatures(left, right);
			logger->CanServer("CanServer::handlePacket(): returning temperatures %.2F / %.2F.", left, right);

	                Packet response(CAN_REPLY | CAN_MOTORTEMPS);
			response.pushF32(left);
			response.pushF32(right);
	                response.send(socket);
                }
                break;
		case (CAN_REQUEST | CAN_WHEELADVANCES):
                {
			int advanceLeft, advanceRight;
			drive.getMotorAdvances(advanceLeft, advanceRight);
			logger->CanServer("CanServer::handlePacket(): returning motor advances %d and %d micrometers.", advanceLeft, advanceRight);

	                Packet response(CAN_REPLY | CAN_WHEELADVANCES);
			response.pushS32(advanceLeft);
			response.pushS32(advanceRight);
	                response.send(socket);
                }
                break;
		case (CAN_REQUEST | CAN_BRAKES_ENABLE):
		{
			logger->CanServer("CanServer::handlePacket(): enabling brakes.");

			Packet response(CAN_REPLY | CAN_BRAKES_ENABLE);

			if(drive.setEmergencyStop(true))
				response.pushS32(0);
			else
				response.pushS32(-1);

			response.send(socket);
		}
		break;
		case (CAN_REQUEST | CAN_BRAKES_DISABLE):
		{
			logger->CanServer("CanServer::handlePacket(): disabling brakes.");

			Packet response(CAN_REPLY | CAN_BRAKES_DISABLE);

			if(drive.setEmergencyStop(false))
				response.pushS32(0);
			else
				response.pushS32(-1);

			response.send(socket);
		}
		break;

		case (CAN_REQUEST | CAN_EMERGENCY_STOP_ENABLE):
		{
			logger->CanServer("CanServer::handlePacket(): enabling emergency stop.");

			drive.setEmergencyStop(true);

			Packet response(CAN_REPLY | CAN_EMERGENCY_STOP_ENABLE);
			response.pushS32(0);
			response.send(socket);
		}
		break;

		case (CAN_REQUEST | CAN_EMERGENCY_STOP_DISABLE):
		{
			logger->CanServer("CanServer::handlePacket(): disabling emergency stop.");

			drive.setEmergencyStop(false);

			Packet response(CAN_REPLY | CAN_EMERGENCY_STOP_DISABLE);
			response.pushS32(0);
			response.send(socket);
		}
		break;

		default: logger->CanServer("CanServer::handlePacket(): unknown packet command id: %d", request->getCommand());
	}
}

// We create the canserver here and not in main, so that we can delete it in catch_int()
// This means that Drive::~Drive() will be called, which applies the brakes.
CanServer* server;

// first, here is the signal handler
void catch_int(int sig_num)
{
	// re-set the signal handler again to catch_int, for next time
	signal(SIGINT, catch_int);
	printf("CanServer::main(): caught SIGINT (CTRL-C), shutting down now.\n");
	delete server;
	exit(0);
}

int main(int argc, char* argv[])
{
	// set the INT (Ctrl-C) signal handler to 'server.shutdown()'
	signal(SIGINT, catch_int);

	Logger::setupLogging(argc, argv);
	server = new CanServer(argc, argv);
	return server->exec();
}

