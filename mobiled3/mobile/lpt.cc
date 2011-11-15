//
//
//

#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>

#if defined (linux)   // better be safe

#include <linux/ioctl.h>
#include <linux/ppdev.h>

#include "lpt.h"

#include "thread/tracelog.h"



///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CLPT::CLPT (const char *devName)
{
  GTL ("CLPT::CLPT()");

  if ((_fd = open ("/dev/parport0", O_RDWR)) < 0)
    {
      perror ("open()");
      throw -1;
    }

  if (ioctl (_fd, PPCLAIM, 0x0))
    {
      perror ("ioctl(PPCLAIM)");
      throw -1;
    }

#if 0
  int arg = 1;
  if (ioctl (_fd, PPEXCL, &arg))
    {
      perror ("ioctl(PPEXCL)");
      throw -1;
    }
#endif

  int direction = 0;

  if (ioctl (_fd, PPDATADIR, &direction))
    {
      perror ("ioctl(PPEXCL)");
      throw -1;
    }

#if 0
  direction = 1;
  if (ioctl (_fd, PPDATADIR, &direction))
    {
      perror ("ioctl(PPEXCL)");
      throw -1;
    }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CLPT::~CLPT (void)
{
  GTL ("CLPT::~CLPT()");

  close (_fd);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CLPT::Out (unsigned char val)
{
  GTL ("CLPT::Out()");

  if (ioctl (_fd, PPWDATA, &val))
    {
      perror ("ioctl(PPWDATA)");
      abort ();
    }

  sleep (1);
}

#endif
