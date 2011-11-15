//
//
//

#include <unistd.h>

#include "thread/tracelog.h"

#include "./genBase2.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void zigZag (CGENBASE2 &genBase)
{
  genBase.MotionRotate (0.0 * M_PI / 180.0);

  while (42)
    {
      genBase.MotionRotate ( 90.0 * M_PI / 180.0);
      genBase.MotionRotate (-90.0 * M_PI / 180.0);
    }
}


static void randomWalk (CGENBASE2 &genBase)
{
  while (42)
    {
      genBase.MotionRotate ((random () % 360) * M_PI / 180.0);
      sleep (1);
    }
}


static void stepWalk (CGENBASE2 &genBase)
{
  double sign = 1.0, angle = 0.0;

  for (int i=1; i<360; i*=2)
    {
      angle += i * sign;

      genBase.MotionRotate (angle * M_PI / 180.0);

      sign = -sign;
    }
}


static void nothing (void)
{
  while (42)
    {
      sleep (1);
    }
}


static void hinUndHer (CGENBASE2 &genBase)
{
  //genBase.MotionRotate (0.0 * M_PI / 180.0);

  while (42)
    {
#if 1
      genBase.MotionTranslate (2.0, 9.0);
      genBase.MotionTranslate (2.0, 6.0);
#else
      genBase.MotionTranslate (-1.0, -6.0);
      genBase.MotionTranslate (+1.0, -6.0);
      genBase.MotionTranslate (-1.0, -6.0);
      genBase.MotionTranslate (-1.0, -1.0);
      genBase.MotionTranslate ( -6.0, -1.0);
      genBase.MotionTranslate ( -7.0, -0.62);
      genBase.MotionTranslate ( -8.0, -0.8);
      genBase.MotionTranslate (-23.5, -0.8);
      genBase.MotionTranslate ( -8.0, -0.8);
      genBase.MotionTranslate ( -7.0, -0.62);
      genBase.MotionTranslate ( -6.0, -1.0);
      genBase.MotionTranslate (-1.0, -1.0);
#endif
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
#if 1
  _globGtlEnable = 1;
#endif

  srandom (time (0x0));

  CGENBASE2 genBase;

  //

  //zigZag (genBase);
  //randomWalk (genBase);
  //stepWalk (genBase);
  nothing ();
  //hinUndHer (genBase);

  //

  printf ("# done, exiting...\n");

  return 0;
}
