//
//
//

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <pcan.h>

#include "mobile/canDongle.h"

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#define CAN_BAUD_1M     0x0014  //   1 MBit/s
#define CAN_BAUD_500K   0x001C  // 500 kBit/s
#define CAN_BAUD_250K   0x011C  // 250 kBit/s
#define CAN_BAUD_125K   0x031C  // 125 kBit/s
#define CAN_BAUD_100K   0x432F  // 100 kBit/s
#define CAN_BAUD_50K    0x472F  //  50 kBit/s
#define CAN_BAUD_20K    0x532F  //  20 kBit/s
#define CAN_BAUD_10K    0x672F  //  10 kBit/s
#define CAN_BAUD_5K     0x7F7F  //   5 kBit/s


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CCANDONGLE::CCANDONGLE (void)
{
  GTL ("CCANDONGLE::CCANDONGLE()");

  // open

  if ((_fd = open ("/dev/pcan24", O_RDWR | O_SYNC)) < 0)
    {
      GTLFATAL (("open(\"/dev/pcan24\") failed\n"));
      abort ();
    }

  // initialize

  TPCANInit init;

  init.wBTR0BTR1 = CAN_BAUD_500K;       // merged BTR0 and BTR1 register of the SJA100
  init.ucCANMsgType = MSGTYPE_STANDARD; // 11 or 29 bits
  init.ucListenOnly = 0;                // listen only mode when != 0

  if ((ioctl (_fd, PCAN_INIT, &init)) < 0)
    {
      GTLFATAL (("ioctl(PCAN_INIT) failed\n"));
      abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CCANDONGLE::~CCANDONGLE (void)
{
  GTL ("CCANDONGLE::~CCANDONGLE()");

  close (_fd);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if 1

static void error (int count)
{
  FILE *fp = fopen ("canbus.err", "a");
  if (fp)
    {
      time_t t = time (0);
      struct tm tm;
      localtime_r (&t, &tm);
      fprintf (fp, "%04i%02i%02i%02i%02i : ",
	       tm.tm_year + 1900,
	       tm.tm_mon + 1,
	       tm.tm_mday,
	       tm.tm_hour,
	       tm.tm_min);
      fprintf (fp, "%i\n", count);
      fclose (fp);
    }
}

#endif


int CCANDONGLE::Send (const unsigned short id,
		      const unsigned char data1,
		      const unsigned char data2,
		      const unsigned char data3,
		      const unsigned char data4,
		      const unsigned char data5,
		      const unsigned char data6,
		      const unsigned char data7,
		      const unsigned char data8)
{
  GTL ("CCANDONGLE::Send()");

  const TPCANMsg msg = {id,
			MSGTYPE_STANDARD,
			8,
			{data1, data2, data3, data4, data5, data6, data7, data8}};

  GTLPRINT (("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x -> 0x%04x\n",
	     msg.DATA[0],
	     msg.DATA[1],
	     msg.DATA[2],
	     msg.DATA[3],
	     msg.DATA[4],
	     msg.DATA[5],
	     msg.DATA[6],
	     msg.DATA[7],
	     msg.ID));

#if 1

  int count = 1;

  while (ioctl (_fd, PCAN_WRITE_MSG, &msg) < 0)
    {
      GTLFATAL (("ioctl(PCAN_WRITE_MSG) failed, retrying...\n"));
      sched_yield ();
      count++;
    }

  if (count != 1)
    {
      error (count);
    }

#else

  if (ioctl (_fd, PCAN_WRITE_MSG, &msg) < 0)
    {
      GTLFATAL (("ioctl(PCAN_WRITE_MSG) failed\n"));
      abort ();
    }

#endif

  return 8;
}


///////////////////////////////////////////////////////////////////////////////
//
// Receive()
//
// returns the ID of the controller which sent the packet
//
///////////////////////////////////////////////////////////////////////////////

int CCANDONGLE::Receive (const int maxSize,
			 unsigned char *data)
{
  GTL ("CCANDONGLE::Receive()");

  TPCANRdMsg msg;

  if (ioctl (_fd, PCAN_READ_MSG, &msg) < 0)
    {
      GTLFATAL (("ioctl(PCAN_READ_MSG) failed\n"));
      abort ();
    }

  memcpy (data, msg.Msg.DATA, 8);

  unsigned int ident = data[6];
  unsigned int error = data[7] & 0x03;
  unsigned int cmd = data[7] >> 2;

  GTLPRINT (("0x%04x -> %i : 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
	     msg.Msg.ID,
	     msg.Msg.LEN,
	     data[0], data[1], data[2], data[3],
	     data[4], data[5], data[6], data[7]));

  GTLPRINT (("ident = 0x%02x, cmd = 0x%02x, error = 0x%02x\n",
	     ident,
	     cmd,
	     error));

  return msg.Msg.ID;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CCANDONGLE::ReceiveTimeout (const int maxSize,
				unsigned char *data,
				const unsigned int milliSeconds,
				const bool ommitWarnings)
{
  GTL ("CCANDONGLE::ReceiveTimeout()");

  GTLPRINT (("awaiting 1 paket in %i milliseconds\n", milliSeconds));

  //

  fd_set rfd;

  FD_ZERO (&rfd);
  FD_SET (_fd, &rfd);

  struct timeval timeout = {milliSeconds / 1000, (milliSeconds % 1000) * 1000};

  int ret = select (_fd + 1, &rfd, 0x0, 0x0, &timeout);

  if (ret < 1)
    {
      if (!ommitWarnings)
	{
	  // ++itschere20030826: it possible to suppress this output.  if we
	  // really didn't expect anything there's no point in printing a
	  // timeout if nothing arrives. in this case it would suffice to print
	  // if something does arrive.

	  GTLFATAL (("timeout\n"));
	}
      return -1;
    }

  if (!FD_ISSET (_fd, &rfd))
    {
      GTLFATAL (("select() weirdness\n"));
      abort ();
    }

  //

  TPCANRdMsg msg;

  if (ioctl (_fd, PCAN_READ_MSG, &msg) < 0)
    {
      GTLFATAL (("ioctl(PCAN_READ_MSG) failed\n"));
      abort ();
    }

  memcpy (data, msg.Msg.DATA, 8);

  unsigned int ident = data[6];
  unsigned int error = data[7] & 0x03;
  unsigned int cmd = data[7] >> 2;

  GTLPRINT (("0x%04x -> %i : 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
	     msg.Msg.ID,
	     msg.Msg.LEN,
	     data[0], data[1], data[2], data[3],
	     data[4], data[5], data[6], data[7]));

  GTLPRINT (("ident = 0x%02x, cmd = 0x%02x, error = 0x%02x\n",
	     ident,
	     cmd,
	     error));

  return msg.Msg.ID;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CCANDONGLE::ReceiveMultipleTimeout (const int numReplies,
					struct reply *replies,
					const unsigned int milliSeconds)
{
  GTL ("CCANDONGLE::ReceiveMultipleTimeout()");
  GTLPRINT (("awaiting %i pakets in %i milliseconds\n",
	     numReplies, milliSeconds));

  if (milliSeconds > 999)
    {
      GTLFATAL (("timeout may not be > 999ms, expect problems!\n"));
    }

  for (int i=0; i<numReplies; i++)
    {
      replies[i].gotcha = false;
    }

  bool hasFatal = false;
  bool hasError = false;

#if 1

  int rcvIdx = 0;
  unsigned short rcvId[8];
  unsigned char rcvCmd[8];
  unsigned char rcvErr[8];

#endif

  //
  //
  //

  struct timeval start;
  gettimeofday (&start, 0x0);

  while (42)
    {
      bool done = true;

      for (int i=0; i<numReplies; i++)
	{
	  done &= replies[i].gotcha;
	}

      if (done)
	{
	  break;
	}

      //

      struct timeval now;
      gettimeofday (&now, 0x0);

      int elapsed = now.tv_usec - start.tv_usec;

      if (elapsed < 0) elapsed += 1000000;

      int timeout = milliSeconds - elapsed / 1000;

      if (timeout < 1) timeout = 1;

      //

      unsigned char buf[8];

      short id = ReceiveTimeout (8, buf, timeout);

      if (id < 0)
	{
	  GTLFATAL (("timeout\n"));
	  hasError = true;
	  hasFatal = true;
	  break;
	}

      int error = buf[7] & 0x03;
      unsigned char cmd = buf[7] >> 2;

#if 1

      if (rcvIdx < 8)
	{
	  rcvId[rcvIdx] = id;
	  rcvCmd[rcvIdx] = cmd;
	  rcvErr[rcvIdx] = error;
	  rcvIdx++;
	}

#endif

      bool gotcha = false;

      for (int i=0; i<numReplies; i++)
	{
	  if ((id == replies[i].id) &&
	      (cmd == replies[i].cmd) &&
	      (!error) &&
	      (!replies[i].gotcha))
	    {
	      GTLPRINT (("command 0x%02x from 0x%04x with error 0x%02x received\n",
			 cmd, id, error));

	      replies[i].gotcha = gotcha = true;
	      memcpy (replies[i].buf, buf, 8 * sizeof (char));
	      break;
	    }
	}

      if (gotcha)
	{
	  continue;
	}

      GTLFATAL (("(bogus?) command 0x%02x from 0x%04x with error 0x%02x received\n",
		 cmd, id, error));
    }

  //
  //
  //

#if 1

  if ((rcvIdx != numReplies) || hasError)
    {
      FILE *fp = fopen ("canbus.out", "a");
      if (fp)
	{
	  time_t t = time (0);
	  struct tm tm;
	  localtime_r (&t, &tm);
	  fprintf (fp, "%04i%02i%02i%02i%02i : ",
		   tm.tm_year + 1900,
		   tm.tm_mon + 1,
		   tm.tm_mday,
		   tm.tm_hour,
		   tm.tm_min);
	  fprintf (fp, "%i/%i ", rcvIdx, numReplies);
	  rcvIdx = rcvIdx > 8 ? 8 : rcvIdx;
	  for (int i=0; i<rcvIdx; i++)
	    {
	      fprintf (fp, " 0x%04x,0x%02x,0x%02x",
		       rcvId[i],
		       rcvCmd[i],
		       rcvErr[i]);
	    }
	  fprintf (fp, "\n");
	  fclose (fp);
	}
    }

#endif

  if (hasFatal)
    {
      GTLFATAL (("fatal error occurred\n"));
      return -1;
    }

  if (hasError)
    {
      return -1;
    }

  return 0;
}
