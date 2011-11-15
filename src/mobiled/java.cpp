#include "java.h"

// Initialize the singleton-pointer to NULL
QMutex Java::singletonMutex;
Java* Java::singleton = NULL;

Java* Java::instance()
{
	QMutexLocker locker(&singletonMutex);

	if(singleton == NULL)
	{
		// First instantiation. Make sure to not only create the new object,
		// but also initialize the thread, so the clients don't have to care.
// 		printf("Java::instance(): first instantiation.\n");
		singleton = new Java();
		singleton->moveToThread(singleton);
		qDebug() << "Java::instance(): now starting java-thread!";
		singleton->start();
// 		sleep(2);
		qDebug() << "Java::instance(): first instantiation done.";
	}

	return singleton;
}

Java::Java(void)
{
	mutex = new QMutex();
	QMutexLocker locker(mutex);

	logger = Logger::instance();
	config = Configuration::instance();

	logger->Java("Java::Java()");

	qRegisterMetaType<Pose>("Pose");
	qRegisterMetaType<QList<QPointF> >("QList<QPointF>");
	qRegisterMetaType<QList<LaserMarkKnown> >("QList<LaserMarkKnown>");
}

void Java::initialize()
{
	logger->Java("Java::initialize(): creating jvm in thread %d", syscall(SYS_gettid));

	vm_args.version = JNI_VERSION_1_4;

	options[0].optionString = "-Djava.compiler=NONE";		// disable JIT
	// This is where we need the path to the Roblets, jars etc. No use in settings $CLASSPATH in bash, its ignored!
	options[1].optionString = "-Djava.class.path=./:../:/usr/local/src/service_robot/mobiled/genrob/jni/:/usr/local/src/servicerobot/mobiled/roblet/jni/:../../genrob/jni/:../../genrob/genControl/:genRob.genControl.jar:genRob.genControl.client.jar:genRob.genPath.jar:uhh.fbi.tams.pathplanner.jar:org.roblet.jar:uhh.fbi.tams.iowarrior.unit.jar:/usr/src/politbuero/mobiled/genrob/genMap/";	// user classes
	options[2].optionString = "-Djava.library.path=/opt/sun-jdk-1.6.0.03/jre/lib/";	// set native library path
	options[2].optionString = "-Djava.library.path=/usr/lib/java/jre/lib/";	// set native library path
	options[3].optionString = "-verbose:none";			// jni/class/...

	vm_args.version = JNI_VERSION_1_2;
	vm_args.options = options;
	vm_args.nOptions = 4;
	vm_args.ignoreUnrecognized = FALSE;

	// load and initialize a Java VM, return a JNI interface pointer in env
	if(JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args) == JNI_ERR)
		abort("Java::Java(): JNI_CreateJavaVM() failed.");
	else
		logger->Java("Java::Java(): JNI_CreateJavaVM() succeeded.");

	robletObstacles = env->FindClass("RobletObstacles");
	if(env->ExceptionOccurred()) env->ExceptionDescribe();
	if(robletObstacles == 0) abort("Java::Java(): Java class RobletObstacles not found");
	robletObstaclesMethodAdd = env->GetStaticMethodID(robletObstacles, "add", "([I[II)V");
	if(robletObstaclesMethodAdd == 0) abort("Java::Java(): Java method RobletObstacles::add() not found.");

	robletObstaclesMethodRemove = env->GetStaticMethodID(robletObstacles, "remove", "([I[I)V");
	if(robletObstaclesMethodRemove == 0) abort("Java::Java(): Java method RobletObstacles::remove() not found.");

	robletPath = env->FindClass("RobletPath");
	if(env->ExceptionOccurred()) env->ExceptionDescribe();
	if(robletPath == 0) abort("Java::Java(): Java class RobletPath not found");
	robletPathMethodGetPath = env->GetStaticMethodID(robletPath, "getPath", "(IIIII)[I");
	if(robletPathMethodGetPath == 0) abort("Java::Java(): Java method RobletPath::getPath() not found.");

	robletLaserMarks = env->FindClass("RobletLaserMarks");
	if(env->ExceptionOccurred()) env->ExceptionDescribe();
	if(robletLaserMarks == 0) abort("Java::Java(): Java class RobletLaserMarks not found");
	robletLaserMarksMethodGet = env->GetStaticMethodID(robletLaserMarks, "get", "()[I");
	if(robletLaserMarksMethodGet == 0) abort("Java::Java(): Java method RobletLaserMarks::get() not found.");

	robletIoWarrior = env->FindClass("RobletIOWarrior");
	if(env->ExceptionOccurred()) env->ExceptionDescribe();
	if(robletIoWarrior == 0) abort("Java::Java(): Java class RobletIOWarrior not found");
	robletIoWarriorMethodSetPort = env->GetStaticMethodID(robletIoWarrior, "setPort", "(IZ)V");
	if(env->ExceptionOccurred()) env->ExceptionDescribe();
	if(robletIoWarriorMethodSetPort == 0) abort("Java::Java(): Java method RobletIOWarrior::setPort() not found.");
}

Java::~Java(void)
{
	logger->Java("Java::~Java(): shutting down.");
	qDebug("Java::~Java(): shutting down.");
	delete mutex;

	// we are done
	jvm->DestroyJavaVM();

	logger->Java("Java::~Java(): done shutting down.");
}

void Java::run(void)
{
	initialize();
// 	logger->Java("Java::run(): attaching jvm to thread-id %d.", syscall(SYS_gettid));
//	jvm->AttachCurrentThread((void**)env, NULL);
	logger->Java("Java::run(): starting event loop, thread-id is %d", syscall(SYS_gettid));
	exec();
}

void Java::slotSetIoWarriorPort(const unsigned int port, const bool enable) const
{
	QMutexLocker locker(mutex);

	logger->Java("Java::slotSetIoWarriorPort()");

	jint jport = port;
	jboolean jenable = enable;

	env->CallStaticVoidMethod(robletIoWarrior, robletIoWarriorMethodSetPort, jport, jenable);

	if(env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		logger->Java("Java::slotSetIoWarriorPort(): setPort() threw an exception, the port was probably NOT set.");
	}
	else
	{
		logger->Java("Java::slotSetIoWarriorPort(): successful, returning.");
	}
}

void Java::slotAddObstaclesToMap(const QList<QPointF> &obstacles) const
{
	QMutexLocker locker(mutex);

	jint x[obstacles.size()];
	jint y[obstacles.size()];
	logger->Java("Java::slotAddObstaclesToMap(): created jint arrays with %d elements each.", obstacles.size());

	for (int i=0;i<obstacles.size();i++)
	{
		x[i] = (int)(obstacles[i].x() * 1000);
		y[i] = (int)(obstacles[i].y() * 1000);
	}

	jintArray xArray = env->NewIntArray(obstacles.size());
	jintArray yArray = env->NewIntArray(obstacles.size());

	env->SetIntArrayRegion(xArray, 0, (int)obstacles.size(), x);
	env->SetIntArrayRegion(yArray, 0, (int)obstacles.size(), y);

	env->CallStaticVoidMethod(robletObstacles, robletObstaclesMethodAdd, xArray, yArray, config->getObstacleTimeout());

	if(env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		logger->Java("Java::slotAddObstaclesToMap(): add() threw an exception, obstacles were NOT added to genMap, returning.");
	}
	else
	{
		logger->Java("Java::slotAddObstaclesToMap(): successful, returning.");
	}
}

void Java::slotRemoveObstaclesFromMap(const QList<QPointF> &obstacles) const
{
	QMutexLocker locker(mutex);

	jint x[obstacles.size()];
	jint y[obstacles.size()];
	logger->Java("Java::slotRemoveObstaclesFromMap(): created jint arrays with %d elements each.", obstacles.size());

	for (int i=0;i<obstacles.size();i++)
	{
		x[i] = (int)(obstacles[i].x() * 1000);
		y[i] = (int)(obstacles[i].y() * 1000);
	}

	jintArray xArray = env->NewIntArray(obstacles.size());
	jintArray yArray = env->NewIntArray(obstacles.size());

	env->SetIntArrayRegion(xArray, 0, (int)obstacles.size(), x);
	env->SetIntArrayRegion(yArray, 0, (int)obstacles.size(), y);

	env->CallStaticVoidMethod(robletObstacles, robletObstaclesMethodRemove, xArray, yArray);

	if(env->ExceptionOccurred())
	{
		env->ExceptionDescribe();
		logger->Java("Java::slotRemoveObstaclesFromMap(): add() threw an exception, obstacles were NOT removed from genMap, returning.");
	}
	else
	{
		logger->Java("Java::slotRemoveObstaclesFromMap(): successful, returning.");
	}
}

void Java::slotGetWayPoints(const Pose start, const Pose end)
{
	QMutexLocker locker(mutex);

	logger->Java
	(
		"Java::slotGetWayPoints(): now processing waypoint request from %s to %s.",
		qPrintable(start.toString()),
		qPrintable(end.toString())
	);

	QList<QPointF> wayPoints;

	jintArray array = (jintArray)env->CallStaticObjectMethod(
		robletPath,
		robletPathMethodGetPath,
		(int)((config->getRobotRadius() + 0.15) * 1000),	// add 15cm safety to pathplanning
		(int)(start.getX()*1000),
		(int)(start.getY()*1000),
		(int)(end.getX()*1000),
		(int)(end.getY()*1000));

	if(env->ExceptionOccurred() != 0)
	{
		env->ExceptionDescribe();
		logger->Java("Java::slotGetWayPoints(): getPath() threw an exception, is genPath running? Emitting empty waypoint list.");
		emit wayPointsReady(wayPoints);
		return;
	}

	jboolean isCopy = JNI_FALSE;
	jint* elements = env->GetIntArrayElements(array, &isCopy);
	if(!elements) abort("Java::slotGetWayPoints(): could not get access to waypoint result array.");

	int arrayLength = env->GetArrayLength(array);

	// extra check, should already have been caught by the Exception check above.
	if(arrayLength == 0)
	{
		logger->Java("Java::slotGetWayPoints(): received an empty waypoint list. Is start/end too close to an obstacle?");
		emit wayPointsReady(wayPoints);
		return;
	}

	// convert returned data into a list of QPointFs.
	for(int i = 0; i < arrayLength; i+=2)
	{
			//logger->Java("Java::slotGetWayPoints(): list has %d elements, now accessing x ([%d]=%d) and y ([%d]=%d).", arrayLength, i, elements[i], i+1, elements[i+1]);
		wayPoints.append(QPointF(elements[i+0] / 1000.0, elements[i+1] / 1000.0));
	}

	env->ReleaseIntArrayElements(array, elements, JNI_ABORT);


	logger->Java("Java::slotGetWayPoints(): received %d waypoints (including start and end).", wayPoints.size());

	optimizeWayPoints(wayPoints);

	emit wayPointsReady(wayPoints);
}

void Java::slotRequestLaserMarksFromGenMap()
{
	QMutexLocker locker(mutex);

	logger->Java("Java::slotRequestLaserMarksFromGenMap(): asking genMap for known Reflectors/LaserMarks in thread %d.", syscall(SYS_gettid));

	jintArray array = (jintArray)env->CallStaticObjectMethod(robletLaserMarks, robletLaserMarksMethodGet);

	if(env->ExceptionOccurred() != 0)
	{
		env->ExceptionDescribe();
		logger->Java("Java::slotRequestLaserMarksFromGenMap(): get() threw an exception, is genMap running? Emitting empty laserMarks list, this'll hurt!");
	}

	jboolean isCopy = JNI_FALSE;
	jint* elements = env->GetIntArrayElements(array, &isCopy);
	if(!elements) abort("Java::slotRequestLaserMarksFromGenMap(): could not get access to LaserMarks result array.");

	int arrayLength = env->GetArrayLength(array);

	QList<LaserMarkKnown> laserMarks;

	for(int i = 0; i < arrayLength; i+=2)
	{
		//logger->Java("Java::getLaserMarksFromGenMap(): List has %d elements, now accessing x ([%d]=%d) and y ([%d]=%d).", arrayLength, i, elements[i], i+1, elements[i+1]);
		laserMarks.append(LaserMarkKnown(QPointF(elements[i+0] / 1000.0, elements[i+1] / 1000.0)));
	}

	env->ReleaseIntArrayElements(array, elements, JNI_ABORT);

	logger->Java("Java::slotRequestLaserMarksFromGenMap(): received %d LaserMarks.", laserMarks.size());

	emit laserMarksReady(laserMarks);
}

void Java::optimizeWayPoints(QList<QPointF> &points) const
{
	logger->Java("Java::optimizeWayPoints(): working on %d waypoints.", points.size());
	foreach(const QPointF &point, points)
		logger->Java("%.2F / %.2F", point.x(), point.y());

	// THIS IS AN EXPERIMENT! To keep the spline from looping wildly, we
	// insert additional points between two points that are more that X
	// meters apart.
	const float distanceMin = config->getPathOptimizationInterval();

	bool pointsUpdated = false;

	do
	{
		pointsUpdated = false;
		QMutableListIterator<QPointF> i(points);
		while(i.hasNext())
		{
			QPointF pointA = i.next();

			// quit if we're at the last point.
			if(! i.hasNext()) continue;
			QPointF pointB = i.peekNext();
			// Compare the difference of the current point with the next
			// We only iterate to size()-1, because the last one doesn't
			// have a point following it.
			const float distance = Conversion::distanceBetween(pointA, pointB);
			//logger->Spline("SplineCubic::SplineCubic(): distance between is %.2F.", distance);

			if(distance > distanceMin)
			{
				QPointF pointBetween = pointA + ((pointB - pointA) / 2);
				logger->Java("Java::optimizeWayPoints(): dist is %.2F, inserting point %.2F/%.2F between %.2F/%.2F and %.2F/%.2F",
				               distance,
				               pointBetween.x(), pointBetween.y(),
				               pointA.x(), pointA.y(),
				               pointB.x(), pointB.y()
				              );
				i.insert(pointBetween);
				pointsUpdated = true;
			}
		}
	}
	while(pointsUpdated);

	logger->Java("Java::optimizeWayPoints(): optimized to %d waypoints.", points.size());

	foreach(const QPointF &point, points)
		logger->Java("%.2F / %.2F", point.x(), point.y());

}
