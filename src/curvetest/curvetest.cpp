#include "curvetest.h"

CurveTest::CurveTest() : QWidget()
{
	spline = new Spline(getWayPoints(QPoint(2000000, 2000000), QPoint(40000000, 40000000)));
	movingPoint = 1;
	curveLength = spline->getLength();

	packetLength = 0;

	socket = new QTcpSocket();
	connect(socket, SIGNAL(readyRead()), this, SLOT(slotReceiveWayPointsFromServer()));

// 	sendWayPointRequest(QPoint(0, 0), QPoint(100000000, 100000000));

	timerUpdateMovingPoint = new QTimer(this);
	connect(timerUpdateMovingPoint, SIGNAL(timeout()), this, SLOT(slotDrawMovingPoint()));
	timerUpdateMovingPoint->setInterval(20);
	timerUpdateMovingPoint->start();
}

void CurveTest::slotDrawMovingPoint(void)
{
	if(movingPoint >= spline->getLength())
	{
// 		sendWayPointRequest(QPoint(0, 0), QPoint(100000000, 100000000));
		movingPoint = 0;
	}

	movingPoint += 100000;
	update();
	QApplication::processEvents();
}

void CurveTest::slotReceiveWayPointsFromServer(void)
{
	QDataStream in(socket);
	in.setVersion(QDataStream::Qt_4_0);

	QString numberOfBytesAvailable = QString::number(socket->bytesAvailable());

        // Set the blocksize of the incoming packet
        if(packetLength == 0)
        {
		printf("bytes of incoming packet available: %d.\n", (int)socket->bytesAvailable());

                if (socket->bytesAvailable() < (int)sizeof(quint16))
                        return;

                in >> packetLength;
		printf("wrote packetLength: %d.\n", packetLength);
        }

        if(socket->bytesAvailable() < packetLength)
	{
		printf("packetLength is %d, but only %s bytes available. returning.\n", packetLength, qPrintable(numberOfBytesAvailable));
                return;
	}

	// reset packetLength for the next incoming packet
	packetLength = 0;

        QList<QPoint> wayPoints;
        in >> wayPoints;

	socket->close();

	printf("CurveTest::getWayPointsFromServer(): I got a total of %d waypoints from server.\n", wayPoints.size());

	for(int i=0;i<wayPoints.size();i++)
	{
		//printf("CurveTest::getWayPointsFromServer(): waypoint %.2f/%.2f is in list.\n", wayPoints.at(i).x(), wayPoints.at(i).y());
	}

	delete spline;
	spline = new Spline(wayPoints);
}

bool CurveTest::sendWayPointRequest(QPoint start, QPoint end)
{
// 	printf("CurveTest::getWayPointsFromServer(): getting waypoints from server...");
//
// 	if(socket->state() != QAbstractSocket::ConnectedState)
// 	{
// 		printf("not connected, reconnecting.\n");
// 		socket->connectToHost(
// 			QHostAddress(Configuration::getWayPointServerIP()),
// 			Configuration::getWayPointServerPort()
// 		);
//
// 		if(!socket->waitForConnected(1000))
// 		{
// 			printf("reconnection failed.\n");
// 			return false;
// 		}
// 	}
//
// 	QList<QPoint> startAndEnd;
//
// 	startAndEnd << start;
// 	startAndEnd << end;
//
// 	QByteArray block;
// 	QDataStream out(&block, QIODevice::WriteOnly);
// 	out.setVersion(QDataStream::Qt_4_0);
//
// 	// fill the byteArray with the dataSize mark
// 	out << (quint16)0;
//
// 	// now feed the data
// 	out << startAndEnd;
//
// 	// set the datasize
// 	out.device()->seek(0);
// 	out << (quint16)(block.size() - sizeof(quint16));
//
// 	// and send it
// 	socket->write(block);
// 	socket->flush();
//
// 	printf("done sending request.\n");

	return true;
}

QList<QPoint> CurveTest::getWayPoints(QPoint start, QPoint end) const
{
	QList<QPoint> wayPoints;

	Q_UNUSED(start);
	Q_UNUSED(end);

	wayPoints.append(QPoint(130000000, 130000000));
	wayPoints.append(QPoint(145340000, 102610000));
	wayPoints.append(QPoint(146220000, 97990000));
	wayPoints.append(QPoint(147000000, 95210000));
	wayPoints.append(QPoint(151340000, 85640000));
	wayPoints.append(QPoint(156700000, 83420000));
	wayPoints.append(QPoint(186890000, 84500000));
	wayPoints.append(QPoint(233570000, 83280000));
	wayPoints.append(QPoint(239080000, 85080000));
	wayPoints.append(QPoint(244500000, 95210000));
	wayPoints.append(QPoint(245330000, 97420000));
	wayPoints.append(QPoint(250000000, 150000000));

	return wayPoints;
}

void CurveTest::paintEvent(QPaintEvent *)
{
	//printf("drawing...\n");

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setFont(QFont("Arial", 3));

	double x = spline->getWayPoints().takeLast().x() / 1000000;
	double y = spline->getWayPoints().takeLast().y() / 1000000;

	//printf("last point: %.2f/%.2f.\n", x, y);
	//printf("window size: %d / %d.\n", painter.window().width(), painter.window().height());
	//printf("scaling by %.2f / %.2f\n", painter.window().width()/x, painter.window().height()/y);

// 	painter.scale(painter.window().width() / x / 1.5, painter.window().height() / y / 1.5);
// 	painter.scale(painter.window().width() / 30.0, painter.window().height() / 30.0);
	painter.scale(4.0 , 4.0);
	painter.translate(-120,200);

	// draw the spline0
	spline->setCursor(0);
	QPainterPath path0(scaleDownPoint(spline->evaluate0()));
	while(spline->getCursor() < spline->getLength() && spline->advance(100000))
		path0.lineTo(scaleDownPoint(spline->evaluate0()));
	painter.drawPath(path0);

	/*
	// draw the spline1
	spline->setCursor(0);
	QPainterPath path1(spline->evaluate1() / 1000000);
	while(spline->getCursor() < spline->getLength() && spline->advance(1000000))
		path1.lineTo(spline->evaluate1() + spline->evaluate0() / 1000000);
	painter.setPen(QColor("blue"));
	painter.drawPath(path1);

	// draw the spline2
	spline->setCursor(0);
	QPainterPath path2(spline->evaluate2() / 1000000);
	while(spline->getCursor() < spline->getLength() && spline->advance(1000000))
		path2.lineTo(spline->evaluate2() + spline->evaluate0() / 1000000);
	painter.setPen(QColor("green"));
	painter.drawPath(path2);
*/

	// draw the waypoints
	painter.setPen(QColor("red"));
	QList<QPoint> points = spline->getWayPoints();
	for(int i=0; i < points.size(); i++)
	{
		QString text = QString("  ") + QString::number(i) + QString(": ") + QString::number(points[i].x() / 1000000.0, 'F', 2) + QString(" / ") + QString::number(points[i].y() / 1000000.0, 'F', 2);

		QPointF currentWayPoint = scaleDownPoint(points[i]);
// 		scaleDownPoint(currentWayPoint);
		painter.drawText(currentWayPoint, text);
		painter.drawEllipse(QRect(currentWayPoint.x()-1, currentWayPoint.y()-1, 1, 1));
	}

	// draw the moving point
	painter.setPen(QColor("blue"));
	painter.setFont(QFont("Arial", 5));
	int oldCursor = spline->getCursor();
	spline->setCursor(movingPoint);

	QPointF abl0 = scaleDownPoint(spline->evaluate0());
	QPointF abl1 = scaleDownPoint(spline->evaluate1());
	QPointF abl2 = scaleDownPoint(spline->evaluate2());

	painter.setPen(QColor("blue"));
	painter.drawLine(abl0, abl0 + abl1);
	painter.setPen(QColor("green"));
	painter.drawLine(abl0, abl0 + abl2);

	QString text = QString::number(spline->getAngleDeg(), 'F', 2);
	painter.setFont(QFont("Arial", 5));
	painter.setPen(QColor("black"));
	painter.drawText(abl0, text);
	painter.drawEllipse(QRectF(abl0.x()-0.5, abl0.y()-0.5, 1.0, 1.0));
	spline->setCursor(oldCursor);
}

QPointF CurveTest::scaleDownPoint(QPoint point, int factor)
{
	QPointF result;
	result.setX((double)point.x() / (double)factor);
	result.setY(-(double)point.y() / (double)factor);
// 	printf("returning %.2F / %.2F.\n", result.x(), result.y());
	return result;
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	Logger::setupLogging(argc, argv);

	CurveTest curvetest;
	curvetest.show();

	return app.exec();
}
