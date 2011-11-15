//
//
//

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined (__WATCOMC__)
  #include <termios.h>
#endif

#if defined (linux)
  #include <sys/ioctl.h>
#endif

#include "tty/tty.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CTTY::Init (void)
{
#ifdef _WIN32

  if (!GetCommState (_fd, &_tFlags))
    {
      perror ("CTTY::CTTY(): GetCommState()");
      abort ();
    }

#else

  if (tcgetattr (_fd, &_tFlags) < 0)
    {
      perror ("CTTY::CTTY(): tcgetattr()");
      abort ();
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if !defined (_WIN32)

CTTY::CTTY (const int fd)
{
  _fd = fd;
  _closeMe = 0;

  Init ();
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CTTY::~CTTY (void)
{
  if (_closeMe)
    {
#ifdef _WIN32
      CloseHandle (_fd);
#else
      close (_fd);
#endif
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CTTY::SetRaw (void)
{
#if defined (_WIN32)

  // raw by default

#elif defined (linux)

  cfmakeraw (&_tFlags);

  if (tcsetattr (_fd, TCSANOW, &_tFlags) < 0)
    {
      perror ("CTTY::SetRaw(): tcsetattr()");
      abort ();
    }

#elif defined (__WATCOMC__)

  // ++itschere20011026: there a html manpage of what cfmakeraw() does under
  // Watcom-C @ QNX online at www.qnx.com, stating that you should include
  // termios.h, but that file doesn't seem to contain a definition of
  // cfmakeraw() on our installation so we'll have to do it manually...
  //
  // ++itschere20011031: yeah, there's a manpage listing pthread calls too, but
  // again I can't find any of them on our machine. see comments in thread/.

  _tFlags.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
  _tFlags.c_oflag &= ~OPOST;
  _tFlags.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
  _tFlags.c_cflag &= ~(CSIZE|PARENB);
  _tFlags.c_cflag |= CS8;

  if (tcsetattr (_fd, TCSANOW, &_tFlags) < 0)
    {
      perror ("CTTY::SetRaw(): tcsetattr()");
      abort ();
    }

#elif defined (__svr4__)

  _tFlags.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL | IUCLC | IMAXBEL);
  _tFlags.c_oflag &= ~OPOST;

  _tFlags.c_cflag |= HUPCL | CLOCAL;

#if 1
  _tFlags.c_cflag &= ~(ECHO | ECHONL | ICANON | ISIG);
#endif

  if (tcsetattr (_fd, TCSANOW, &_tFlags) < 0)
    {
      perror ("CTTY::SetRaw(): tcsetattr()");
      abort ();
    }

#else

  #error "operating system not supported"

#endif

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CTTY::SaveFlags (void)
{
  _tFlagsSaved = _tFlags;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CTTY::RestoreFlags (void)
{
  _tFlags = _tFlagsSaved;

#ifdef _WIN32

  if (!SetCommState (_fd, &_tFlags))
    {
      perror ("CTTY::RestoreFlags() -- SetCommState()");
      abort ();
    }

#else

  if (tcsetattr (_fd, TCSANOW, &_tFlags) < 0)
    {
      perror ("CTTY::RestoreFlags() -- tcsetattr()");
      abort ();
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CTTY::Read (void *ptr, const int size) const
{
#ifdef _WIN32

  unsigned long ret;

  ReadFile (_fd, ptr, size, &ret, 0x0);

  return ret;

#else

  return read (_fd, ptr, size);

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CTTY::Write (const void * const ptr, const int size) const
{
#ifdef _WIN32

  unsigned long ret;

  WriteFile (_fd, ptr, size, &ret, 0x0);

  return ret;

#else

  return write (_fd, ptr, size);

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if defined (linux)

int CTTY::WaitIn (const milliseconds_t timeout) const
{
  fd_set rfd;

  FD_ZERO (&rfd);
  FD_SET (_fd, &rfd);

  struct timeval tv = {timeout / 1000, (timeout % 1000) * 1000}, *tvp;

  if (timeout < 0)
    {
      tvp = 0x0;
    }
  else
    {
      tvp = &tv;
    }

  // this either returns a negative error, 0 if no data is available or +1 if
  // data is available

  return select (_fd + 1, &rfd, 0x0, 0x0, tvp);
}


void CTTY::FlushInput (void) const
{
  tcflush (_fd, TCIFLUSH);
}


int CTTY::Ioctl (int cmd, void *ptr)
{
  return ioctl (_fd, cmd, ptr);
}


#endif
