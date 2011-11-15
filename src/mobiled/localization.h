#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <QList>
#include <QTime>

#include "pose.h"
#include "configuration.h"
#include "lasermark.h"
#include "correspondence.h"
#include "logger.h"

///
/// @brief This class manages the robot's self-localization using a Kalmann-filter.
/// This is wicked math (!) and was done by Daniel Westhoff and friends.
///

class Localization : public QObject
{
	Q_OBJECT

	private:
		static Localization* singleton;	///< pointer to the only instance of Localization (which is a singleton class).
		static QMutex singletonMutex;

		QMutex* mutex;		///< used for thread-synchronization in member methods.

		Configuration* config;

		Correspondence* correspondence;

		/// Keeps the radiant(?)-angle between -180.0 and +180.0 degrees.
		//void clipAngle(double &angle);

		/// Initialisierung der Kovarianzmatrix
		void initCovariance(void);

		/// Constructor. Private, as this is a singleton
		Localization(void);

		/// Pose (estimate) of the robot.
		Pose* currentRobotPose;

		Logger* logger;

		QTime timeOfLastOdometryPrediction;

		/// Schätzung der Konvarianzmatrix der Pose des Roboters.
		float _P [3][3];

		///
		/// This method gets each detected laser reflector fed into it. It then updates the current pose by analyzing the
		/// distance and orientation to the detected laser-mark.
		/// @param measuredDistanceToMark the measured distance to the mark in meters.
		/// @param measuredAngleToMark the measured angle to the mark in radiants.
		/// @param knownMark the coordinates of the known mark on the map as QPoint. Unit is micrometers.
		///
		void updatePoseUsingSingleMark(const LaserMarkMeasured markMeasured, const LaserMarkKnown markKnown);

	public:
		///
		/// This static method returns a pointer to the only Localization-object in existence. If there is no
		/// Localization-object yet, one will be created before returning a pointer to it.
		///
		static Localization* instance(void);

		/// Destruktor.
		~Localization(void);

		///
		/// This method updates the robot's pose based on its currentRobotPose (which is a few milliseconds old) and its current translational and rotational velocities.
		/// @param speedTranslation the robot's current translational speed in m/s.
		/// @param speedRotation the robot's current rotational speed in deg/s.
		///
		void updatePoseUsingWheelSpeeds(const int speedL, const int speedR);

// 		void updatePoseUsingWheelAdvances(const int advL, const int advR);

		///
		/// This method takes a list of known marks. For each measured mark, it asks the Correspondence-class
		/// for the corresponding known mark and passes both to updatePoseUsingSingleMark().
		/// @param markDistances the list containing the distances to the measured marks
		/// @param markAngles the list containing the angles to the measured marks
		/// @param matchedMarkIds this list can be filled with the IDs of the known marks, in case those could be found. Currently unused.
		///
		void updatePoseUsingLaserMarks(QList<LaserMarkMeasured> &marks);

		///
		/// This method sets the robot's internal pose
		/// @param pose the pose to which the robot will be set
		///
		void setPose(const Pose pose);

		///
		/// This method returns the robot's current pose.
		/// @return the root's current pose
		///
		Pose getPose(void) const;





};

#endif
