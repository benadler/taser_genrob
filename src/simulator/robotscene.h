#ifndef ROBOTSCENE_H
#define ROBOTSCENE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsPathItem>
#include <QGraphicsEllipseItem>
#include <QPainterPath>
#include <QPixmap>
#include <QPointF>
#include <QLineF>
#include <QTimer>
#include <QFile>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QDebug>

#include "robotcontrols.h"
#include "configuration.h"
#include "conversion.h"
#include "robotitem.h"
#include "logger.h"

#include "../splinecubic.h"
#include "../splinecosinus.h"
#include "../splinehermite.h"

#include <math.h>

/// @class RobotScene
/// @brief This class exposes the functionality of the CAN-devices to a UDP socket.
/// This class is a very simple server. It receives UDP datagrams, reads their
/// commands, calls methods in the CanDevice-members and *can* send data back
/// to the sender

class RobotScene : public QGraphicsScene
{
	Q_OBJECT

	private:
		QTimer *timer;
		RobotItem* robotItem;
		QGraphicsPathItem *robotPathItem;
		QPainterPath *robotPath;
		QGraphicsRectItem *wheelL, *wheelR;
		QGraphicsLineItem *axis;
		RobotControls *robotControls;
		QGraphicsRectItem* laserMarksParent;
		QGraphicsRectItem* linesParent;
		QGraphicsRectItem* tracerParent;
		Configuration* config;

		SplineCubic* splineCubic;
		QPainterPath* splineCubicPath;
		QGraphicsPathItem* splineCubicPathItem;

		SplineHermite* splineHermite;
		QPainterPath* splineHermitePath;
		QGraphicsPathItem* splineHermitePathItem;

		int iterations;
		Pose pose;
		double scaleFactor;
		double robotAngle;

		void moveRobotToPose(Pose target);
		void resetRobotPath();
		QPointF scalePoint(const QPointF &point);

	private slots:
		void slotUpdateRobot(void);
		void slotClearTrace();
		void slotSetPose(double x, double y, double angle);
		void slotUpdateSplineCubic(bool, QList<QPointF> points);
		void slotUpdateSplineHermite(bool, QList<QPointF> points, float tension, float bias);

	signals:
		void newPose(double x, double y, double angle);

	public:
		RobotScene(RobotControls* robotControls);
		~RobotScene();

		void loadLaserMarks(QString filename);
		void loadLines(QString filename);
};

#endif
