//
//
//

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "./periodicTimer.h"

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"

#if defined (linux)

#include <linux/rtc.h>


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CPERIODICTIMER::CPERIODICTIMER (const bool realtime)
{
  GTL ("CPERIODICTIMER::CPERIODICTIMER()");

  _realtime = realtime;

  if ((_fd = open ("/dev/rtc", O_RDONLY)) < 0)
    {
      perror ("open()");
      GTLFATAL (("failed to open /dev/rtc\n"));
      throw -1;
    }

  unsigned long rate = 32; // 1/32s = 31.25ms
  //unsigned long rate = 64;

  if (ioctl (_fd, RTC_IRQP_SET, rate))
    {
      perror ("ioctl()");
      GTLFATAL (("failed to set interrupt rate\n"));
      throw -1;
    }

  if (ioctl (_fd, RTC_PIE_ON, 0x0))
    {
      perror ("ioctl()");
      GTLFATAL (("failed to enable interrupts\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CPERIODICTIMER::~CPERIODICTIMER (void)
{
  GTL ("CPERIODICTIMER::~CPERIODICTIMER()");

  ioctl (_fd, RTC_PIE_OFF, 0x0);

  close (_fd);

  Stop ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CPERIODICTIMER::Fxn (void)
{
  GTL ("CPERIODICTIMER::Fxn");

  GTLPRINT (("pid=%i\n", getpid ()));

  ConfirmRunning ();

  //

  struct timeval last;

  gettimeofday (&last, 0x0);

  //

  if (_realtime)
    {
      ScheduleRealtime (5);
    }

  nice (-5);   // for `top' statistics only, not used in rt-scheduling

  //

  while (!_stopRequested)
    {
      fd_set rfd;

      FD_ZERO (&rfd);
      FD_SET (_fd, &rfd);

      struct timeval tv = {0, 100000};   // 0.1s timeout

      int ret = select (_fd+1, &rfd, 0x0, 0x0, &tv);

      if (ret < 0)
	{
	  GTLFATAL (("select() failed\n"));
	  abort ();
	}

      if (!ret)
	{
	  GTLFATAL (("select() timed out\n"));
	  continue;
	}

      if (!FD_ISSET (_fd, &rfd))
	{
	  GTLFATAL (("weirdness #08/15\n"));
	  continue;
	}

      unsigned long data;

      if (read (_fd, &data, sizeof (data)) != sizeof (data))
	{
	  GTLFATAL (("read() failed\n"));
	  continue;
	}

      //

      struct timeval now;

      gettimeofday (&now, 0x0);

      int us = now.tv_sec > last.tv_sec ?
	1000000 + now.tv_usec - last.tv_usec :   // `>'
	now.tv_usec - last.tv_usec;   // `=' (`<' not possible)

      last = now;

      int diff = us - 31250;

      if ((abs (diff) > 2000) && _realtime)
	{
	  GTLFATAL (("interval=%i (diff=%i), expect problems\n", us, diff));
	}

      //

      Event ();
    }
}

#endif
