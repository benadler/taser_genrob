#ifndef PROTOCOL_MOBILED_H
#define PROTOCOL_MOBILED_H

// Every reply-packet has this bitmask set
#define CMD_REPLY                    0x80000000

/// PROTOCOL

// Just send a ping-reply
#define CMD_PING                     0x00000000

// Request a packet containing all OPcodes
#define CMD_CHECKPROTOCOL            0x00000001

/// ROBOT STATUS

// Request the robotstatus. See enum RobotStatus
#define CMD_GETROBOTSTATUS           0x00010000

// Requests battery voltage
#define CMD_GETBATTERYVOLTAGE        0x00010001

// Requests drive temperature
#define CMD_GETDRIVETEMPERATURES     0x00010002

// Request waypoints of current path
#define CMD_GETCURRENTPATH           0x00010003

/// LOCALIZATION

// Request current pose
#define CMD_GETPOSE                  0x00020000

// Set current pose
#define CMD_SETPOSE                  0x00020001

// Tell correspondence to update its marks
#define CMD_UPDATEMARKS              0x00020002

/// MOTION

// Stop the robot, abort any motion
#define CMD_STOP                     0x00030000

// Translates by %f meters
#define CMD_MOVETRANSLATE            0x00030001

// Rotates by %f degrees, positive is counterclockwise
#define CMD_MOVEROTATE               0x00030002

// Moves to a point using a spline from pathplanner
#define CMD_MOVESPLINE               0x00030003

// Moves along given points using a spline
#define CMD_MOVESPLINEALONG          0x00030004

// Waits for the robot to complete its motion
// before sending a reply packet.
#define CMD_WAITFORCOMPLETED         0x00030005

/// LASERSCANNERS

// Requests the number of scanners
#define CMD_GETNUMSCANNERS           0x00040000
// Requests the pose of given scanner
#define CMD_GETSCANNERPOSE       0x00040001
// Requests the radial data from given scanner
#define CMD_GETSCANRADIALSCANNER     0x00040002

/// COLLISIONAVOIDANCE

// Requests the current status of CollisionAvoidance
#define CMD_GETCOLLISIONAVOIDANCE    0x00050000
// Sets the current status of CollisionAvoidance
#define CMD_SETCOLLISIONAVOIDANCE    0x00050001

/// CONFIGURATION

// Request current speedfactor
#define CMD_GETSPEEDFACTOR           0x00060000
// Set speedfactor
#define CMD_SETSPEEDFACTOR           0x00060001

// Request current speedlimit
#define CMD_GETSPEEDLIMIT            0x00060002
// Set speedlimit
#define CMD_SETSPEEDLIMIT            0x00060003

// Request current abortMotionBrakeDeceleration
#define CMD_GETABORTMOTIONDECELERATION     0x00060004
// Set abortMotionBrakeDeceleration
#define CMD_SETABORTMOTIONDECELERATION     0x00060005

// Request current smallestHighSpeedCurveRadius
#define CMD_GETSMALLESTHIGHSPEEDCURVERADIUS 0x00060006
// Set smallestHighSpeedCurveRadius
#define CMD_SETSMALLESTHIGHSPEEDCURVERADIUS 0x00060007

// Request current robotRadius
#define CMD_GETROBOTRADIUS           0x00060008
// Set robotRadius
#define CMD_SETROBOTRADIUS           0x00060009

// Request current pathPlannerPollInterval
#define CMD_GETPATHPLANNERPOLLINTERVAL 0x00060010
// Set pathPlannerPollInterval
#define CMD_SETPATHPLANNERPOLLINTERVAL 0x00060011


#endif
