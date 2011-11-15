#ifndef WAYPOINTSERVER_H
#define WAYPOINTSERVER_H

#include <QCoreApplication>
#include <QTcpSocket>
#include <QTcpServer>
#include <QPointF>
#include <QList>

#include "configuration.h"
#include "logger.h"
#include "packet.h"

#include <stdlib.h>	// needed for rand() and friends
#include <sys/time.h>	// needed for gettimeofday()

/// @class WayPointServer
/// @brief
/// This class is a very simple server. It receives UDP datagrams, reads their
/// commands, calls methods in the CanDevice-members and *can* send data back
/// to the sender

class WayPointServer : public QCoreApplication
{
	Q_OBJECT

	private:
		Logger* logger;		///< the logger-class
		Configuration* config;

		QTcpSocket* tcpSocket;
		QTcpServer* server;

		quint16 packetLength;	///< the length of the incoming packet.

		///
		/// This method takes a received packet, looks at the command and then acts on it.
		/// @param packet a pointer to the packet
		///
		bool handleRequest(QList<QPointF> startAndEnd);

		bool startTcpServer(void);

		QList<QPointF> getWayPoints(const QPointF start, const QPointF end) const;

	private slots:
		void slotIncomingPacket();
		void slotNewConnection();

	public:
		WayPointServer(int argc, char **argv);
		~WayPointServer(void);

};

#endif
