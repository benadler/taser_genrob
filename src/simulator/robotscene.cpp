#include "robotscene.h"

RobotScene::RobotScene(RobotControls* robotControls) : QGraphicsScene()
{
	this->robotControls = robotControls;

	connect(robotControls, SIGNAL(newPose(double, double, double)), this, SLOT(slotSetPose(double, double, double)));
	connect(this, SIGNAL(newPose(double, double, double)), robotControls, SLOT(slotSetPose(double, double, double)));

	// make sure the robot gets updated regularly.
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdateRobot()));

	// for the clearTrace-Button
	connect(robotControls->btnClearTrace, SIGNAL(clicked()), this, SLOT(slotClearTrace()));

	// when robotcontrols has new splines for us, process them
	connect(robotControls, SIGNAL(showSplineCubic(bool, QList<QPointF>)), this, SLOT(slotUpdateSplineCubic(bool, QList<QPointF>)));
	connect(robotControls, SIGNAL(showSplineHermite(bool, QList<QPointF>, float, float)), this, SLOT(slotUpdateSplineHermite(bool, QList<QPointF>, float, float)));

	config = Configuration::instance();

	// initialize the random number generator. Used for movement noise.
	struct timeval now;
	gettimeofday (&now, 0x0);
	srand(now.tv_usec);

	scaleFactor = 20.0;

	double wheelWidth = 0.04 * scaleFactor;
	double wheelHeight = 0.15 * scaleFactor;

	pose.setPose(QPointF(13.0, 13.0), 0.0);
	robotAngle = 0.0;

	splineCubic = 0;
	splineCubicPath = 0;
	splineCubicPathItem = 0;
	splineHermite = 0;
	splineHermitePath = 0;
	splineHermitePathItem = 0;

	iterations = 0;

	robotItem = new RobotItem(-0.35*scaleFactor, -0.35*scaleFactor, 0.7*scaleFactor, 0.7*scaleFactor);
	connect(robotItem, SIGNAL(newPose(double, double, double)), SLOT(slotSetPose(double, double, double)));

	robotPath = new QPainterPath();
	robotPathItem = new QGraphicsPathItem();
	robotPathItem->setPen(QPen(Qt::green));

	wheelL = new QGraphicsRectItem(-wheelHeight/2, -wheelWidth/2, wheelHeight, wheelWidth, robotItem);
	wheelR = new QGraphicsRectItem(-wheelHeight/2, -wheelWidth/2, wheelHeight, wheelWidth, robotItem);

	wheelL->setPos(0, -scaleFactor * config->getAxisLength() / 2.0);
	wheelR->setPos(0, +scaleFactor * config->getAxisLength() / 2.0);

	// the axis goes from left to right wheel.
	axis = new QGraphicsLineItem(
		wheelL->pos().x(),
		wheelL->pos().y(),
		wheelR->pos().x(),
		wheelR->pos().y(),
		robotItem
	);

	// fake items, used for grouping
	laserMarksParent = new QGraphicsRectItem(1000, 1000, 0, 0);
	tracerParent = new QGraphicsRectItem(1, 1, 0, 0);
	linesParent = new QGraphicsRectItem(2, 2, 0, 0);

	addItem(robotItem);
	addItem(robotPathItem);
	addItem(laserMarksParent);

// 	QGraphicsTextItem* labelL = new QGraphicsTextItem("l", wheelL);
// 	labelL->setFlags(QGraphicsItem::ItemIgnoresTransformations);
// 	labelL->setFont(QFont("Arial", 30));
// 	addItem(labelL);
//
// 	QGraphicsTextItem* labelR = new QGraphicsTextItem("r", wheelR);
// 	labelR->setFlags(QGraphicsItem::ItemIgnoresTransformations);
// 	labelR->setFont(QFont("Arial", 30));
// 	addItem(labelR);

	//1000/25
	timer->start(50);

	loadLaserMarks(config->getKnownLaserMarksFilename());
	loadLines(config->getLinesFilename());
}

void RobotScene::slotSetPose(double x, double y, double angle)
{
// 	qDebug() << "RobotScene::slotSetPose()" << x << y << angle;

// 	resetRobotPath();

	if(angle > -500.0)
	{
// 		qDebug() << "RobotScene::slotSetPose(): angle is greater than -500, source is robotcontrol UI";
		// We're being called from robotControl's UI, cause the angle has a valid value.
		pose.setX(x);
		pose.setY(y);
		pose.setAngleDeg(angle);

		moveRobotToPose(pose);
	}
	else
	{
// 		qDebug() << "RobotScene::slotSetPose(): angle is SMALLER than -500, source is dragging";
		// We're being called because the robot was dragged. Tell robotcontrols!
		pose.setX(x / scaleFactor);
		pose.setY(y / -scaleFactor);

// 		qDebug("RobotScene::slotSetPose(): telling robotControls UI about  enw values from dragging: %.2F, %.2F, %.2F\n", x, y, pose.getAngleDeg());
		robotControls->slotSetPose(pose.getX(), pose.getY(), pose.getAngleDeg());
	}
}

void RobotScene::loadLaserMarks(QString filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug("couldn't open file %s", qPrintable(filename));
		return;
	}

	// delete the old marks
	QList<QGraphicsItem*> oldLaserMarks = laserMarksParent->children();
	qDebug("now deleting %d lasermarks.", oldLaserMarks.size());
	foreach(QGraphicsItem* item, oldLaserMarks)
	{
		removeItem(item);
		delete item;
	}

	removeItem(laserMarksParent);

	QTextStream in(&file);
	while(!in.atEnd())
	{
		QString line = in.readLine();
		double x = line.section(" ", 0, 0).toInt() / 1000.0 * scaleFactor;
		double y = -line.section(" ", 1, 1).toInt() / 1000.0 * scaleFactor;

		QGraphicsRectItem* laserMark = new QGraphicsRectItem(x-1, y-1, 2, 2, laserMarksParent);
		QPen pen(Qt::red);
		pen.setWidth(1);
		laserMark->setPen(pen);
	}

	addItem(laserMarksParent);
}

void RobotScene::loadLines(QString filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug("couldn't open file %s", qPrintable(filename));
		return;
	}

	// delete the old marks
	QList<QGraphicsItem*> oldLines = linesParent->children();
	qDebug("now deleting %d lines.", oldLines.size());
	foreach(QGraphicsItem* item, oldLines)
	{
		removeItem(item);
		delete item;
	}

	removeItem(linesParent);

	QTextStream in(&file);
	while(!in.atEnd())
	{
		QString line = in.readLine();
		double x1 = line.section(" ", 0, 0).toFloat() * scaleFactor;
		double y1 = -line.section(" ", 1, 1).toFloat() * scaleFactor;
		double x2 = line.section(" ", 2, 2).toFloat() * scaleFactor;
		double y2 = -line.section(" ", 3, 3).toFloat() * scaleFactor;

		QGraphicsLineItem* lineItem = new QGraphicsLineItem(x1, y1, x2, y2, linesParent);
		lineItem->setPen(QPen(Qt::gray));
	}

	addItem(linesParent);
}

void RobotScene::slotClearTrace()
{
	// delete the old tracers
	QList<QGraphicsItem*> oldTracerMarks = tracerParent->children();
	//qDebug("now deleting %d tracemarks.", oldTracerMarks.size());
	foreach(QGraphicsItem* item, oldTracerMarks)
	{
		removeItem(item);
		delete item;
	}

	resetRobotPath();
}

void RobotScene::resetRobotPath(void)
{
	// replace the old path.
	delete robotPath;
	robotPath = new QPainterPath(robotItem->pos());
	robotPathItem->setPath(*robotPath);
}

void RobotScene::slotUpdateRobot(void)
{
	// How much farther did the motors move between the last call and now?
	int advanceL = 0, advanceR = 0;
	robotControls->getMotorAdvances(2, advanceL, advanceR);

	if(advanceL == 0 && advanceR == 0) return;

	//qDebug("RobotScene::slotUpdateRobot(): robot advanced l %d r %d meters", advanceL, advanceR);

	//qDebug() << "RobotScene::slotUpdateRobot(): old pose is" << pose.toString(true);

	// add wheel movement noise (see man 3 rand). If maxNoise is e.g. 70, make the noise go from -70% to +70%.
	double noiseWheelInPercentL = ((double)(2*robotControls->noise->value())*rand()/(RAND_MAX+1.0))- robotControls->noise->value();
	double noiseWheelInPercentR = ((double)(2*robotControls->noise->value())*rand()/(RAND_MAX+1.0))- robotControls->noise->value();

	advanceL += (int)(advanceL * (noiseWheelInPercentL * 0.01));
	advanceR += (int)(advanceR * (noiseWheelInPercentR * 0.01));

	// add a tracer item to our current position
	if(iterations == 0)
	{
		// lengthen the path to the robot's current position
		robotPath->lineTo(robotItem->pos());
		robotPathItem->setPath(*robotPath);

		// update the path label's length value
		robotControls->labelPathLength->setText(QString::number(robotPath->length() / scaleFactor, 'f', 3).append(" m"));

		// add a tracer circle
		removeItem(tracerParent);
// 		qDebug() << "adding a tracer item to" << robotItem->pos();
		QRectF newTracerRect(robotItem->pos().x()-1, robotItem->pos().y()-1, 2, 2);
// 		QGraphicsEllipseItem* tracer = new QGraphicsEllipseItem(newTracerRect, tracerParent);
// 		tracer->setPen(QPen(QColor("red")));
		addItem(tracerParent);
	}

	pose.advance(advanceL, advanceR);

	//qDebug() << "RobotScene::slotUpdateRobot(): new pose is" << pose.toString(true);

	moveRobotToPose(pose);

	QPointF centerOfRobot = axis->line().pointAt(0.5);

	// update the display of the robot's position and angle
	emit newPose(robotItem->pos().x() / scaleFactor, -robotItem->pos().y() / scaleFactor, pose.getAngleDeg());

	// make sure that all views have the robot centered
	if(robotControls->chkKeepInView->isChecked())
		foreach(QGraphicsView *view, views())
			view->centerOn(robotItem);

	iterations++;
	iterations %= 10;
}

void RobotScene::moveRobotToPose(Pose target)
{
// 	qDebug("RobotScene::moveRobotToPose(): moving robot to %s", qPrintable(target.toString(true)));

	double angleDiff = robotAngle - target.getAngleDeg();
	robotAngle = target.getAngleDeg();

	// y-values are negative because qt-coordinate system's y-values grow from top to bottom.

	robotItem->setPos
	(
		target.getX() * scaleFactor,
		-target.getY() * scaleFactor
	);
	robotItem->rotate(angleDiff);
}

RobotScene::~RobotScene()
{
	delete wheelL;
	delete wheelR;
	delete axis;
	delete robotItem;
	delete robotPath;
	delete robotPathItem;
}

void RobotScene::slotUpdateSplineCubic(bool show, QList<QPointF> points)
{
	// Delete the old spline;
	if(splineCubic != 0)
	{
		// first remove old waypoints
		QList<QGraphicsItem*> oldWayPoints = splineCubicPathItem->children();
		foreach(QGraphicsItem* item, oldWayPoints)
		{
			removeItem(item);
			delete item;
		}

		delete splineCubic;
		splineCubic = 0;

		delete splineCubicPath;
		splineCubicPath = 0;

		removeItem(splineCubicPathItem);
		delete splineCubicPathItem;
		splineCubicPathItem = 0;
	}

	if(show)
	{
		splineCubic = new SplineCubic(points);
		splineCubicPath = new QPainterPath(scalePoint(splineCubic->evaluate0(0.0)));

		while(splineCubic->getCursor() < splineCubic->getLength() && splineCubic->advance(0.01))
			splineCubicPath->lineTo(scalePoint(splineCubic->evaluate0()));

		splineCubicPathItem = new QGraphicsPathItem(*splineCubicPath);
		splineCubicPathItem->setFlags(QGraphicsItem::ItemIsMovable);
		splineCubicPathItem->setPen(QPen(QColor(50, 50, 255)));

		// add waypoints
		foreach(QPointF wayPoint, points)
		{
			QPointF tempPoint = scalePoint(wayPoint);
			QGraphicsRectItem* temp = new QGraphicsRectItem(tempPoint.x()-1, tempPoint.y()-1, 2, 2, splineCubicPathItem);
			temp->setPen(QPen(QColor(255, 0, 0)));
		}

		// add polyline as guide
		QPainterPath* splineCubicPathPoly = new QPainterPath(scalePoint(points.at(0)));
		for(int i = 1; i < points.size(); ++i)
			splineCubicPathPoly->lineTo(scalePoint(points.at(i)));
		QGraphicsPathItem *splineCubicPathPolyItem = new QGraphicsPathItem(*splineCubicPathPoly, splineCubicPathItem);
		splineCubicPathPolyItem->setPen(QPen(QColor(0, 0, 0, 100)));

		addItem(splineCubicPathItem);
	}
}

void RobotScene::slotUpdateSplineHermite(bool show, QList<QPointF> points, float tension, float bias)
{
	// Delete the old spline;
	if(splineHermite != 0)
	{
		// first remove old waypoints
		QList<QGraphicsItem*> oldWayPoints = splineHermitePathItem->children();
		foreach(QGraphicsItem* item, oldWayPoints)
		{
			removeItem(item);
			delete item;
		}

		delete splineHermite;
		splineHermite = 0;

		delete splineHermitePath;
		splineHermitePath = 0;

		removeItem(splineHermitePathItem);
		delete splineHermitePathItem;
		splineHermitePathItem = 0;
	}


	if(show)
	{
		splineHermite = new SplineHermite(points);
		splineHermite->setParameters(tension, bias);
		splineHermitePath = new QPainterPath(scalePoint(splineHermite->evaluate0(0.0)));

		while(splineHermite->getCursor() < splineHermite->getLength() && splineHermite->advance(0.01))
			splineHermitePath->lineTo(scalePoint(splineHermite->evaluate0()));

		splineHermitePathItem = new QGraphicsPathItem(*splineHermitePath);
		splineHermitePathItem->setFlags(QGraphicsItem::ItemIsMovable);
		splineHermitePathItem->setPen(QPen(QColor(50, 50, 255)));

		// add waypoints
		foreach(QPointF wayPoint, points)
		{
			QPointF tempPoint = scalePoint(wayPoint);
			QGraphicsRectItem* temp = new QGraphicsRectItem(tempPoint.x()-1, tempPoint.y()-1, 2, 2, splineHermitePathItem);
			temp->setPen(QPen(QColor(255, 0, 0)));
		}

		// add polyline as guide
		QPainterPath* splineHermitePathPoly = new QPainterPath(scalePoint(points.at(0)));
		for(int i = 1; i < points.size(); ++i)
			splineHermitePathPoly->lineTo(scalePoint(points.at(i)));
		QGraphicsPathItem *splineHermitePathPolyItem = new QGraphicsPathItem(*splineHermitePathPoly, splineHermitePathItem);
		splineHermitePathPolyItem->setPen(QPen(QColor(0, 0, 0, 100)));

		addItem(splineHermitePathItem);
	}
}

QPointF RobotScene::scalePoint(const QPointF &point)
{
	return QPointF(point.x() * scaleFactor, point.y() * -scaleFactor);
}
