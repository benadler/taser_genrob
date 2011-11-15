#include "localization.h"

/// Berechnet das Quadrat der Zahl 'a'
static inline float sqr(const float &a)
{
  return a*a;
}

// Initialize the singleton-pointer to NULL
QMutex Localization::singletonMutex;
Localization* Localization::singleton = NULL;

Localization* Localization::instance()
{
	QMutexLocker locker(&singletonMutex);

	if(singleton == NULL)
		singleton = new Localization();

	return singleton;
}

Localization::Localization(void)
{
	logger = Logger::instance();
	logger->Localization("Localization::Localization() created in thread id %d.", syscall(SYS_gettid));

	mutex = new QMutex(QMutex::NonRecursive);
	config = Configuration::instance();

	correspondence = Correspondence::instance();

	// Set the robot's initial pose.
	currentRobotPose = new Pose();
	logger->Localization("Localization::Localization(): pose is now %s.", qPrintable(currentRobotPose->toString()));

	// Kovariance-Matrix
	initCovariance();
}

Localization::~Localization(void)
{
	logger->Localization("Localization::~Localization()");
	delete currentRobotPose;
}

void Localization::setPose(const Pose pose)
{
	QMutexLocker locker(mutex);

	logger->Localization("Localization::setPose(): setting pose to %s.", qPrintable(pose.toString()));

	currentRobotPose->setX(pose.getX());
	currentRobotPose->setY(pose.getY());
	currentRobotPose->setAngleDeg(pose.getAngleDeg());

	// initialise covariance-matrix
	initCovariance();
}

Pose Localization::getPose(void) const
{
	QMutexLocker locker(mutex);

	//logger->Localization("Localization::getPose(): returning pose %s.", qPrintable(currentRobotPose->toString()));

	return Pose(*currentRobotPose);
}

void Localization::updatePoseUsingWheelSpeeds(const int speedL, const int speedR)
{
	QMutexLocker locker(mutex);

	double secondsSinceLastCall;

	// Check whether timeOfLastOdometryPrediction is defined, it won't be at the first call to this method.
	if(! timeOfLastOdometryPrediction.isValid())
		secondsSinceLastCall = 0.0;
	else
		secondsSinceLastCall = timeOfLastOdometryPrediction.msecsTo(QTime::currentTime()) / 1000.0;

	timeOfLastOdometryPrediction = QTime::currentTime();

	// Ugly hack. If this method wasn't called for a long time because the robot wasn't moving,
	// then secondsSinceLastCall will have a BIG value, which will bomb localization in the
	// first iteration. If we find such a huge value, ignore this call.
	if(secondsSinceLastCall > 0.5)
	{
		logger->Localization("Localization::updatePoseUsingWheelSpeeds(): timeDiff is more than .5 seconds. Robot must have been idle, ignoring.");
		return;
	}

	Q_ASSERT(secondsSinceLastCall >= 0.0);

	const double angleDegBefore = currentRobotPose->getAngleDeg();
	const double cos_theta = cos(currentRobotPose->getAngleRad());
	const double sin_theta = sin(currentRobotPose->getAngleRad());

	// advance the robot's pose.
	const int advanceL = (int)(speedL * secondsSinceLastCall);
	const int advanceR = (int)(speedR * secondsSinceLastCall);
	const double distanceTravelled = currentRobotPose->advance(advanceL, advanceR);

	// The following is only used for debug output!
	double angleDegDiff = currentRobotPose->getAngleDeg() - angleDegBefore;
	// If the angle goes from e.g. 0 to 345 degrees due to a clockwise rotation, we'd
	// get a diff of 345 degrees. In reality, it means we rotated by -15 degrees, so
	// make sure thats what we get:
	if(angleDegDiff > 180.0) angleDegDiff -= 360.0;
	if(angleDegDiff < -180.0) angleDegDiff += 360.0;
// 	logger->Localization
// 	(
// 		"Localization::updateWhlSpds(): l %d, r %d, timeDiff %.4Fs, distance: %.6Fm  angle: %.6Fdeg",
//   		speedL, speedR,
// 		secondsSinceLastCall,
// 		distanceTravelled,
// 		angleDegDiff
// 	);

	// A, i.e. Df/dx
	// A beschreibt die Weiterentwicklung des Systems, A verschiebt und dreht die Gaussglocke
	double A[3][3];
	A[0][0] = 1;
	A[0][1] = 0;
	A[0][2] = -distanceTravelled * sin_theta;
	A[1][0] = 0;
	A[1][1] = 1;
	A[1][2] = distanceTravelled * cos_theta;
	A[2][0] = 0;
	A[2][1] = 0;
	A[2][2] = 1;

  	// PNew = APA^T
	double PNew[3][3];	// Kovarianzmatrix, beschreibt die Gaussglocke, ihre Varianz und die Pose, d.h. deren Zentrum
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			PNew[i][j] = 0;
			for(int k=0; k<3; k++)
			{
				for(int l=0; l<3; l++)
				{
					//_P ist die Kovarianzmatrix vom vorherigen Zeitpunkt
					// Rechnung ist eine Näherung.
					PNew[i][j] += A[i][k] * _P[k][l] * A[j][l];
				}
			}
		}
	}

	// B, i.e. df/du
	#warning "Parameter hard-kodiert"
	// B beschreibt, wie gut A mit der Wirklichkeit übereinstimmt, bzw. den Fehler, den man durch Verwendung von A bekommt.
	double B[3][2];
	B[0][0] = cos_theta * 0.037;
	B[0][1] = cos_theta * 0.037;
	B[1][0] = sin_theta * 0.037;
	B[1][1] = sin_theta * 0.037;
	B[2][0] = - 0.074 / 0.6;
	B[2][1] = 0.074 / 0.6;

	// Q: noise in wheel measurement
	double Q[2][2];
	Q[0][0] = sqr(0.05 * 0.074); // small(!) numbers, which is why we shouldn't be called in too short intervals
	Q[0][1] = 0;
	Q[1][0] = 0;
	Q[1][1] = sqr(0.05 * 0.074);

	// PNew += BQB^t, "^t" heisst transponiert.
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			for(int k=0; k<2; k++)
			{
				for(int l=0; l<2; l++)
				{
					// Add noise to kovarianzmatrix.
					PNew[i][j] += B[i][k] * Q[k][l] * B[j][l];
				}
			}
		}
	}

	// _P = PNew
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			_P[i][j] = PNew[i][j];
		}
	}

	// P muss positiv semidefinit sein: Symmetrisch an der Diagonalen und die Werte müssen >0 sein.
	// Minimum für Diagonalwerte überprüfen
	if(_P[0][0] < 0.00001) _P[0][0] = 0.00001;
	if(_P[1][1] < 0.00001) _P[1][1] = 0.00001;
	if(_P[2][2] < 0.00001) _P[2][2] = 0.00001;

	// Kovarianzmatrix nicht-negativ halten
	if(_P[0][1] < 0.0) _P[0][1] = 0.0;
	if(_P[0][2] < 0.0) _P[0][2] = 0.0;
	if(_P[1][2] < 0.0) _P[1][2] = 0.0;

	// Kovarianzmatrix symmetrisch halten
	_P[1][0] = _P[0][1];
	_P[2][0] = _P[0][2];
	_P[2][1] = _P[1][2];

	// Pose ausgeben
// 	logger->Localization("Localization::updateWhlSpds(): Pose: %s, varX=%f, varY=%f, varT=%f [rad]",
// 		qPrintable(currentRobotPose->toString(false)),
// 		_P[0][0],
// 		_P[1][1],
// 		_P[2][2]);

	Q_ASSERT((_P[0][0] >= 0.0) && (_P[1][1] >= 0.0) && (_P[2][2] >= 0.0));
}

void Localization::updatePoseUsingLaserMarks(QList<LaserMarkMeasured> &marks)
{
	//logger->Localization("Localization::updateUsingLaserScanMarks(): using %d marks.", marks.size());

	// for each mark:
	//	- find a matching known mark using correspondence
	//	- pass the measured and known marks to updatePoseUsingSingleMark()

	int numMatched = 0;

  	// use the scanned marks to update the pose, one after another.
	foreach(const LaserMarkMeasured &currentMeasuredMark, marks)
	{
		LaserMarkKnown knownMark;

		// for the current scanned mark, try to find a registered mark and gets its official position
		bool markSuccessfullyMatched = correspondence->getMatchingMark(
			*currentRobotPose,
			currentMeasuredMark,
			knownMark);

		if(markSuccessfullyMatched)
		{
			numMatched++;
			updatePoseUsingSingleMark(currentMeasuredMark, knownMark);
		}
	}

// 	logger->Localization("Localization::updatePoseUsingLaserMarks(): matched %d of %d marks.", numMatched, marks.size());
}

// Positions- und Orientierungsschätzung durch Messung aktualisieren.
// Predict() nimmt die aktuelle Pose und sagt die Position zu einem Zeitpunkt in der Zukunft voraus.
// Update() nimmt Messungen und
// Update() wird für jede gemessene Lasermarke aufgerufen () die ersten beiden parameter).
//    Ausserdem werden die Koordinaten einer auf der Map gefundenen Marke, die dazu passt, mit übergeben (markx/marky).

void Localization::updatePoseUsingSingleMark(const LaserMarkMeasured markMeasured, LaserMarkKnown markKnown)
{
	QMutexLocker locker(mutex);

	// How far is the robot from the known LaserMark?
	float dx = markKnown.getWorldCoordinate().x() - currentRobotPose->getX();
	float dy = markKnown.getWorldCoordinate().y() - currentRobotPose->getY();

	// Given the robot's current pose, at which angle SHOULD I have seen the known LaserMark?
	// Pose thinks 0 degrees is looking north, mobiled thinks 0 degrees is looking east, so offset by +90
	float angleEstimated = atan2(dy, dx) - currentRobotPose->getAngleRad();
	if(Conversion::rad2deg(angleEstimated) < 0.0) angleEstimated += Conversion::deg2rad(360.0);

	// At which angle DID I see the measured LaserMark?
	float angleMeasured = atan2(markMeasured.getPlatformCoordinate().y(), markMeasured.getPlatformCoordinate().x());
	angleMeasured = angleMeasured - Conversion::deg2rad(90.0);

	if(Conversion::rad2deg(angleMeasured) < 0.0) angleMeasured += Conversion::deg2rad(360.0);
	// H: dh/dx
	float H[3];

	// r2, und somit das komplette H beschreiben die Messguete
	float r2 = sqr(dx) + sqr(dy);
	H[0] = dy / r2;
	H[1] = - dx / r2;
	H[2] = -1;

	float angleError = angleMeasured - angleEstimated;

	// what does this line do?
	angleError -= 2*M_PI * floor(angleError/(2*M_PI));      // 0..2*PI
	if(angleError > M_PI) angleError -= 2*M_PI;            // -PI..+PI
/*
	logger->Localization("Localization::updatePoseUsingSingleMark(): angleEstimated %0.3F deg, angleMeasured %0.3F deg, innovation %.2F deg.",
		Conversion::rad2deg(angleEstimated),
		Conversion::rad2deg(angleMeasured),
		Conversion::rad2deg(angleError));*/

	// Anschliessend�end werden Messwert und Prediction fusioniert. Es werden nur
	// die Winkel zu den Marken verwendet, ist genau genug.
	// PH^T
	double PHt[3];
	for(int i=0; i<3; i++)
	{
		PHt[i] = 0;
		for(int k=0; k<3; k++) PHt[i] += _P[i][k] * H[k];
	}

	// HPH^T
	double innovCov = 0;

	for(int k=0; k<3; k++)
		innovCov += H[k] * PHt[k];

	// +R
	innovCov += sqr(5 * M_PI / 180.0);

	// ()^-1
	float innovCovInv = 1 / innovCov;

	// Kalman-Gain, der Gewichtungsfaktor zwischen der Messung und der Prediction.
	float K[3];
	// innovCovInv ist die invertierte 1x1-Kovarianzmatrix der Innovation.
	// Würde man die Entfernungen zu den Marken hinzurechnen, hätte man eine 2x2 Matrix.
	for(int i=0; i<3; i++) K[i] = PHt[i] * innovCovInv;

	// _P: _P -= KPH^T
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			_P[i][j] -= K[i] * PHt[j];

	// currentRobotPose: currentRobotPose += K * angleError
	currentRobotPose->setX(currentRobotPose->getX() + K[0] * angleError);
	currentRobotPose->setY(currentRobotPose->getY() + K[1] * angleError);
	currentRobotPose->setAngleRad(currentRobotPose->getAngleRad() + K[2] * angleError);

	// bounds checking should be done in Pose-class
	/*
	currentRobotPose->setAngleRad
	(
		currentRobotPose->getAngleRad()
		-
		2 * M_PI * floor(currentRobotPose->getAngleRad() / (2 * M_PI))
	);      // 0..2*PI

	if(currentRobotPose->getAngleRad() > M_PI)
		currentRobotPose->setAngleRad(currentRobotPose->getAngleRad() - 2*M_PI);            // -PI..+PI
	*/

	// Minimum fuer Diagonalwerte ueberpruefen
	if(_P[0][0] < 0.00001) _P[0][0] = 0.00001;
	if(_P[1][1] < 0.00001) _P[1][1] = 0.00001;
	if(_P[2][2] < 0.00001) _P[2][2] = 0.00001;

	// Kovarianzmatrix positiv halten
	if(_P[0][1] < 0.0) _P[0][1] = 0.0;
	if(_P[0][2] < 0.0) _P[0][2] = 0.0;
	if(_P[1][2] < 0.0) _P[1][2] = 0.0;

	// Kovarianzmatrix symmetrisch halten
	_P[1][0] = _P[0][1];
	_P[2][0] = _P[0][2];
	_P[2][1] = _P[1][2];

	// Pose ausgeben
// 	logger->Localization("Localization::updatePoseUsingSingleMark(): new pose: %s, varX=%.2F, varY=%.2F, varT=%.2F",
// 		qPrintable(currentRobotPose->toString()),
// 		_P[0][0],
// 		_P[1][1],
// 		_P[2][2]);

	Q_ASSERT((_P[0][0] >= 0.0) && (_P[1][1] >= 0.0) && (_P[2][2] >= 0.0));
}

// Winkel zwischen -180° und +180° halten.
/*void Localization::clipAngle(double &angle)
{
	// Winkel im Intervall [-PI..PI] halten
	if(angle >= M_PI)
	{
		angle -= (2 * M_PI);
	}
	else if(angle < (-1.0 * M_PI))
	{
		angle += (2 * M_PI);
	}
}*/

// Covarianzmatrix initialisieren
void Localization::initCovariance(void)
{
	// Kovariance-Matrix
	_P[0][0] = 0.1;
	_P[0][1] = 0.0;
	_P[0][2] = 0.0;
	_P[1][0] = 0.0;
	_P[1][1] = 0.1;
	_P[1][2] = 0.0;
	_P[2][0] = 0.0;
	_P[2][1] = 0.0;
	_P[2][2] = 0.1;
}
