//
// mouse.cpp
// (C) 12/2001 by TeSche (Torsten Scherer)
// itschere@techfak.uni-bielefeld.de
//

#include <stdio.h>
#include <stdlib.h>

#include "tty/mouse.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMOUSE::CMOUSE (const char *devName) : CTTYS (devName)
{
  GTL ("CMOUSE::CMOUSE()");
  GTLPRINT (("dev=%s\n", devName));

  SetRaw ();
  SetSpeed (1200);
  SetSize (7);
  SetStopBits (1);
  SetParity (NONE);
  SetCtsRts (1);

  char c;

  if (Read (&c, 1) != 1)
    {
      GTLFATAL (("Read() failed, can't be no mouse\n"));
      abort ();
    }

  if (c != 'M')
    {
      GTLFATAL (("Read() doesn't return `M', can't be no mouse\n"));
      abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMOUSE::~CMOUSE (void)
{
  GTL ("CMOUSE::~CMOUSE()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CMOUSE::GetEvent (int &buttons, int &dx, int &dy)
{
  GTL ("CMOUSE::GetEvent()");

  unsigned char c[] = {0, 0, 0};

  // go into a loop reading characters from the device until a complete mouse
  // data paket is received

  while (42)
    {
      c[0] = c[1];
      c[1] = c[2];

      if (Read (&c[2], 1) != 1)
	{
	  fprintf (stderr, "Read() failed\n");
	  abort ();
	}

      // these are the only constraints we can be certain about that indicate a
      // legal mouse data paket according to the microsoft protocol.

      if (((c[0] & 0xc0) == 0x40) &&
	  ((c[1] & 0x80) == 0x00) &&
	  ((c[2] & 0x80) == 0x00))
	{
	  break;
	}
    }

  buttons = (c[0] & 0x30) >> 4;
  dx = (char)(((c[0] & 0x03) << 6) | (c[1] & 0x3f));
  dy = (char)(((c[0] & 0x0c) << 4) | (c[2] & 0x3f));
}


int CMOUSE::GetEvent (int &buttons,
		      int &dx,
		      int &dy,
		      const milliseconds_t timeout)
{
  GTL ("CMOUSE::GetEvent()");

  unsigned char c[] = {0, 0, 0};

  // go into a loop reading characters from the device until a complete mouse
  // data paket is received

  while (42)
    {
      c[0] = c[1];
      c[1] = c[2];

      if (WaitIn (timeout) < 1)
	{
	  return -1;
	}

      if (Read (&c[2], 1) != 1)
	{
	  fprintf (stderr, "Read() failed\n");
	  abort ();
	}

      // these are the only constraints we can be certain about that indicate a
      // legal mouse data paket according to the microsoft protocol.

      if (((c[0] & 0xc0) == 0x40) &&
	  ((c[1] & 0x80) == 0x00) &&
	  ((c[2] & 0x80) == 0x00))
	{
	  break;
	}
    }

  buttons = (c[0] & 0x30) >> 4;
  dx = (char)(((c[0] & 0x03) << 6) | (c[1] & 0x3f));
  dy = (char)(((c[0] & 0x0c) << 4) | (c[2] & 0x3f));

  return 0;
}
