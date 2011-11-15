#ifndef CORRESPONDENCE_H
#define CORRESPONDENCE_H

#include <QList>
#include <QPoint>
#include <QFile>
#include <QTransform>
#include <math.h>

#include "logger.h"
#include "pose.h"
#include "java.h"
#include "configuration.h"
#include "lasermark.h"

/// @brief This class manages known LaserMarks and associates them to measured LaserMarks
///
/// This class can load a list of known LaserMarks either from a given file or from genMap.
/// When given the robot's current position and a LaserMarkMeasured, it can associate a
/// known LaserMark to the measured LaserMark. Using this mapping of marks, Localization can
/// find a more precise current robotpose.

class Correspondence : public QObject
{
	Q_OBJECT

	private:
		static Correspondence* singleton;	///< pointer to the only instance of Localization (which is a singleton class).
		static QMutex singletonMutex;

		QMutex* mutex;		///< used for thread-synchronization in member methods.

		/// Constructor.
		Correspondence();

		/// Destructor.
		~Correspondence(void);

		QList<LaserMarkKnown> knownLaserMarks;	///< this list holds all known marks
		Logger* logger;
		Java* java;

	public:
		/// This static method returns a pointer to the only Correspondence-object in existence. If there is no
		/// Correspondence-object yet, one will be created before returning a pointer to it.
		static Correspondence* instance(void);

		/// This method returns a list of all known marks
		/// @param knownMarks a list of LaserMarkKnowns, which will be filled the the known LaserMarks.
		void getKnownMarks(QList<LaserMarkKnown> &knownMarks) const;

		/// This method takes the robot's Pose, a measured LaserMark and a reference to a known LaserMark.
		/// Given this information, it will try to find a LaserMarkKnown that corresponds to the given
		/// LaserMarkMeasured.
		/// @param pose the robot's pose when the LaserMarkMeasured was detected
		/// @param markMeasured a LaserMarkMeasured representing the measured LaserMark
		/// @param markKnown a reference to a LaserMarkKnown. Will be filled on success.
		/// @return true when a LaserMarkKnown could be matched to the LaserMarkMeasured, else false.
		bool getMatchingMark(const Pose &pose, const LaserMarkMeasured markMeasured, LaserMarkKnown &markKnown);

		/// This method loads known LaserMarks from a file. Currently unused, as we
		/// use genMap for LaserMark management.
		/// @param filename the file to be used for loading the known LaserMarks
		/// @return true when loading succeeded, else false
		bool loadKnownLaserMarksFromFile(const QString filename);

		/// This method saves all known LaserMarks to a file. Currently unused, as we
		/// use genMap for LaserMark management.
		/// @param filename the file to be used for saving the known LaserMarks
		/// @return true when saving succeeded, else false
		bool saveKnownLaserMarksToFile(const QString filename) const;

		/// This method requests known LaserMarks from genMap using java/jni.
		void requestKnownLaserMarksFromGenMap(void);

	public slots:
		void slotReceiveKnownLaserMarksFromGenMap(QList<LaserMarkKnown> laserMarks);
};

#endif
