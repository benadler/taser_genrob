#ifndef CANSERVER_H
#define CANSERVER_H

#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>

#include <signal.h>

#include "logger.h"
#include "packet.h"
#include "battery.h"
#include "drive.h"
#include "remotecontrol.h"
#include "configuration.h"
#include "protocol_can.h"

/// @class CanServer
/// @brief This class exposes the functionality of the CAN-devices to a UDP socket.
/// This class is a very simple server. It receives UDP datagrams, reads their
/// commands, calls methods in the CanDevice-members and *can* send data back
/// to the sender

class CanServer : public QCoreApplication
{
	Q_OBJECT

	private:
		Logger* logger;			///< the logger-class
		Configuration* config;
		QTcpServer* tcpServer;
		QTcpSocket* socket;

		Battery battery;		///< a battery attached to CAN
		RemoteControl remoteControl;	///< a remoteControl attached to CAN
		Drive drive;			///< the motors attached to CAN

		///
		/// This method takes a received packet, looks at the command and then acts on it.
		/// @param packet a pointer to the packet
		///
		void handlePacket(Packet* packet);

	private slots:
		void slotNewConnection(void);
		void slotSocketStateChanged(QAbstractSocket::SocketState);
		void slotDataReceived(void);

	public:
		CanServer(int argc, char** argv);
};

#endif
