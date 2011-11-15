//
// voltageDisplayThread.cc
//

#include <stdlib.h>
#include <unistd.h>

#include "defines.h"

#if MOBILEVERSION == HAMBURG
// ++westhoff20040205: need these for the ploting of the voltages
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#endif

#ifdef WITH_RCCL
#include <rccl.h>
#include <fts3.h>
#endif

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"

#include "./voltageDisplayThread.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVOLTAGEDISPLAYTHREAD::~CVOLTAGEDISPLAYTHREAD (void)
{
  GTL ("CVOLTAGEDISPLAYTHREAD::~CVOLTAGEDISPLAYTHREAD()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#ifdef WITH_RCCL

static void stepLog (LOG *l, const double v)
{
  //

  if (++l->used > l->size)
    {
      l->used = l->size;
    }

  for (int i=0; i<4; i++)
    {
      memmove (l->item[i].data + 1,
	       l->item[i].data,
	       (l->size - 1) * sizeof (float));
    }

  static time_t tStart = 0;

  if (!tStart)
    {
      tStart = time (0);
    }

  l->item[0].data[0] = (time (0) - tStart) / 3600.0;
  l->item[1].data[0] = v;
  l->item[2].data[0] = 54.0;
  l->item[3].data[0] = 42.0;

  //

  static VIEW *view = 0x0;

  if (!view)
    {
      if (!(view = viewCreate ("t=Zeit[h],u,u_max,u_min", 1, VIEW_GRID)))
	{
	  abort ();
	}

      viewSetTitle (view, "Batteriespannung");
      viewSetLabel (view, "Spannung [Volt]");
    }

  if (l->used > 1)
    {
      viewLog (view, l);
    }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

typedef double buffertype_t;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static buffertype_t medianInsert (buffertype_t *buf,
				  const int bufSize,
				  const int ommitSize,
				  const buffertype_t value)
{
  // insert at head

  memmove (buf+1, buf, (bufSize-1) * sizeof (buffertype_t));

  buf[0] = value;

  // sort into temporary buffer

  buffertype_t *tmpBuf = (buffertype_t *)alloca (bufSize * sizeof (buffertype_t));

  memcpy (tmpBuf, buf, bufSize * sizeof (buffertype_t));

  for (int i=0; i<bufSize-1; i++)
    {
      int smallestIdx = i;
      buffertype_t smallestValue = tmpBuf[i];

      for (int j=i+1; j<bufSize; j++)
	{
	  if (tmpBuf[j] < smallestValue)
	    {
	      smallestValue = tmpBuf[j];
	      smallestIdx = j;
	    }
	}

      if (smallestIdx != i)
	{
	  const buffertype_t tmp = tmpBuf[i];
	  tmpBuf[i] = tmpBuf[smallestIdx];
	  tmpBuf[smallestIdx] = tmp;
	}
    }

  // compute median

  buffertype_t sum = 0.0;
  int cnt = bufSize - 2 * ommitSize;

  if (cnt > 0)
    {
      for (int i=ommitSize; i<bufSize-ommitSize; i++)
	{
	  sum += tmpBuf[i];
	}

      return sum / (buffertype_t)cnt;
    }

  // error

  fprintf (stderr, "median parameter error\n");
  abort ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CVOLTAGEDISPLAYTHREAD::Fxn (void)
{
  GTL ("CVOLTAGEDISPLAYTHREAD::Fxn()");

  nice (19);

#if MOBILEVERSION != HAMBURG

#ifdef WITH_RCCL

  LOG *l = logCreate (10000, 4);

  float dummy;
  logDefineItem (l, 0, "t", &dummy, 0x0);
  logDefineItem (l, 1, "u", &dummy, 0x0);
  logDefineItem (l, 2, "u_max", &dummy, 0x0);
  logDefineItem (l, 3, "u_min", &dummy, 0x0);

#endif  // WITH_RCCL

#endif  // HAMBURG

  //

#define BUFSIZE 15

  buffertype_t cntBuf[BUFSIZE];

  for (int i=0; i<BUFSIZE; i++)
    {
      double voltage;

      do
	{
	  sleep (1);

	  voltage = _motorFeeder->GetBatteryVoltage ();

	  // GTLPRINT (("Voltage: %2.2f\n",voltage));
	}
      while (voltage <= 0.0);

#define OMMITSIZE 4

      medianInsert (cntBuf, BUFSIZE, OMMITSIZE, voltage);
    }

#if MOBILEVERSION == HAMBURG

  // ++westhoff20040205: Create plot for voltage without RCCL.

  std::string filename ("voltage.log");

  std::string plotCmd = "";

  FILE *plot;
  plot = popen ("gnuplot", "w");   // ++itschere20040325: wow, didn't know this exists! :-)

  // set title

  plotCmd = "set title 'battery voltage'\n";

  fprintf (plot, plotCmd.c_str());
  fflush (plot);

  // set legend

  plotCmd = "set key left bottom\n";

  fprintf (plot, plotCmd.c_str());
  fflush (plot);

  // set y label

  plotCmd = "set ylabel '[volt]'\n";

  fprintf (plot, plotCmd.c_str());
  fflush (plot);

  // set x label

  plotCmd = "set xlabel '[time]'\n";

  fprintf (plot, plotCmd.c_str());
  fflush (plot);

  // Set format of time in datafile (YYYY/mm/dd-HH:MM:SS).
  // Format is self explaining and human readable. Two percent
  // signs are used in string because the following fprintf-
  // command sees % as a special character followed by a format
  // discription and a variable in the ...-list and %% as a
  // single percent character.

  plotCmd = "set timefmt \"%%Y/%%m/%%d-%%H:%%M:%%S\"\n";

  fprintf (plot, plotCmd.c_str());
  fflush (plot);

  // set x axis to time data

  plotCmd = "set xdata time\n";

  fprintf (plot, plotCmd.c_str());
  fflush (plot);

  // set format of time in plot (HH:MM:SS\nYYYY/mm/dd)

  plotCmd = "set format x \"%%T\\n%%Y/%%m/%%d\"\n";

  fprintf (plot, plotCmd.c_str());
  fflush (plot);

  // set grid

  plotCmd = "set grid\n";

  fprintf (plot, plotCmd.c_str());
  fflush (plot);

  // open file for data

  std::ofstream datafile (filename.c_str(),
			  std::ios::out | std::ios::trunc);

#endif

  //

  while (!_stopRequested)
    {

#if MOBILEVERSION != HAMBURG

      sleep (5);

#else

      sleep (1);

#endif

      const double voltage = _motorFeeder->GetBatteryVoltage ();

      if (voltage < 0.0)
	{
	  continue;
	}

#if MOBILEVERSION != HAMBURG

// ++westhoff20040205: If RCCL code is used mobiled stops with
//                    "No child processes" error message for robot HAMBURG.

#ifdef WITH_RCCL

      const double avg = medianInsert (cntBuf, BUFSIZE, OMMITSIZE, voltage);

      static unsigned int run = 0;

      stepLog (l, avg);

      const bool save = !(++run % (12*15));   // 12 ticks/min

      if (save)
	{
	  time_t t = time (0);
	  struct tm tm;
	  localtime_r (&t, &tm);
	  char buf[64];
	  sprintf (buf,
		   "voltage-%04i%02i%02i%02i%02i.log",
		   tm.tm_year + 1900,
		   tm.tm_mon + 1,
		   tm.tm_mday,
		   tm.tm_hour,
		   tm.tm_min);
	  logSave (l, buf);
	  GTLFATAL (("%s saved\n", buf));
	}

#else   // WITH_RCCL

      GTLPRINT (("voltage = %f\n", voltage));

#endif  // WITH_RCCL

#else   // HAMBURG

      // ++westhoff20040205: Add new average voltage value to plot. Time is in
      //                     human readable and self explaining format.

      const double avg = medianInsert (cntBuf, BUFSIZE, OMMITSIZE, voltage);

      static unsigned int run = 0;

      ++run;

      // plot average every 15 seconds (4/minute)

      const bool save = !(run % 1);

      if (save)
      {
        time_t now = time (0);
        struct tm tm;
        localtime_r (&now, &tm);

        datafile << std::setfill('0')
                 << std::setw(4)
                 << tm.tm_year + 1900
                 << "/"
                 << std::setw(2)
                 << tm.tm_mon + 1
                 << "/"
                 << std::setw(2)
                 << tm.tm_mday
                 << "-"
                 << std::setw(2)
                 << tm.tm_hour
                 << ":"
                 << std::setw(2)
                 << tm.tm_min
                 << ":"
                 << std::setw(2)
                 << tm.tm_sec
	         << " "
	         << floor ((avg * 100.0) + 0.5) / 100.0     // round  to two decimal places
                 << " "
                 << floor ((voltage * 100.0) + 0.5) / 100.0 // round to two decimal places
	         << std::endl;                              // std::endl calls std::flush!!!

        // plot voltage with gnuplot

        plotCmd = "plot '"
                  + filename
                  + "' u 1:3  t 'voltage' w l lt 2, '"
                  + filename
                  + "' u 1:2 t 'voltage (15 sec mean)' w l lt 1"
                  + "\n";

        fprintf (plot, plotCmd.c_str());
        fflush (plot);

        // reset run

        run = 0;
      }

#endif  // HAMBURG

    }
}
