//
// mobile/defines.h
//

#ifndef _MOBILE_DEFINES_H_
#define _MOBILE_DEFINES_H_


// ++itschere20030204: define WITH_ODO_LOC if you want an additional
// localisation that only uses odometry information. this is only useful for
// testing the long term stability of odometry based localisation.
#define WITH_ODO_LOC


// ++itschere20030805: where to look for the map?
#define MAPFILE "/vol/biorob/share/map.map"
#define MARKFILE "/vol/biorob/share/map.marks"


//
// ++itschere20030821: switch of neobotix protocol and other feature versions,
// currently supported values: BIELEFELD, MUNICH and HAMBURG. the idea of
// course is that all platforms should speak the same protocol and support the
// same features, but for now this is not true.
//
// both (BIELEFELD and MUNICH) run off the same header files, Cmd_IOBoard.h and
// Cmd_MotCtrl.h in the directory `oli', which is officially wrong for
// BIELEFELD but works. the one difference is that GETDIGIN/SETDIGOUT for
// Cmd_IOBoard.h (cmd_c167.h) are mixed up, but since we don't use them we
// don't care.
//

#define BIELEFELD 0x0
#define MUNICH    0x1
#define HAMBURG   0x2

#define MOBILEVERSION HAMBURG


//
// ++itschere20030821: `#define FAKE_GYRO' if the gyro is not to be used. in
// this case the motor feeder will use the current rotational velocity as from
// the localization as faked gyro value (because the localization has no
// routine that does not take a gyro value).
//

#define FAKE_GYRO
//#undef FAKE_GYRO


//
// ++itschere20030828: `#define DISABLE_VOLTAGE_GRAPH' if the battery voltage
// graph is never to be displayed, e.g. because the voltage readings are broken
// (like in MUNICH).
//

#undef DISABLE_VOLTAGE_GRAPH


//
// ++itschere20031118: `#define LASER_LOGFILE' to get a log file of marks as
// seen by the localization (laser) in every cycle. this may be usefull for
// debugging purposes, when you want to answer questions like: "how many marks
// does it see on average in particular areas in the map?'
//

#undef LASER_LOGFILE
//#define LASER_LOGFILE "marks.out"


//

#endif
