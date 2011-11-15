#include "waypointserver.h"

WayPointServer::WayPointServer(int argc, char **argv) : QCoreApplication(argc, argv)
{
	logger = Logger::instance();
	config = Configuration::instance();

	server = new QTcpServer(this);
	connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

	startTcpServer();
}

WayPointServer::~WayPointServer()
{
	server->close();
}

bool WayPointServer::startTcpServer(void)
{
	if(server->listen(QHostAddress::Any, 2007))
	{
		logger->WayPointServer("WayPointServer::startTcpServer(): successfully opened TCP server on port %d.", config->getWayPointServerPort());
		return true;
	}
	else
	{
		logger->WayPointServer("WayPointServer::startTcpServer(): failed to open TCP server on port %d.", config->getWayPointServerPort());
		return false;
	}
}

void WayPointServer::slotNewConnection()
{
	tcpSocket = server->nextPendingConnection();

	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(slotIncomingPacket()));
	//connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnected()));

	server->close();
}

void WayPointServer::slotIncomingPacket()
{
	logger->WayPointServer("WayPointServer::slotIncomingPacket()");

	QDataStream in(tcpSocket);
	in.setVersion(QDataStream::Qt_4_0);

	QString numberOfBytesAvailable = QString::number(tcpSocket->bytesAvailable());

        // Set the blocksize of the incoming packet
        if(packetLength == 0)
        {
		//printf("bytes of incoming packet available: %d.\n", (int)tcpSocket->bytesAvailable());

		if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
                        return;

                in >> packetLength;
		//printf("wrote packetLength: %d.\n", packetLength);
        }

        if(tcpSocket->bytesAvailable() < packetLength)
	{
		printf("packetLength is %d, but only %s bytes available. returning.\n", packetLength, qPrintable(numberOfBytesAvailable));
                return;
	}

	// reset packetLength for the next incoming packet
	packetLength = 0;

        QList<QPointF> startAndEnd;
        in >> startAndEnd;

	handleRequest(startAndEnd);

	tcpSocket->close();

	startTcpServer();
}

bool WayPointServer::handleRequest(QList<QPointF> startAndEnd)
{
	// request 106: get route from point A to point B
	// answer 1006: contains (int)numberOfWayPoints, waypoints (double x, double y)
	logger->WayPointServer("WayPointServer::handleReqeust(): getWayPoints.");

	QPointF start = startAndEnd.takeFirst();
	QPointF end = startAndEnd.takeLast();

	QList<QPointF> wayPoints = getWayPoints(start, end);


        if(tcpSocket->state() == QAbstractSocket::ConnectedState)
	{
		logger->WayPointServer("WayPointServer::handlePacket(): sending packet with %d waypoints to %s:%d.", wayPoints.size(), qPrintable(QHostAddress(tcpSocket->peerAddress()).toString()), tcpSocket->peerPort());

		QByteArray block;
		QDataStream out(&block, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_4_0);

		// fill the byteArray with the dataSize mark
		out << (quint16)0;

		// now feed the data
		out << wayPoints;

		// set the datasize
		out.device()->seek(0);
		out << (quint16)(block.size() - sizeof(quint16));

		// and send it
		tcpSocket->write(block);
		tcpSocket->flush();

		return true;
	}

	return false;
}

QList<QPointF> WayPointServer::getWayPoints(const QPointF start, const QPointF end) const
{
	logger->WayPointServer("WayPointServer::getWayPoints(): creating waypoints for route from %.2f/%.2f to %.2f/%.2f", start.x(), start.y(), end.x(), end.y());

	// We get start and end, deliver some points in between
	double distanceX = end.x() - start.x();
	double distanceY = end.y() - start.y();

	// see rand() 3 manpage
	struct timeval now;
	gettimeofday (&now, 0x0);
	srand(now.tv_usec);
	int numberOfPointsInBetween = 1 + (int)(10.0*rand()/(RAND_MAX+1.0));

	QList<QPointF> wayPoints;

	wayPoints.append(start);

	for(int i=1; i < (numberOfPointsInBetween+1); i++)
	{
		double x = start.x() + (i * (distanceX/numberOfPointsInBetween));
		double xNoise = 2.0 * ((distanceX/numberOfPointsInBetween/2) - ((distanceX/numberOfPointsInBetween)*rand()/(RAND_MAX+1.0)));

		double y = start.y() + (i * (distanceX/numberOfPointsInBetween));
		double yNoise = 2.0 * ((distanceY/numberOfPointsInBetween/2) - ((distanceY/numberOfPointsInBetween)*rand()/(RAND_MAX+1.0)));

		logger->WayPointServer("WayPointServer::getWayPoints(): adding intermediate point %.2f/%.2f.", x + xNoise, y + yNoise);

		wayPoints.append(QPointF(x + xNoise, y + yNoise));
	}

	wayPoints.append(end);

	logger->WayPointServer("WayPointServer::getWayPoints(): returning a list with %d waypoints (%d points in between).", wayPoints.size(), numberOfPointsInBetween);

	return wayPoints;
}

int main(int argc, char **argv)
{
	Logger::setupLogging(argc, argv);

	WayPointServer server(argc, argv);

	return server.exec();
}
