#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QGraphicsView>
#include <QMainWindow>
#include <QApplication>
#include <QDockWidget>
#include <QTextEdit>

#include <stdio.h>

#include <sys/time.h>

/// @class LogWidget
/// @brief This class exposes the functionality of the CAN-devices to a UDP socket.
/// This class is a very simple server. It receives UDP datagrams, reads their
/// commands, calls methods in the CanDevice-members and *can* send data back
/// to the sender

class LogWidget : public QDockWidget
{
	Q_OBJECT

	private:
		QTextEdit textEdit;

	public:
		LogWidget();
		~LogWidget();
		void appendLine(QString &line);
		void UdpServer(const char *fmt, ...);
};

#endif
