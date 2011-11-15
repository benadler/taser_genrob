//
// rc422.cc
// (C) 01/03 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>


#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"

#include "./rs422.h"


#ifdef USE_RTLINUX

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRS422::CRS422 (const int idx)
{
  GTL ("CRS422::CRS422(const int)");

  char fName[16];

  sprintf (fName, "/dev/ttyMX%i", idx);

  if ((_fd = open (fName, O_RDWR)) < 0)
    {
      GTLFATAL (("failed to open fifo \"%s\"\n", fName));
      throw -42;
    }
}


CRS422::~CRS422 (void)
{
  close (_fd);
}


void CRS422::SetSpeed (const int newSpeed)
{
  ioctl (_fd, 0x12345678, newSpeed);
}


int CRS422::Read (void *ptr, const int size) const
{
  return read (_fd, ptr, size);
}


int CRS422::ReadTimeout (void *ptr, const int size, const int seconds) const
{
  GTL ("CRS422::ReadTimeout (void*, int, int)");

  fd_set rfd;

  FD_ZERO (&rfd);

  FD_SET (_fd, &rfd);

  struct timeval tv = {seconds, 0};

  if (select (_fd+1, &rfd, 0x0, 0x0, &tv) < 1)
    {
      return -1;
    }

  return read (_fd, ptr, size);
}


int CRS422::Write (const void *ptr, const int size) const
{
  return write (_fd, ptr, size);
}


void CRS422::FlushInput (void) const
{
  ioctl (_fd, 0x12345679, 0);
}


void CRS422::Mark (void) const
{
  ioctl (_fd, 0, 0);
}


#else

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRS422::CRS422 (const int idx)
{
  GTL ("CRS422::CRS422(const int)");

  char fName[16];

  sprintf (fName, "/dev/ttyM%i", idx);

  GTLPRINT(("Device: %s\n", fName));

  try
    {
      _ttys = new CTTYS (fName);

      _ttys->SetRaw ();

      _ttys->SetSpeed (9600);

      _ttys->SetLowLatency ();
    }

  catch (...)
    {
      GTLFATAL (("failed to open ttys \"%s\"\n", fName));
      throw -42;
    }
}


CRS422::~CRS422 (void)
{
  delete _ttys;
}


void CRS422::SetSpeed (const int newSpeed)
{
  GTL ("CRS422::SetSpeed(const int)");

  if (newSpeed == 500000)
    {
      _ttys->SetSpeed (460800);
    }
  else
    {
      _ttys->SetSpeed (newSpeed);
    }
}


int CRS422::Read (void *ptr, const int size) const
{
  GTL ("CRS422::Read (void*, int)");

  int todo = size, done = 0;

  while (todo > 0)
    {
      int got = _ttys->Read (ptr, todo);

      GTLPRINT (("CRS422::Read() -- got %i instead of %i\n", got, todo));

      if (got < 0)
	{
	  return done;
	}

      (unsigned char *)ptr += got;
      done += got;
      todo -= got;
    }

  GTLPRINT (("CRS422::Read() -- got %i\n", done));

  return done;
}


int CRS422::ReadTimeout (void *ptr, const int size, const int seconds) const
{
  GTL ("CRS422::ReadTimeout (void*, const int, const int) const");

#if 1

  sleep (seconds);

  return _ttys->Read (ptr, size);

#else

  fd_set rfd;

  FD_ZERO (&rfd);

  FD_SET (_fd, &rfd);

  struct timeval tv = {seconds, 0};

  if (select (_fd+1, &rfd, 0x0, 0x0, &tv) < 1)
    {
      return -1;
    }

  return read (_fd, ptr, size);

#endif
}


int CRS422::Write (const void *ptr, const int size) const
{
  return _ttys->Write (ptr, size);
}


void CRS422::FlushInput (void) const
{
  _ttys->FlushInput ();
}


void CRS422::Mark (void) const
{
}


#endif
