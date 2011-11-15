//
// sicklaser.cc
//
// referred documents:
//
// [1] SICK AG, "Definition der Telegramme zwischen Benutzerschnittstelle
//     und LMS- oder LMI-400 System über RS 422/RS 232", Version 04.20
//
// [2] SICK AG, "Telegramme zur Bedienung / Konfiguration der Lasermesssysteme
//     LMS 2xx Firmware-Version V2.10/X1.14", Dokument
//     8 007 953/0000/04-04-2003, 09.04.2003, (128 Seiten)
//     "file://TLLMS2xxD_8007953_04042003.pdf"
//

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>   // FIONREAD, TIOCGCOUNT
#include <linux/serial.h>

#include "./sicklaser.h"
#include "./defines.h"

//#ifdef DEBUG
//#undef DEBUG
//#endif
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSICKLASER::CSICKLASER (int idx, const bool hiSpeed)
  : CRS422 (idx)
{
  _idx = idx;
  _hiSpeed = hiSpeed;

  _inBuf = 0;
  _msgOk = _msgKo = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSICKLASER::~CSICKLASER (void)
{
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CSICKLASER::Reset (void)
{
  GTL ("CSICKLASER::Reset()");

  //

#if 0

  SetRaw ();

  SetSize (8);
  SetStopBits (1);
  SetParity (NONE);
  SetCtsRts (0);

  SetVmin (64);
  SetVtime (1);

#endif

  //
  // ++itschere20020320: *** ATTENTION !!! ***
  //
  // on pc hardware (8250, 16550 etc.) the serial communication speed is
  // programmed by means of a divisor for a chip-internal clock
  // rate. historically most chips are driven by a 1.8432MHz quartz, which they
  // immediately internally divide by 16, leading to a "baud base" of
  // 115200. it is this value that is divided by the programmable divisor,
  // where (I've read) some (old?) chips only allowed divisors >= 2, leading to
  // a maximum speed of 57600.
  //
  // more modern implementations of the 16550 design allow the chip to be
  // driven with a much higher clock, e.g. the MOXA CI-132 we use is usually
  // driven by a 14.7456MHz clock (8 * 1.8432MHz). it probably just uses an
  // additional divide-by-8 step that can be switched on or off. the baud base
  // achieved by this is 8 times higher, say 921600. the result is of course
  // that all divisors must a larger by a factor of 8 too to achieve the
  // classical speeds.
  //
  // the SICK laser scanner uses a speed of 500000, which is by no means
  // achievable by dividing any of the above number by any integer number. to
  // get this speed the 14.7456MHz quartz on the board was replace by a 16MHz
  // quartz, leading to a physical baud base of 1000000 instead of
  // 921600. however, the software driver doesn't know about this and therefore
  // things get a bit hairy:
  //
  // you have to know that the chip physically uses a speed 16/14.7456=1.085
  // times higher that what the software thinks it does and deliberately
  // program "wrong" baud rates (divisors). for 500000 the virtual speed is
  // 460800, which luckily is one of the legal speeds for the unmodified board
  // so the driver may just do what it wants to. for 9600 the virtual speed is
  // 8847.36, which is not. what we do is use a patched driver which catches
  // this special case and uses a divisor of 104 (921600/8847.36=104.166),
  // knowing that this leads to a virtual speed of 921600/104=8861.53 which we
  // know corresponds to a physical speed of 9615.38 and hope for the best...
  //

  GTLPRINT (("-> SPEED 500000\n"));

  SetSpeed (500000);

  Mode (0x25, false);   // stop any continuous output, discarding errors
  usleep (100000);
  Mode (0x42, false);   // try to switch from 500000 to 9600, discarding errors
  usleep (100000);

  GTLPRINT (("-> SPEED 38400\n"));

  SetSpeed (38400);

  Mode (0x25, false);   // stop any continuous output, discarding errors
  usleep (100000);
  Mode (0x42, false);   // try to switch from 38400 to 9600, discarding errors
  usleep (100000);

  GTLPRINT (("-> SPEED 19600\n"));

  SetSpeed (19200);

  Mode (0x25, false);   // stop any continuous output, discarding errors
  usleep (100000);
  Mode (0x42, false);   // try to switch from 19200 to 9600, discarding errors
  usleep (100000);

  GTLPRINT (("-> SPEED 9600\n"));

  SetSpeed (9600);

  Mode (0x25, false);   // stop any continuous output, discarding errors

  sleep (1);

  FlushInput ();   // discard any input
  FlushInput ();   // discard any input
  FlushInput ();   // discard any input

  GTLPRINT (("speed synchronized\n"));

  //

  if (ResetSick () < 0)   // sets speed to 9600
    {
      GTLFATAL (("ResetSick() failed\n"));
      throw -42;
    }

  GTLPRINT (("reset successful\n"));

  //

  if (_hiSpeed)
    {
      GTLPRINT(("setting high speed mode -> SPEED 500000\n"));

      if (Mode (0x48) < 0)   // configure to 500000 baud
	{
	  GTLFATAL (("Mode(0x48) failed\n"));
	  throw -42;
	}

      SetSpeed (500000);
    }
  else
    {
      GTLPRINT(("setting normal speed mode -> SPEED 38400\n"));

      if (Mode (0x40) < 0)   // configure to 38400 baud
	{
	  GTLFATAL (("Mode(0x40) failed\n"));
	  throw -42;
	}

      SetSpeed (38400);
    }

  GTLFATAL (("laser #%i up and running\n", _idx));
}


///////////////////////////////////////////////////////////////////////////////
//
// CSICKLASER::WriteSick()
//
// SICK documentation [1][2] enforces a timing on data being sent to a LMS/LMI
// not only on telegram level, but also on byte level. I could have called this
// function "WriteReally" in accordance with "ReadReally", but since this
// timing thing is a totally sick idea I've called this function as it is
// called.
//
///////////////////////////////////////////////////////////////////////////////

int CSICKLASER::WriteSick (const unsigned char *buf, const int size) const
{
  GTL ("CSICKLASER::WriteSick()");

  //

  int todo = size;
  const unsigned char *ptr = buf;
  struct timeval now, last = {0, 0};

  //

  while (todo > 0)
    {
      gettimeofday (&now, 0x0);

      int us =
	(now.tv_usec - last.tv_usec) +
	((now.tv_sec - last.tv_sec) > 0 ? 1000000 : 0);

      if (last.tv_sec && (us > 5000))
	{
	  // SICK specs [1][2] say that there mustn't be more than 6ms between
	  // two bytes sent to a LMS/LMI, we better take 5ms.

	  GTLFATAL (("timing exceeded, expect problems\n"));
	}

      if (us < 1000)
	{
	  // SICK specs [1][2] say that there must be at least 55us between two
	  // bytes sent to a LMS/LMI. we've seen 150us failing and therefore
	  // use a much higher value (doesn't hurt much if short commands are
	  // slowed down even more). in case this should not be enough the
	  // higher level calls should re-call their SendTelegram() if they're
	  // interested in their calls succeeding.

	  continue;
	}

      // send 1 byte, sigh...

      if (Write (ptr, 1) != 1)
	{
	  GTLFATAL (("Write() failed\n"));
	  return -1;
	}

      gettimeofday (&last, 0x0);

      ptr++;
      todo--;
    }

  //

  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

unsigned short CSICKLASER::ComputeCrc (unsigned char *ptr, int len)
{
  unsigned short crc = 0;
  unsigned char data[2] = {0, 0};

  while (--len >= 0)
    {
      data[1] = data[0];
      data[0] = *ptr++;

      if (crc & 0x8000)
	{
	  crc = (crc & 0x7fff) << 1;
	  crc ^= 0x8005;   // generator polynomial
	}
      else
	{
	  crc <<= 1;
	}

      crc ^= (data[0] | (data[1] << 8));
    }

  return crc;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSICKLASER::SendTelegram (const unsigned char *cmd,
			      int len,
			      const bool wait)
{
  GTL ("CSICKLASER::SendTelegram()");

  unsigned char buf[1024], *ptr = buf, *start;

  // so that junk in the receiver buffer won't clog the acknowledge

  FlushInput ();

  // construct buffer

  start = ptr;

  *ptr++ = 0x02;   // STX

  *ptr++ = 0x00;   // ADR

  *ptr++ = len & 255;   // LENL
  *ptr++ = len >> 8;   // LENH

  memcpy (ptr, cmd, len);
  ptr += len;

  unsigned short crc = ComputeCrc (start, 4 + len);

  *ptr++ = crc & 255;
  *ptr++ = crc >> 8;

  // send buffer

  int size = ptr - buf;

  if (WriteSick (buf, size) != size)
    {
      GTLFATAL (("WriteSick() failed\n"));
      return -1;
    }

  //

  if (wait)
    {
      unsigned char c;

      //

      if (ReadTimeout (&c, 1, 1) != 1)
	{
	  GTLFATAL (("Read() failed, no acknowledge?\n"));
	  return -1;
	}

      if (c != 0x06)
	{
	  GTLFATAL (("acknowledge failed (0x%02x instead 0x06)\n", c));
	  return -1;
	}
    }

  //

  return len;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSICKLASER::ReceiveTelegram (unsigned char *cmd, const int maxLen)
{
  GTL ("CSICKLASER::ReceiveTelegram()");

  int got;
  unsigned char buf[1024];

  if ((got = Read (buf, 4)) != 4)
    {
      GTLFATAL (("failed to Read() telegram header (%i)\n", got));
      return -1;
    }

  // STX

  if (buf[0] != 0x02)
    {
      GTLFATAL (("failed to identify telegram header\n"));

      while (42)
	{
	  GTLPRINT (("0x%02x 0x%02x 0x%02x 0x%02x\n", buf[0], buf[1], buf[2], buf[3]));

	  if (Read (buf, 4) != 4)
	    {
	      abort ();
	    }
	}

      return -1;
    }

  // ADR

  if (buf[1] != 0x80)
    {
      GTLFATAL (("buffer weirdness\n"));
      return -1;
    }

  // LEN

  int size = (buf[3] << 8) | buf[2];

  // CMD+DATA+CRC

  if (Read (buf + 4, size + 2) != size + 2)
    {
      GTLFATAL (("failed to Read() telegram body\n"));
      return -1;
    }

  // CRC

  unsigned short crc = (buf[4+size+1] << 8) | buf[4+size];

  if (ComputeCrc (buf, 4 + size) != crc)
    {
      GTLFATAL (("checksum failed\n"));
      return -1;
    }

  //

  memcpy (cmd, buf + 4, size);

  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSICKLASER::ResetSick (void)
{
  GTL ("CSICKLASER::ResetSick()");

  //

  unsigned char RESET[] = {0x10};

  while (SendTelegram (RESET, 1) <= 0)   // be insistent
    {
      GTLPRINT (("SendTelegram(RESET) failed\n"));
      usleep (100000);
    }

  //

  Mark ();

  //

  unsigned char buf[1024];

  if (ReceiveTelegram (buf, 1024) < 0)
    {
      GTLFATAL (("ReceiveTelegram() failed\n"));
      abort ();
    }

  if (buf[0] != 0x91)
    {
      GTLFATAL (("expected 0x91 reply, got 0x%02x\n", buf[0]));
      abort ();
    }

  //

  Mark ();

  GTLPRINT (("please be patient, the next step may take some seconds\n"));

  //

  int len = ReceiveTelegram (buf, 1024);

  if (len < 0)
    {
      GTLFATAL (("ReceiveTelegram() failed\n"));
      abort ();
    }

  if (buf[0] != 0x90)
    {
      GTLFATAL (("expected 0x90 reply, got 0x%02x\n", buf[0]));
      abort ();
    }

  buf[1+len] = 0;

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSICKLASER::GetType (void)
{
  GTL ("CSICKLASER::GetType()");

  //

  unsigned char CMD[] = {0x3a};

  while (SendTelegram (CMD, sizeof (CMD)) <= 0)   // be insistent
    {
      GTLFATAL (("SendTelegram() failed\n"));
    }

  //

  unsigned char buf[1024];

  int len = ReceiveTelegram (buf, 1024);

  if (buf[0] != 0xba)
    {
      GTLFATAL (("expected 0xba reply, got 0x%02x\n", buf[0]));
      abort ();
    }

  //

  buf[1+len] = 0;

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSICKLASER::Mode (unsigned char mode, const bool wait)
{
  GTL ("CSICKLASER::Mode()");
  GTLPRINT (("mode=0x%02x\n", mode));

  unsigned char MODE[] = {0x20, mode};

  while (42)
    {
      if (SendTelegram (MODE, 2, wait) > 0)
	{
	  break;
	}

      if (wait)
	{
	  GTLFATAL (("SendTelegram() failed\n"));
	  continue;
	}

      return 0;
    }

  //

  if (wait)
    {
      unsigned char buf[1024];

      if (ReceiveTelegram (buf, 1024) < 0)
	{
	  GTLFATAL (("ReceiveTelegram() failed\n"));
	  return -1;
	}

      if (buf[0] != 0xa0)
	{
	  GTLFATAL (("expected 0xa0 reply, got 0x%02x\n", buf[0]));
	  abort ();
	}
    }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline unsigned short Peek16Inc (unsigned char **ptr)
{
  unsigned short val = (*ptr)[0] | ((*ptr)[1] << 8);

  *ptr += 2;

  return val;
}


static inline unsigned short Mask (const unsigned short val,
				   const unsigned int start,
				   const unsigned int count)
{
  return (val >> start) & ((1 << count) - 1);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CSICKLASER::StartScanContinuous (void)
{
  GTL ("CSICKLASER::StartScanContinuous()");

#if 1

  FlushInput();

  Mode (0x24, false);

  FlushInput();
  FlushInput();

  Mode (0x24);

  FlushInput();
  FlushInput();
  FlushInput();

#else

  if (Mode (0x24) < 0)
    {
      GTLFATAL (("Mode() failed\n"));
      return -1;
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CSICKLASER::GetScanContinuousSick (CSICKSCAN &sickScan)
{
  GTL ("CSICKLASER::GetScanContinuousSick(CSICKSCAN&)");

  //
  // a specialized version of ReceiveTelegram()
  //

  unsigned int skipped = 0;

  //
  //
  //

  while (42)
    {
#if 1

      int got, todo = 732 - _inBuf;

      if ((got = Read (_scanBuf + _inBuf, todo)) < 0)
	{
	  GTLFATAL (("oops, read() on rt-linux fifo failed?\n"));
	  continue;
	}

      _inBuf += got;

#else

      // wait for data

      fd_set rfd;

      FD_ZERO (&rfd);
      FD_SET (_fd, &rfd);

      struct timeval timeout = {1, 0};

      if (select (_fd+1, &rfd, 0x0, 0x0, &timeout) < 1)
	{
	  GTLFATAL (("select() failed or timed out\n"));
	  continue;   // keep things going
	}

      //
      // read as much data as possible
      //

      int now;

      if (ioctl (_fd, FIONREAD, &now) < 0)
	{
	  GTLFATAL (("ioctl(FIONREAD) failed\n"));
	  continue;   // keep things going
	}

      now = BUFSIZE - _inBuf < now ? BUFSIZE - _inBuf : now;

      int got;

      if ((got = Read (_scanBuf + _inBuf, now)) < 1)
	{
	  GTLFATAL (("read() failed\n"));
	  continue;   // keep things going
	}

      _inBuf += got;

#endif

      //
      // take advantage of the knowledge that reply telegrams in (at
      // least this particular) continuous mode are of size 732.
      //
      // answer '0xb0' of LMS2xx: [2] p. 45-49
      //

      if (_inBuf < 732)
	{
	  continue;   // no telegram yet
	}

      //
      // actually, the telegram consists of:
      //
      //   1 : 0x02 (STX - start of text)
      //   1 : 0x80 (address)
      //   2 : size (726)
      //   1 : 0xb0                                     [2] p. 45-49
      //   2 : flags
      // 722 : scan data (361*2=722)
      //   1 : state of LMS (only in answer telegrams)  [2] p. 103
      //   2 : crc
      // ---
      // 732
      //

      bool trashMe = true;

      const int size = (_scanBuf[3] << 8) | _scanBuf[2];

      if ((_scanBuf[0] == 0x02) &&
	  (_scanBuf[1] == 0x80) &&
	  (size == 726))
	{
	  const unsigned short crc1 = (_scanBuf[4+size+1] << 8) | _scanBuf[4+size];
	  const unsigned short crc2 = ComputeCrc (_scanBuf, 4 + size);

	  if (crc1 == crc2)
	    {
	      trashMe = false;
	    }
	}

      if (trashMe)
	{
	  // telegram integrity failure, search next telegram (next 0x02 byte)

	  int idx;

	  for (idx=1; idx<_inBuf; idx++)
	    {
	      if (_scanBuf[idx] == 0x02)
		{
		  break;
		}
	    }

	  // `idx' points to either the next 0x02 byte or the end of the buffer

	  _inBuf -= idx;
	  skipped += idx;
	  memmove (_scanBuf, _scanBuf+idx, _inBuf);

	  continue;
	}

      //
      // legal telegram received, do some sanity checks
      //
      // ++westhoff20040310: if 'info->rx_trigger = 14' in source of kernel
      // module 'mxser.o' then skipped is true for more then a third of all
      // scans. 'rx_trigger' trigger has to be set to 8, then the interupt to
      // read data from the serial card is thrown more often but no bytes (and
      // scans) get lost.
      //

      if (skipped)
	{
	  _msgKo++;

	  char buf[128];

	  sprintf (buf,
		   "scanner=%i, msg=%i/%i, skipped=%i",
		   _idx,
		   _msgKo,
		   _msgOk,
		   skipped);

	  GTLFATAL (("%s\n", buf));

	  // GTLCOUNT (0, buf);

	  skipped = 0;
	}

      //
      //
      //

      unsigned char *data = _scanBuf + 5;

      const unsigned short val = Peek16Inc (&data);

      if ((Mask (val,  0, 10) != 361) ||   // number of scans
	  (Mask (val, 11,  2) != 0) ||     // number of partial scan
	  (Mask (val, 13,  1) != 0) ||     // flag for partial scan
	  (Mask (val, 14,  2) != 1))       // unit
	{
	  GTLFATAL (("scan data weirdness, ignoring\n"));

	  _msgKo++;

	  _inBuf -= 732;
	  memmove (_scanBuf, _scanBuf+732, _inBuf);

	  continue;
	}

      //
      // sane structure received, analyze it
      //

      {
	float *distPtr = sickScan._dist;

#if MOBILEVERSION == HAMBURG
        unsigned char *intensityPtr = sickScan._intensity;
#else
	bool *blendPtr = sickScan._blend;
#endif

	int cnt = 361;

#if 0
	unsigned char *dataSave = data;

	for (int i=0; i<360; i+=8)
	  {
	    unsigned short d1 = Peek16Inc (&data);
	    unsigned short d2 = Peek16Inc (&data);
	    unsigned short d3 = Peek16Inc (&data);
	    unsigned short d4 = Peek16Inc (&data);
	    unsigned short d5 = Peek16Inc (&data);
	    unsigned short d6 = Peek16Inc (&data);
	    unsigned short d7 = Peek16Inc (&data);
	    unsigned short d8 = Peek16Inc (&data);

	    GTLPRINT (("0x %04x %04x %04x %04x %04x %04x %04x %04x : %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
		       d1, d2, d3, d4, d5, d6, d7, d8,
		       d1 / 1000.0,
		       d2 / 1000.0,
		       d3 / 1000.0,
		       d4 / 1000.0,
		       d5 / 1000.0,
		       d6 / 1000.0,
		       d7 / 1000.0,
		       d8 / 1000.0));
	  }

	data = dataSave;

	abort ();
#endif

	int numberOfBlends = 0;

	while (--cnt >= 0)
	  {
	    const unsigned short val = Peek16Inc (&data);

#if MOBILEVERSION == HAMBURG

	    // ++westhoff20040209: use last three bits as reflector intensity
            // value.

	    // ++itschere20040325: obviously the HAMBURG scanner operates with
	    // different settings. it delivers values 0..7 in bits 14..16 where
	    // the others only deliver 0..1 in bit 16. actually it would be
	    // possible to use a combined mode because scanners != HAMBURG
	    // don't use bits 14 and 15, so if they would evalutae bits 14..16
	    // they would return either 0 or 4, which is still a boolean
	    // statement...

	    // max. 8.191m (2^13-1)
	    const int dist = Mask (val, 0, 13);
#else
	    // also max. 8.191m (2^13-1), because bits #14 and #15 are unused
	    const int dist = Mask (val, 0, 15);
#endif

	    if (dist <= 0x1ff7)   // maximal legal value in 8m range...
	      {
		*distPtr++ = dist / 1000.0;

#if MOBILEVERSION == HAMBURG

                // ++westhoff20040401: intensities will be returned
                // 8 values for reflector
                const unsigned char intensity = Mask (val, 13, 3);

#warning "Angle may be wrong in GTLPRINT because count is decreased!"

                *intensityPtr++ = intensity;

                if (intensity > 0)
                {
                  numberOfBlends++;

                  GTLPRINT (("Blending: angle = %5.1f°   distance = %5.3f m   intensity = %u\n",
                            cnt * 0.5,
                            *(distPtr-1),
                            intensity));
                }

#else
		// 1 value for reflector
		const bool blended = Mask (val, 15, 1);

		if (*blendPtr++ = blended)
		  {
		    numberOfBlends++;
		  }
#endif
	      }
	    else
	      {
		*distPtr++ = -1.0;
#if MOBILEVERSION == HAMBURG
                *intensityPtr++ = 0;
#else
		*blendPtr++ = false;
#endif
	      }

	    // ...other maximal legal values are  0x3ff7 (16m) and 0x7ff7 (32m)
	  }

	GTLPRINT (("Blendings seen in scan: %d\n", numberOfBlends));
      }

#if MOBILEVERSION == HAMBURG

      //
      // ++westhoff20040107: print state of scan, the state can indicate
      //                     errors, messages and informations, but i have
      //                     no idea what the messages realy mean
      //                     -> see [2], p. 103
      //

      const char state =  _scanBuf[4+size-1];

      static char binary [9];

      char wert, c, n=0;

      wert = state;

      while (n < 8)
	{
	  c = wert & 0x80;       // höchstes Bit von wert nach c

	  wert = wert << 1;      // wert ein Bit nach links

	if (!c)                  // c==0: 0 in den String
	  {
	    binary [n] = '0';
	  }
	else                     // c!=0: 1 in den String
	  {
	    binary [n] = '1';
	  }

	n++;                     // Umlaufzähler erhöhen
      }

      binary [n] = 0;             // String nullterminieren


      GTLPRINT (("State of scan: %s\n", binary));

      if (state != 0x10)
	{
	  GTLFATAL (("Received problematic scan, check state!\n"));
	  //abort ();
	}

#endif

      //
      // got a valid CSICKSCAN
      //

      _msgOk++;

      _inBuf -= 732;
      memmove (_scanBuf, _scanBuf+732, _inBuf);

      break;
    }

  //
  // we've filled in `sickScan' and can return
  //
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CSICKLASER::GetSerialPortStatistics (int &rx,
					  int &rxOverrun,
					  int &tx)
{
  GTL ("CSICKLASER::GetSerialPortStatistics()");

  struct serial_icounter_struct iCounter;

  if (_ttys->Ioctl (TIOCGICOUNT, &iCounter) < 0)
    {
      GTLFATAL (("CTTYS::Ioctl() failed\n"));
      return;
    }

  rx = iCounter.rx;
  rxOverrun = iCounter.overrun;
  tx = iCounter.tx;
}
