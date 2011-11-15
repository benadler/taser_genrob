//
//
//

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "mobile/canPlatform.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline double diffTimevalSeconds (const struct timeval &t1,
					 const struct timeval &t2)
{
  return (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;
}


static inline double diffTimevalMucroSeconds (const struct timeval &t1,
					      const struct timeval &t2)
{
  return 1000000.0 * (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CMYCANPLATFORM : public CCANPLATFORM
{
public:

  // need these public

  void DisableBrakeLeft (const bool arg)
  {
    CCANPLATFORM::DisableBrakeLeft (arg);
  }
  void DisableBrakeRight (const bool arg)
  {
    CCANPLATFORM::DisableBrakeRight (arg);
  }
};


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

  CMYCANPLATFORM canPlatform;

  //canPlatform.StartMotors ();

  while (42)
    {
      canPlatform.DisableBrakeLeft  (true);
      usleep (200000);
      canPlatform.DisableBrakeRight (true);
      usleep (200000);

      //canPlatform.SetWorldVelocity (1, 1);
      usleep (800000);

      canPlatform.DisableBrakeLeft  (false);
      usleep (200000);
      canPlatform.DisableBrakeRight (false);
      usleep (200000);

      usleep (800000);
    }

  return 0;
}
