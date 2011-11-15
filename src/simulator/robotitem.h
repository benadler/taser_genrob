#ifndef ROBOTITEM_H
#define ROBOTITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QObject>
#include <QPointF>
#include <QDebug>

#include "robotcontrols.h"
#include "configuration.h"
#include "conversion.h"
#include "logger.h"

#include <math.h>

/// @class RobotItem
/// @brief foo

class RobotItem : public QObject, public QGraphicsRectItem
{
	Q_OBJECT

	private:
		QGraphicsPolygonItem *arrow;
		void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
		void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
		void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
		void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

	public:
		RobotItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0);

	signals:
		void newPose(double x, double y, double angle);
};

#endif
