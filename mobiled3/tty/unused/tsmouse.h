#ifndef __TSMOUSE_H__
#define __TSMOUSE_H__

#include <iostream.h>
#include <string.h>
//#include <termios.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//#include <pthread.h>

#include "transtype.H"

#define mglStartup      "\rz\rz\r"      /* Startup  (Undoc) */
#define mglModeOff	"m0\r"		/* Translation and Rotation Mode OFF */
#define mglModeLargest	"m7\r"		/* Only send the largest */
#define mglModeOn	"m3\r"		/* Translation and Rotation Mode ON */
#define mglDataRateFast	"p?0\r"		/* 60 msec Date Rate */
#define mglDataRateSlow	"pNM\r"		/* 320 msec Date Rate */
#define mglAskDataRate	"pQ\r"		/* Send akt. Date Rate */
#define mglQuality	"qA0\r"		/* Linear */
#define mglNullRadius	"n0\r"		/* Null Radius at 8 */
#define mglBeeper	"b9\r"		/* Short Beep */
#define mglZero		"z\r"		/* Zeroing */
#define mglDataQuestion	"dQ\r"		/* Data Request (Polling mode) */
#define mglVersionText 	"vQ\r"          /* Version Request */

#define NOKEY 0x0
#define KEY1  0x1
#define KEY2  0x2
#define KEY3  0x4
#define KEY4  0x8
#define KEY5  0x10
#define KEY6  0x20
#define KEY7  0x40
#define KEY8  0x80
#define KEYX  0x100


#define ROTX(GAMA)  TRMatrix(1,0,0,0,int(cos(GAMA)),int(-sin(GAMA)),0,int(sin(GAMA)),int(cos(GAMA)))

#define ROTY(GAMA)  TRMatrix(int(cos(GAMA)),0,int(sin(GAMA)),0,1,0,int(-sin(GAMA)),0,int(cos(GAMA)))

#define ROTZ(GAMA)  TRMatrix(int(cos(GAMA)),int(-sin(GAMA)),0,int(sin(GAMA)),int(cos(GAMA)),0,0,0,1)

typedef TMousePos *PMousePos;

/* ======================================================================= */
/* ======================================================================= */
class TRMatrix
{

private:
  long rm[3][3];

public:

  TRMatrix(long v11, long v12, long v13,
           long v21, long v22, long v23,
           long v31, long v32, long v33);

inline void Calc(long x, long y, long z, long *rx, long *ry, long *rz);
};

void TRMatrix::Calc(long x, long y, long z, long *rx, long *ry, long *rz)
{

#if 0  /* Sat Oct 25 16:26:44 1997 */
  cout << rm[0][0] << " " << rm[0][1] << " " << rm[0][2] << endl;
  cout << rm[1][0] << " " << rm[1][1] << " " << rm[1][2] << endl;
  cout << rm[2][0] << " " << rm[2][1] << " " << rm[2][2] << endl;
#endif

  *rx = rm[0][0] * x + rm[0][1] * y + rm[0][2] * z;
  *ry = rm[1][0] * x + rm[1][1] * y + rm[1][2] * z;
  *rz = rm[2][0] * x + rm[2][1] * y + rm[2][2] * z;
}

extern TRMatrix ROTX_90;
extern TRMatrix ROTY_90;
extern TRMatrix ROTZ_90;
extern TRMatrix ROTX_m90;
extern TRMatrix ROTY_m90;
extern TRMatrix ROTZ_m90;

/* ======================================================================= */
/* ======================================================================= */

#include "tty/ttys.h"

class TSMouse : public CTTYS
{
private:
  static unsigned char NibbleCodes[];
  static unsigned char Table6Bits[64];

  int BeepOn;
  int Rotate;

  TRMatrix rm;


  unsigned char mkNibble(unsigned char CH){return NibbleCodes[(CH)&0x0F];}
  unsigned int  Low4(unsigned char Value, int *Err);
  long int  Low6(long  ch,    int *Err);

public:
  TSMouse (const char *dev);
  virtual ~TSMouse (void);
  
  void          Show              (unsigned char *buf, int size);
  long          DecodeData        (PMousePos Pos, unsigned char *buf);
  unsigned long DecodeKey         (unsigned char *buf);
  int           ReadCommand       (unsigned char *buf);

  int WriteCommand (const char *buf)
  {
    return  Write (buf, strlen (buf));
  }
  int WriteCommand (const unsigned char *buf)
  {
    return WriteCommand ((const char *)buf);
  }

  void          BeepOnKey         (int flag){BeepOn = flag;}
  void          RotateOn          (int flag){Rotate = flag;}
  int           SetNullRadius     (unsigned char radius);
  int           SetDataRate       (unsigned char min,   unsigned char max);
  int           SetSensitivity    (unsigned char trans, unsigned char rot);
  void          SetRotationMatrix (TRMatrix Rot){rm = Rot;}
  virtual void  ErrorState        (void){cout << "Error\n"; abort();}
};

typedef TSMouse *PSMouse;

/* ======================================================================= */
/* ======================================================================= */

#endif
