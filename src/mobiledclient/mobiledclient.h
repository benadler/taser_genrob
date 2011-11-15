#ifndef MOBILEDCLIENT_H
#define MOBILEDCLIENT_H

#include <QMainWindow>
#include <QApplication>
#include <QUdpSocket>
#include <QByteArray>
#include <QTimer>
#include <QHostAddress>
#include <QErrorMessage>

#include "../packet.h"
#include "../logger.h"
#include "../configuration.h"
#include "../conversion.h"
#include "../protocol_mobiled.h"
#include "ui_mobiledclient.h"

/// @class MobileDClient
/// @brief
///
///
///

// Yes, copying theis enum from robot.h is ugly, but I'd need to include Java/JDK stuff otherwise.
enum RobotStatus
{
	idle,		// 0 brakes on
	stalled,	// 1 brakes on
	movementSetup,  // 2 brakes on
	moving,		// 3 brakes off
	aborting	// 4 brakes off
};

class MobileDClient : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

	private:
		Logger *logger;
		QTcpSocket *socket;
		Configuration* config;
		QHostAddress address;
		QErrorMessage* errDialog;

		///
		/// This method takes a received packet, looks at the command and then acts on it.
		/// @param packet a pointer to the packet
		///
		void handlePacket(Packet* packet);

		QString robotStatus2Text(int status) const;

	private slots:
		void slotIncomingPacket();
		void slotSocketStateChanged(QAbstractSocket::SocketState);

		void slotPoseGet();
		void slotPoseSet();

		void slotCaEnable();
		void slotCaDisable();

		void slotMoveSpline();
		void slotMoveRotate();
		void slotMoveTranslate();
		void slotMoveStop();

		void slotScannerGet();
		void slotVoltageGet();
		void slotTempsGet();
		void slotConnectToHost(void);
		void slotDisconnectFromHost(void);

		void slotRobotStatusGet();

	public:
		MobileDClient();
		~MobileDClient();
};

#endif
