//
//
//

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define CMD_REPLY                    0x80000000

#define CMD_PING                     0x00000000
#define CMD_CHECKPROTOCOL            0x00000001
#define CMD_DEBUGGING                0x00000002

#define CMD_GETMODE                  0x00010000
#define CMD_SETMODE                  0x00010001

#define CMD_GETPOSITION              0x00020000
#define CMD_SETPOSITION              0x00020001
#define CMD_GETPOSITIONANDVARIANCE   0x00020002   // ++itschere20030806
#define CMD_GETPOSITIONODO           0x00020003   // ++itschere20031209

#define CMD_GETALLMARKS              0x00020014
#define CMD_GETALLMARKSINITIAL       0x00020017
#define CMD_GETALLMARKSWITHVARIANCES 0x00020018   // ++itschere20031117

#define CMD_GETSCALE                 0x00030010
#define CMD_SETSCALE                 0x00030011
#define CMD_MODIFYSCALE              0x00030012

#define CMD_STOP                     0x00030100
#define CMD_TRANSLATE                0x00030200
#define CMD_FORWARD                  0x00030201
#define CMD_ROTATEANGLE              0x00030300
#define CMD_ROTATEPOINT              0x00030301
#define CMD_TURNANGLE                0x00030302   // ++westhoff20060421
#define CMD_MOVE                     0x00030400
#define CMD_MOVEPOINT                0x00030401

#define CMD_ISCOMPLETED              0x00030500
#define CMD_WAITFORCOMPLETED         0x00030501

#define CMD_STALLED                  0x00030600   // ++westhoff20050506

#define CMD_APPLYBRAKES              0x00030700   // ++westhoff20050803
#define CMD_RELEASEBRAKES            0x00030701   // ++westhoff20050803
#define CMD_AREBRAKESRELEASED        0x00030702   // ++westhoff20050803

#define CMD_GETNUMSCANNERS           0x00040000   //
#define CMD_GETSCANNERPOSITION       0x00040010   //
#define CMD_GETSCANSCANNER           0x00040020   // ++westhoff20050808
#define CMD_GETSCANRADIALSCANNER     0x00040021   // ++westhoff20050808
#define CMD_GETSCANPLATFORM          0x00040030   // ++westhoff20050808
#define CMD_GETSCANRADIALPLATFORM    0x00040031   // ++westhoff20050808
#define CMD_GETSCANWORLD             0x00040040   // ++westhoff20050808

#define CMD_GETALLLINES              0x00050004

#define CMD_STARTODOLOGGING          0x00060000   // ++westhoff20050202
#define CMD_STOPODOLOGGING           0x00060001   // ++westhoff20050202

#define CMD_GETBATTERYVOLTAGE        0x00070000   // ++westhoff20050330
#define CMD_GETDRIVETEMPERATURES     0x00070010   // ++westhoff20050330

#endif
