#include "simulator.h"

Simulator::Simulator() : QMainWindow()
{
	setWindowTitle("Simulator");

	logger = Logger::instance();

	robotView = new RobotView();
	robotView->fitInView(0, 0, 10, 10);
	setCentralWidget(robotView);

	logWidget = new LogWidget();
// 	addDockWidget(Qt::BottomDockWidgetArea, logWidget);

	robotControls = new RobotControls();
	addDockWidget(Qt::RightDockWidgetArea, robotControls);

	robotScene = new RobotScene(robotControls);
	robotView->setScene(robotScene);

	config = Configuration::instance();

	tcpServer = new QTcpServer(this);
	if(!tcpServer->listen(QHostAddress::Any, config->getCanServerPort()))
		abort("Simulator::Simulator(): Could not open canServer on port %d, exiting.", config->getCanServerPort());

	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void Simulator::slotNewConnection(void)
{
	logger->Simulator("Simulator::slotNewConnection(): new client connected.");
	socket = tcpServer->nextPendingConnection();
	connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotSocketStateChanged(QAbstractSocket::SocketState)));
	connect(socket, SIGNAL(readyRead()), this, SLOT(slotDataReceived()));

	// close the server, one client is enough
	tcpServer->close();
}

void Simulator::slotSocketStateChanged(QAbstractSocket::SocketState state)
{
	if(state == QTcpSocket::UnconnectedState)
	{
		// the client has disconnected. Reopen the server so we can get a new client
		logger->Simulator("Simulator::slotSocketStateChanged(): client disconnected. Applying brakes, reopening server.");
		robotControls->setBrakes(true);
		robotControls->setMotorSpeeds(0, 0);
		tcpServer->listen(QHostAddress::Any, config->getCanServerPort());
	}
}


void Simulator::slotDataReceived(void)
{
	unsigned int bytesAvailable = socket->bytesAvailable();

	//logger->Simulator("Simulator::slotDataReceived(): %d bytes available.", bytesAvailable);

	// Commands have at least 4 words (=16 byte)
	if(bytesAvailable < 16)
		return;

	// There might be multiple packets waiting in the buffer. Do NOT copy the whole
	// buffer into ONE packet. We have to look for the packetLength first, which is
	// located at bytes 4,5,6,7 (see packet.h documentation for header layout).
	QByteArray packetHeader = socket->read(8);
	unsigned int packetLength = *(unsigned int*)(packetHeader.constData() + 4);

	//logger->Simulator("Simulator::slotDataReceived(): the packet being received is %d bytes long.", packetLength);

	// we can now read the whole packet into a buffer.
	QByteArray packetComplete(packetHeader);
	packetComplete.append(socket->read(packetLength - 8));

	Packet packet;
	packet.setData((const unsigned char*)packetComplete.data(), packetComplete.size());

	if(packet.isValid())
		handlePacket(&packet);
	else
		logger->Simulator("Simulator::slotDataReceived(): received an invalid packet.");

	// If there is more data waiting (for another packet), then make sure its fetched ASAP.
	if(socket->bytesAvailable() > 0)
	{
		logger->Simulator("Simulator::slotDataReceived(): There's %d more bytes waiting, re-scheduling myself.", socket->bytesAvailable());
		QTimer::singleShot(0, this, SLOT(slotDataReceived()));
	}
}

void Simulator::handlePacket(Packet* request)
{
	// see what the packet contains, then act.
	switch(request->getCommand())
	{
		case (CAN_REQUEST | CAN_BATTERYVOLTAGE):
		{
			float voltage = robotControls->getBatteryVoltage();
			logger->Simulator("Simulator::handlePacket(): getVoltage: voltage is %.2F volts", voltage);

			Packet response(CAN_REPLY | CAN_BATTERYVOLTAGE);
			response.pushF32(voltage);
			response.send(socket);
		}
		break;
		case (CAN_REQUEST | CAN_SET_WHEELSPEEDS):
		{
			int speedLeft = request->popS32();
			int speedRight = request->popS32();
			logger->Simulator("Simulator::handlePacket(): setting speed left/right to %d / %d um/s.", speedLeft, speedRight);
			robotControls->setMotorSpeeds(speedLeft, speedRight);
		}
		break;
		case (CAN_REQUEST | CAN_REMOTECONTROL):
                {
			int speed, steering = 0;
			robotControls->getRemoteControl(steering, speed);
			logger->Simulator("Simulator::handlePacket(): returning speed %d, steering, %d.", speed, steering);
	                Packet response(CAN_REPLY | CAN_REMOTECONTROL);
			response.pushS32(steering);
			response.pushS32(speed);
	                response.send(socket);
                }
                break;
		case (CAN_REQUEST | CAN_MOTORTEMPS):
                {
			float left, right;
			robotControls->getMotorTemperatures(left, right);
			logger->Simulator("Simulator::handlePacket(): returning temperatures %.2FC / %.2FC.", left, right);
	                Packet response(CAN_REPLY | CAN_MOTORTEMPS);
			response.pushF32(left);
			response.pushF32(right);
	                response.send(socket);
                }
                break;
		case (CAN_REQUEST | CAN_WHEELADVANCES):
                {
			int advanceLeft, advanceRight;
			robotControls->getMotorAdvances(1, advanceLeft, advanceRight);
			logger->Simulator("Simulator::handlePacket(): returning motor advances %d and %d.", advanceLeft, advanceRight);
	                Packet response(CAN_REPLY | CAN_WHEELADVANCES);
			response.pushS32(advanceLeft);
			response.pushS32(advanceRight);
	                response.send(socket);
                }
                break;
		case (CAN_REQUEST | CAN_EMERGENCY_STOP_ENABLE):
		{
			logger->Simulator("Simulator::handlePacket(): enabling brakes via EMSTOP.");
			robotControls->setBrakes(true);
		}
		break;
		case (CAN_REQUEST | CAN_EMERGENCY_STOP_DISABLE):
		{
			logger->Simulator("Simulator::handlePacket(): disabling brakes via EMSTOP.");
			robotControls->setBrakes(false);

		}
		break;


		default: logger->Simulator("Simulator::handlePacket(): unknown packet command id: 0x%08x", request->getCommand());
	}
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	Logger::setupLogging(argc, argv);

	Simulator server;
	server.show();

	return app.exec();
}
