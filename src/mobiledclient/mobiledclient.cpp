#include "mobiledclient.h"

MobileDClient::MobileDClient() : QMainWindow()
{
	setupUi(this);
	setWindowTitle("MobileDClient");

	logger = Logger::instance();
	config = Configuration::instance();

	socket = new QTcpSocket(this);
	socket->connectToHost(QHostAddress::LocalHost, 1234);

	connect(socket, SIGNAL(readyRead()), this, SLOT(slotIncomingPacket()));
	connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotSocketStateChanged(QAbstractSocket::SocketState)));

	connect(connectionConnect, SIGNAL(clicked()), this, SLOT(slotConnectToHost()));
	connect(connectionDisconnect, SIGNAL(clicked()), this, SLOT(slotDisconnectFromHost()));

	connect(poseGet, SIGNAL(clicked()), this, SLOT(slotPoseGet()));
	connect(poseSet, SIGNAL(clicked()), this, SLOT(slotPoseSet()));

	connect(caEnable, SIGNAL(clicked()), this, SLOT(slotCaEnable()));
	connect(caDisable, SIGNAL(clicked()), this, SLOT(slotCaDisable()));

	connect(moveSpline, SIGNAL(clicked()), this, SLOT(slotMoveSpline()));
	connect(moveRotate, SIGNAL(clicked()), this, SLOT(slotMoveRotate()));
	connect(moveTranslate, SIGNAL(clicked()), this, SLOT(slotMoveTranslate()));
	connect(moveStop, SIGNAL(clicked()), this, SLOT(slotMoveStop()));

	connect(robotStatusGet, SIGNAL(clicked()), this, SLOT(slotRobotStatusGet()));

	connect(scannerGet, SIGNAL(clicked()), this, SLOT(slotScannerGet()));
	connect(voltageGet, SIGNAL(clicked()), this, SLOT(slotVoltageGet()));
	connect(tempsGet, SIGNAL(clicked()), this, SLOT(slotTempsGet()));

	errDialog = new QErrorMessage(this);
}

MobileDClient::~MobileDClient()
{
	logger->MobileDClient("MobileDClient::~MobileDClient(): shutting down now.");
}

void MobileDClient::slotSocketStateChanged(QAbstractSocket::SocketState)
{
	logger->MobileDClient("MobileDClient::slotSocketStateChanged(): state is now: %d.", socket->state());
	if(socket->state() == QAbstractSocket::ConnectedState)
	{
		// set the lineedit bgcolor
		QColor color("green");
		QPalette palette = connectAddress->palette();
		palette.setColor(QPalette::Shadow, color);
		palette.setColor(QPalette::WindowText, color);
		connectAddress->setPalette(palette);

		// ask for the number of scanners
		Packet packetNumScanners(CMD_GETNUMSCANNERS);
		packetNumScanners.send(socket);

// 		processEvents();

		// how many commands are supported my mobiled?
		Packet packetCheckProtocol(CMD_CHECKPROTOCOL);
		packetCheckProtocol.send(socket);
	}

	if(socket->state() == QAbstractSocket::UnconnectedState)
	{
		// set the lineedit bgcolor
		QColor color("red");
		QPalette palette = connectAddress->palette();
		palette.setColor(QPalette::Shadow, color);
		palette.setColor(QPalette::WindowText, color);
		connectAddress->setPalette(palette);

		// ask for the number of scanners
		Packet packet(CMD_GETNUMSCANNERS);
		packet.send(socket);
	}
}

void MobileDClient::slotPoseGet()
{
	logger->MobileDClient("MobileDClient::slotPoseGet()");
	Packet packet(CMD_GETPOSE);
	packet.send(socket);
}

void MobileDClient::slotPoseSet()
{
	logger->MobileDClient("MobileDClient::slotPoseSet()");
	Packet packet(CMD_SETPOSE);
	packet.pushF32(poseX->value());
	packet.pushF32(poseY->value());
	packet.pushF32(Conversion::deg2rad(poseAngle->value()));
	packet.send(socket);
}

void MobileDClient::slotCaDisable()
{
	logger->MobileDClient("MobileDClient::slotCaDisable()");
	Packet packet(CMD_SETCOLLISIONAVOIDANCE);

	// disable collision avoidance
	packet.pushS32((int)false);
	packet.send(socket);
}

void MobileDClient::slotCaEnable()
{
	logger->MobileDClient("MobileDClient::slotCaDisable()");
	Packet packet(CMD_SETCOLLISIONAVOIDANCE);

	// disable collision avoidance
	packet.pushS32((int)true);
	packet.send(socket);
}

void MobileDClient::slotMoveSpline()
{
	logger->MobileDClient("MobileDClient::slotMoveSpline()");
	Packet packet(CMD_MOVESPLINE);

	packet.pushF32(moveX->value());
	packet.pushF32(moveY->value());
	packet.pushF32(Conversion::deg2rad(moveAngle->value()));
	packet.send(socket);
}

void MobileDClient::slotMoveRotate()
{
	logger->MobileDClient("MobileDClient::slotMoveRotate()");
	Packet packet(CMD_MOVEROTATE);

	packet.pushF32(Conversion::deg2rad(moveAngleDiff->value()));
	packet.send(socket);
}

void MobileDClient::slotMoveTranslate()
{
	logger->MobileDClient("MobileDClient::slotMoveTranslate()");
	Packet packet(CMD_MOVETRANSLATE);

	packet.pushF32(moveDistance->value());
	packet.send(socket);
}

void MobileDClient::slotMoveStop()
{
	logger->MobileDClient("MobileDClient::slotPoseMoveToStop()");
	Packet packet(CMD_STOP);
	packet.send(socket);
}

void MobileDClient::slotRobotStatusGet()
{
	logger->MobileDClient("MobileDClient::slotRobotStatusGet()");
	Packet packet(CMD_GETROBOTSTATUS);
	packet.send(socket);
}

void MobileDClient::slotScannerGet()
{
	logger->MobileDClient("MobileDClient::slotScannerGet()");
	Packet packet(CMD_GETSCANRADIALSCANNER);
	packet.pushU32((unsigned int)scannerNumber->value());
	packet.send(socket);
}

void MobileDClient::slotVoltageGet()
{
	logger->MobileDClient("MobileDClient::slotVoltageGet()");
	Packet packet(CMD_GETBATTERYVOLTAGE);
	packet.send(socket);
}

void MobileDClient::slotTempsGet()
{
	logger->MobileDClient("MobileDClient::slotTempsGet()");
	Packet packet(CMD_GETDRIVETEMPERATURES);
	packet.send(socket);
}

void MobileDClient::slotConnectToHost(void)
{
	logger->MobileDClient("MobileDClient::slotConnectToHost(): connecting to %s:%d...", qPrintable(connectAddress->text()), config->getMobileDServerPort());
	socket->connectToHost(connectAddress->text(), config->getMobileDServerPort());
}

void MobileDClient::slotDisconnectFromHost(void)
{
	logger->MobileDClient("MobileDClient::slotDisconnectFromHost(): disconnecting...");
	socket->disconnectFromHost();
}

void MobileDClient::slotIncomingPacket()
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

	// we can now read the whole packet into a buffer.
	QByteArray packetComplete(packetHeader);
	packetComplete.append(socket->read(packetLength - 8));

	Packet packet;
	packet.setData((const unsigned char*)packetComplete.data(), packetComplete.size());

	if(packet.isValid())
		handlePacket(&packet);
	else
		logger->MobileDClient("MobileDClient::slotIncomingPacket(): received an invalid packet.");

	// If there is more data waiting (for another packet), then make sure its fetched ASAP.
	if(socket->bytesAvailable() > 0)
	{
		logger->MobileDClient("MobileDClient::slotIncomingPacket(): There's %d more bytes waiting, re-scheduling myself.", socket->bytesAvailable());
		QTimer::singleShot(0, this, SLOT(slotIncomingPacket()));
	}
}

void MobileDClient::handlePacket(Packet* packet)
{
	logger->MobileDClient("MobileDClient::handlePacket(): processing packet with cmd 0x%08x.", packet->getCommand());

	// see what the packet contains, then act.
	switch(packet->getCommand())
	{
		case (CMD_REPLY | CMD_GETBATTERYVOLTAGE):
		{
			if(packet->popS32() == 0)
			{
				float voltage = packet->popF32();
				voltageLabel->setText(QString::number(voltage));
			}
			else
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");
		}
		break;
		case (CMD_REPLY | CMD_GETDRIVETEMPERATURES):
                {
	                if(packet->popS32() == 0)
	                {
		                float left = packet->popF32();
		                float right = packet->popF32();
		                tempsLabel->setText(QString::number(left).append(" / ").append(QString::number(right)));
	                }
	                else
		                errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");
                }
                break;
		case (CMD_REPLY | CMD_GETNUMSCANNERS):
		{
			if(packet->popS32() == 0)
			{
				unsigned int numScanners = packet->popU32();
				scannerNumber->setMaximum(numScanners - 1);
			}
			else
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");

		}
		break;

		case (CMD_REPLY | CMD_GETPOSE):
		{
			if(packet->popS32() == 0)
			{
				poseX->setValue(packet->popF32());
				poseY->setValue(packet->popF32());
				poseAngle->setValue(Conversion::rad2deg(packet->popF32()));
			}
			else
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");
		}
		break;

		// setposition also answers with the current pose
		case (CMD_REPLY | CMD_SETPOSE):
		{
			if(packet->popS32() == 0)
			{
				poseX->setValue(packet->popF32());
				poseY->setValue(packet->popF32());
				poseAngle->setValue(Conversion::rad2deg(packet->popF32()));
			}
			else
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");
		}
		break;

		case (CMD_REPLY | CMD_SETCOLLISIONAVOIDANCE):
		{
			if(packet->popS32() == 0)
			{
				bool enabled = (bool)packet->popS32();
				if(enabled)
					caStatus->setText("enabled");
				else
					caStatus->setText("disabled");
			}
			else
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");

		}
		break;

		case (CMD_REPLY | CMD_MOVESPLINE):
		{
			if(packet->popS32() != 0)
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");
		}
		break;

		case (CMD_REPLY | CMD_STOP):
		{
			if(packet->popS32() != 0)
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");
		}
		break;

		case (CMD_REPLY | CMD_GETSCANRADIALSCANNER):
		{
			scannerView->clear();
			errDialog->showMessage("MobileDClient::handlePacket(): got a scanner reply, lets see...");

			if(packet->popS32() == 0)
			{
				errDialog->showMessage("MobileDClient::handlePacket(): status ok");
				int numberOfScans = packet->popU32();
				logger->MobileDClient("MobileDClient::handlePacket(): contains %d scans.", numberOfScans);
				int foo = numberOfScans + 0;
				logger->MobileDClient("MobileDClient::handlePacket(): looping %d times.", foo);
				for(int i=0;i<foo;i++)
				{
					float distance = packet->popF32();
					logger->MobileDClient("MobileDClient::handlePacket(): scandistance: %.2F.", distance);
					QTableWidgetItem *itemNumber = new QTableWidgetItem(QString::number(i));
					QTableWidgetItem *itemDistance = new QTableWidgetItem(QString::number(distance));
					scannerView->setItem(i, 0, itemNumber);
					scannerView->setItem(i, 1, itemDistance);
				}
			}
			else
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");
		}
		break;

		case (CMD_REPLY | CMD_CHECKPROTOCOL):
		{
			if(packet->popS32() == 0)
			{
				protocolLabel->setText(QString("protocol version %1, supports %2 commands").arg(packet->peekU32(1)).arg(packet->getDataLength()/4));
			}
			else
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");
		}
		break;

		case (CMD_REPLY | CMD_GETROBOTSTATUS):
		{
			if(packet->popS32() == 0)
			{
				robotStatusLabel->setText(robotStatus2Text(packet->popU32()));
			}
			else
				errDialog->showMessage("MobileDClient::handlePacket(): received reply with status != 0");
		}
		break;

		default: logger->MobileDClient("MobileDClient::handlePacket(): unknown packet command id: %d", packet->getCommand());
	}
}

QString MobileDClient::robotStatus2Text(int status) const
{
	if(status == idle)
		return "idle";
	if(status == stalled)
		return "stalled";
	if(status == movementSetup)
		return "movementSetup";
	if(status == moving)
		return "moving";
	if(status == aborting)
		return "aborting";

	return "ERROR";
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	Logger::setupLogging(argc, argv);

	MobileDClient client;
	client.show();

	return app.exec();
}
