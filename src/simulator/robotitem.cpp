#include "robotitem.h"

RobotItem::RobotItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent)
{
	// make the robot transparent
	setBrush(QBrush(QColor(0, 0, 0, 0)));
	QVector<QPointF> polyList;
	polyList << QPointF(-height/3.5, width/3);
	polyList << QPointF(height/3, 0.0);
	polyList << QPointF(-height/3.5, -width/3);
	arrow = new QGraphicsPolygonItem(QPolygonF(polyList), this);
	arrow->setBrush(QBrush(QColor(0, 255, 0, 128)));
	setRect(x, y, width, height);
	setParentItem(parent);
	setFlags(QGraphicsItem::ItemIsMovable);
	setAcceptsHoverEvents(true);

	setPen(QPen(Qt::gray));
}

void RobotItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	QGraphicsItem::mouseReleaseEvent(event);

	// tell robotscene about the new position!
	emit newPose(pos().x(), pos().y(), -1000.0);
}

void RobotItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	QGraphicsItem::mouseMoveEvent(event);

	// tell robotscene about the new position IF the LMB was pressed
	if(event->buttons() & Qt::LeftButton)
		emit newPose(pos().x(), pos().y(), -1000.0);
}

void RobotItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	QGraphicsItem::hoverEnterEvent(event);

	//change bgcolor
	setBrush(QBrush(QColor(255, 0, 0, 128)));
}

void RobotItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	QGraphicsItem::hoverLeaveEvent(event);

	//change bgcolor
	setBrush(QBrush(Qt::white));
}
