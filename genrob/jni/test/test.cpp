#include <QCoreApplication>
#include <QList>
#include <QPointF>

#include "java.h"
#include "logger.h"

int main(int argc, char** argv)
{
	Logger::setupLogging(argc, argv);

	QCoreApplication app(argc, argv);

	printf("instantiating Java singleton...\n");

	Java* java = Java::instance();

	QList<QPointF> obstacles;

	for(int i=3;i < 7;i++)
	{
		obstacles << QPointF(i, i);
	}
	obstacles << QPointF(16.4, 8.3);

	printf("adding obstacles...\n");

	java->start();

	java->addObstaclesToMap(obstacles, 100000);

	printf("added obstacles to map.\n");

	java->addObstaclesToMap(obstacles, 100000);

	printf("added obstacles to map.\n");

// 	sleep(2);

	printf("retrieving laserMarks...\n");
	java->getLaserMarksFromGenMap();

// 	sleep(2);

	printf("sending waypoint request...\n");
	java->sendWayPointRequest(Pose(13.3, 13.5), Pose(19.5, 8.5));
	app.exec();
}
