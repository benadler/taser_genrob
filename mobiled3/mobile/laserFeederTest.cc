//
//
//

#include <math.h>
#include <unistd.h>

#include "laserFeeder.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  _globGtlEnable = 1;

  double dummy1, dummy2;

  CLASERFEEDER front ("/dev/ttyM0", true, 0.3, 0.0, 0.0, &dummy1);
  CLASERFEEDER rear ("/dev/ttyM1", false, -0.3, 0.0, M_PI, &dummy2);

  while (42)
    {
      select (0, 0x0, 0x0, 0x0, 0x0);
    }

  return 0;
}
