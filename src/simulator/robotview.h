#ifndef ROBOTVIEW_H
#define ROBOTVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QGLWidget>
#include <math.h>

/// @class RobotView
/// @brief This class exposes the functionality of the CAN-devices to a UDP socket.
/// This class is a very simple server. It receives UDP datagrams, reads their
/// commands, calls methods in the CanDevice-members and *can* send data back
/// to the sender

class RobotView : public QGraphicsView
{
	Q_OBJECT

	private:

	public:
		RobotView();
		~RobotView();

		void wheelEvent(QWheelEvent *event);
		void scaleView(double scaleFactor);
};

#endif
