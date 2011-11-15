#ifndef CURVETEST_H
#define CURVETEST_H

#include <QList>
#include <QFont>
#include <QColor>
#include <QPointF>
#include <QGraphicsLineItem>
#include <QPainter>
#include <QLabel>
#include <QTimer>
#include <QString>
#include <QPaintEvent>
#include <QRectF>
#include <QPainterPath>
#include <QTcpSocket>
#include <QWidget>
#include <QApplication>

#include "configuration.h"
#include "spline.h"
#include "packet.h"

class CurveTest : public QWidget
{
	Q_OBJECT

	private:
		Spline *spline;
		QTcpSocket* socket;
		QLabel* label;
		
		QTimer* timerUpdateMovingPoint;
		Configuration* config;
		
		quint16 packetLength;
	
		int movingPoint, curveLength;
		
		QList<QPoint> getWayPoints(QPoint start, QPoint end) const;
		void paintEvent(QPaintEvent *);
		
		bool sendWayPointRequest(QPoint start, QPoint end);
		QPointF scaleDownPoint(QPoint point, int factor = 1000000);

	private slots:
		void slotDrawMovingPoint(void);
		void slotReceiveWayPointsFromServer(void);

	public:
		CurveTest();
};

#endif
