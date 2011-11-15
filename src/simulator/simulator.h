#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/param.h>          // for htons() and friends
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>          //inet_addr() and friends
#include <unistd.h>    /* standard unix functions, like getpid()         */
#include <signal.h>    /* signal name macros, and the signal() prototype */

#include <QGraphicsView>
#include <QMainWindow>
#include <QApplication>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QByteArray>
#include <QHostAddress>

#include "../packet.h"
#include "../logger.h"
#include "../configuration.h"
#include "../protocol_can.h"
#include "logwidget.h"
#include "robotcontrols.h"
#include "robotview.h"
#include "robotscene.h"

/// @class Simulator
/// @brief This class exposes the functionality of the CAN-devices to a TCP socket.
/// This class is a very simple server. It receives UDP datagrams, reads their
/// commands, calls methods in the CanDevice-members and *can* send data back
/// to the sender

class Simulator : public QMainWindow
{
	Q_OBJECT

	private:
		LogWidget* logWidget;			///< the logger-widget
		Logger *logger;
		Configuration* config;
		QTcpServer* tcpServer;
		QTcpSocket *socket;
		RobotControls* robotControls;
		RobotView *robotView;
		RobotScene *robotScene;

// 		QHostAddress senderIP;
// 		quint16 senderPort;

		///
		/// This method takes a received packet, looks at the command and then acts on it.
		/// @param packet a pointer to the packet
		///
		void handlePacket(Packet* packet);

	private slots:
		void slotNewConnection(void);
		void slotDataReceived();
		void slotSocketStateChanged(QAbstractSocket::SocketState);

	public:
		Simulator();
};

#endif
