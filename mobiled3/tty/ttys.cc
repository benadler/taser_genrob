//
//
//

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined (linux)
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/serial.h>
#endif

#include "tty/ttys.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////



//
//
//

CTTYS::CTTYS (const char *dev)
{
#ifdef _WIN32

  if (!(_fd = CreateFile (dev,
			  GENERIC_READ | GENERIC_WRITE,
			  0,   // exclusive open
			  0x0,   // security attributes
			  OPEN_EXISTING,
#if 1
			  FILE_FLAG_NO_BUFFERING |
#endif
			  FILE_FLAG_OVERLAPPED,   // file attributes
			  0x0)))   // template file for attributes
    {
      perror ("CTTYS::CTTYS(): CreateFile()");
      abort ();
    }

  // GetCommState()

  Init ();
  
  //

  _tFlags.DCBlength = sizeof (DCB);
  _tFlags.fBinary = TRUE;   // enable binary mode
#if 1
  _tFlags.fOutxDsrFlow = FALSE;
#else
  _tFlags.fOutxDsrFlow = TRUE;
#endif
  _tFlags.fDtrControl = DTR_CONTROL_ENABLE;
#if 1
  _tFlags.fDsrSensitivity = FALSE;
#else
  _tFlags.fDsrSensitivity = TRUE;
#endif
  _tFlags.fTXContinueOnXoff = FALSE;
  _tFlags.fOutX = FALSE;
  _tFlags.fInX = FALSE;
  _tFlags.fErrorChar = FALSE;
  _tFlags.fNull = FALSE;
  _tFlags.fAbortOnError = FALSE;
  //_tFlags.xOnLim = 0..32767
  //_tFlags.XoffLim = 0..32767
  //_tFlags.XonChar
  //_tFlags.XoffChar
  //_tFlags.ErrorChar
  //_tFlags.EofChar
  //_tFlags.EvtChar

  if (!SetCommState (_fd, &_tFlags))
    {
      perror ("CTTYS::CTTYS(): SetCommState()");
      abort ();
    }

  //

  COMMTIMEOUTS timeouts;

#if 1
  // wait forever
  timeouts.ReadIntervalTimeout = 0;
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.ReadTotalTimeoutConstant = 0;
  timeouts.WriteTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant = 0;
#else
  timeouts.ReadIntervalTimeout = 15;   // up to 14ms as per SICK manual
  timeouts.ReadTotalTimeoutMultiplier = 0;
  timeouts.ReadTotalTimeoutConstant = 100;
  timeouts.WriteTotalTimeoutMultiplier = 0;
  timeouts.WriteTotalTimeoutConstant = 100;
#endif

  if (!SetCommTimeouts (_fd, &timeouts))
    {
      perror ("CTTYS::CTTYS(): SetCommTimeouts()");
      abort ();
    }

  //

  if (!(_event = CreateEvent (0x0, TRUE, FALSE, 0x0)))
    {
      perror ("CTTYS::CTTYS(): CreateEvent()");
      abort ();
    }

#else

  if ((_fd = open (dev, O_RDWR | O_SYNC)) < 0)
    {
      printf ("Device: %s\n", dev);
      perror ("CTTYS::CTTYS(): open()");
      abort ();
    }

  Init ();

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

CTTYS::~CTTYS (void)
{
  CloseHandle (_event);
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CTTYS::SetSpeed (const int newSpeed)
{
#ifdef _WIN32

  // 56000 -> 500000 baud
  //  1075 ->   9600 baud

  _tFlags.BaudRate = newSpeed;

  if (!SetCommState (_fd, &_tFlags))
    {
      perror ("SetCommState()");
      abort ();
    }

#else

  speed_t speed;

  switch (newSpeed)
    {
    case 300:
      speed = B300;
      break;
    case 600:
      speed = B600;
      break;
    case 1200:
      speed = B1200;
      break;
    case 2400:
      speed = B2400;
      break;
    case 4800:
      speed = B4800;
      break;
    case 9600:
      speed = B9600;
      break;
    case 19200:
      speed = B19200;
      break;
    case 38400:
      speed = B38400;
      break;
    case 57600:
      speed = B57600;
      break;
    case 115200:
      speed = B115200;
      break;
    case 230400:
      speed = B230400;
      break;
    case 460800:
      speed = B460800;
      break;
    case 921600:
      speed = B921600;
      break;
    default:
      printf ("CTTYS::SetSpeed() -- illegal baud rate\n");
      abort ();
    }

  cfsetispeed (&_tFlags, speed);
  cfsetospeed (&_tFlags, speed);

  tcsetattr (_fd, TCSANOW, &_tFlags);

#endif

  return 0;
}


//
//
//

int CTTYS::SetSize (const int newSize)
{
#ifdef _WIN32

  _tFlags.ByteSize = 8;

  if (!SetCommState (_fd, &_tFlags))
    {
      perror ("SetCommState()");
      abort ();
    }

#else
	
  switch (newSize)
    {
    case 5:
      _tFlags.c_cflag &= ~CSIZE;
      _tFlags.c_cflag |= CS5;
      break;
    case 6:
      _tFlags.c_cflag &= ~CSIZE;
      _tFlags.c_cflag |= CS6;
      break;
    case 7:
      _tFlags.c_cflag &= ~CSIZE;
      _tFlags.c_cflag |= CS7;
      break;
    case 8:
      _tFlags.c_cflag &= ~CSIZE;
      _tFlags.c_cflag |= CS8;
      break;
    default:
      printf ("CTTYS::SetSize() -- illegal size\n");
      abort ();
    }

  tcsetattr (_fd, TCSANOW, &_tFlags);

#endif

  return 0;
}


//
//
//

int CTTYS::SetStopBits (const int newBits)
{
#ifdef _WIN32

  _tFlags.StopBits = ONESTOPBIT;

  if (!SetCommState (_fd, &_tFlags))
    {
      perror ("SetCommState()");
      abort ();
    }

#else

  switch (newBits)
    {
    case 1:
      _tFlags.c_cflag &= ~CSTOPB;
      break;
    case 2:
      _tFlags.c_cflag |= CSTOPB;
      break;
    default:
      printf ("CTTYS::SetStopBits() -- illegal number of stop bits\n");
      abort ();
    }

  tcsetattr (_fd, TCSANOW, &_tFlags);

#endif

  return 0;
}


//
//
//

int CTTYS::SetParity (const PARITY parity)
{
#ifdef _WIN32

  switch (parity)
    {
    case NONE:
      _tFlags.fParity = FALSE;
      _tFlags.Parity = NOPARITY;
      break;

    case ODD:
      _tFlags.fParity = TRUE;
      _tFlags.Parity = ODDPARITY;
      break;

    case EVEN:
      _tFlags.fParity = TRUE;
      _tFlags.Parity = EVENPARITY;
      break;

    default:
      printf ("CTTYS::SetParity() -- illegal flag\n");
      abort ();
    }

  if (!SetCommState (_fd, &_tFlags))
    {
      perror ("SetCommState()");
      abort ();
    }

#else

  switch (parity)
    {
    case NONE:
      _tFlags.c_cflag &= ~PARENB;
      break;

    case ODD:
      _tFlags.c_iflag |= IGNPAR | INPCK;
      _tFlags.c_cflag |= PARENB | PARODD;
      break;

    case EVEN:
      _tFlags.c_iflag |= IGNPAR | INPCK;
      _tFlags.c_cflag |= PARENB;
      break;

    default:
      printf ("CTTYS::SetParity() -- illegal flag\n");
      abort ();
    }

  tcsetattr (_fd, TCSANOW, &_tFlags);

#endif

  return 0;
}


//
//
//

int CTTYS::SetCtsRts (const int newCtsRts)
{
#ifdef _WIN32

  switch (newCtsRts)
    {
    case TRUE:
      _tFlags.fOutxCtsFlow = TRUE;
      _tFlags.fRtsControl = RTS_CONTROL_ENABLE;
      break;

    case FALSE:
      _tFlags.fOutxCtsFlow = FALSE;
      _tFlags.fRtsControl = RTS_CONTROL_DISABLE;
      break;
    }

  if (!SetCommState (_fd, &_tFlags))
    {
      perror ("SetCommState()");
      abort ();
    }

#elif !defined (__WATCOMC__)

  switch (!!newCtsRts)
    {
    case 1:
#ifdef __SVR4
      _tFlags.c_cflag |= CRTSCTS | CRTSXOFF;
#else
      _tFlags.c_cflag |= CRTSCTS;
#endif
      break;
    case 0:
#ifdef __SVR4
      _tFlags.c_cflag &= ~(CRTSCTS | CRTSXOFF);
#else
      _tFlags.c_cflag &= ~CRTSCTS;
#endif
      break;
    default:
      printf ("CTTYS::SetCtsRts() -- illegal flag\n");
      abort ();
    }

  tcsetattr (_fd, TCSANOW, &_tFlags);

#else

  // ++itschere20011026: Watcom-C doesn't have CRTSCTS?

#endif

  return 0;
}


//
//
//

#ifndef _WIN32

void CTTYS::SetLowLatency (void)
{
  struct serial_struct serial;

  ioctl (_fd, TIOCGSERIAL, &serial);

  serial.flags |= ASYNC_LOW_LATENCY;

  ioctl (_fd, TIOCSSERIAL, &serial);
}


int CTTYS::SetVmin (const int vmin)
{
  _tFlags.c_cc[VMIN] = vmin;

  if (tcsetattr (_fd, TCSANOW, &_tFlags) < 0)
    {
      perror ("CTTY::SetVmin(): tcsetattr()");
      abort ();
    }

  return 0;
}


int CTTYS::SetVtime (const int vtime)
{
  _tFlags.c_cc[VTIME] = vtime;

  if (tcsetattr (_fd, TCSANOW, &_tFlags) < 0)
    {
      perror ("CTTY::SetVtime(): tcsetattr()");
      abort ();
    }

  return 0;
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

int CTTYS::Read (void *ptr, const int size) const
{
  static const char * const fxnName = "CTTYS::Read() -- ";

  fprintf (stderr, "CTTYS::READ (void*, int)");

#if 0

  int cnt = size;
  char *s = (char *)ptr;
  unsigned long total;
  
  while (--cnt >= 0)
    {
      unsigned long ret;

      if (!ReadFile (_fd, s++, 1, &ret, 0x0))
	{
	  DWORD e = GetLastError ();
	  fprintf (stderr, "ReadFile() failed -> %i\n", e);
	  abort ();
	}

      if (ret != 1)
	{
	  fprintf (stderr, "ReadFile() returned no data\n");
	  abort ();
	}
    }

  total = size;

#else

  ResetEvent (_event);

  OVERLAPPED overlap;

  overlap.hEvent = _event;

  unsigned long ret;

  ReadFile (_fd, ptr, size, &ret, &overlap);

  if (WaitForSingleObject (_event, INFINITE) == WAIT_FAILED)
    {
      fprintf (stderr, "WaitForSingleObject() -> %i\n", GetLastError ());
      abort ();
    }

  unsigned long total;

  if (!GetOverlappedResult (_fd, &overlap, &total, FALSE))
    {
      fprintf (stderr, "%sGetOverlappedResult() failed\n", fxnName);
      return -1;
    }

#endif

  if (total != size)
    {
      fprintf (stderr, "%sfailed to read (%i/%i)\n", fxnName, total, size);
      return -1;
    }

  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CTTYS::Write (const void * const ptr, const int size) const
{
  static const char * const fxnName = "CTTYS::Write() -- ";

#if 0

  int cnt = size;
  const char *s = (const char *)ptr;
  unsigned long total;
  
  while (--cnt >= 0)
    {
      unsigned long ret;

      if (!WriteFile (_fd, s++, 1, &ret, 0x0))
	{
	  DWORD e = GetLastError ();
	  fprintf (stderr, "WriteFile() failed -> %i\n", e);
	  abort ();
	}
    }

  total = size;

#else

  ResetEvent (_event);

  OVERLAPPED overlap;

  overlap.hEvent = _event;

  unsigned long ret;

  WriteFile (_fd, ptr, size, &ret, &overlap);   // 997 - overlapped i/o in progress

  if (WaitForSingleObject (_event, INFINITE) == WAIT_FAILED)
    {
      fprintf (stderr,
	       "CTTYS::Write() -- WaitForSingleObject() failed (%i)\n",
	       GetLastError ());
      abort ();
    }

  unsigned long total;

  if (!GetOverlappedResult (_fd, &overlap, &total, FALSE))
    {
      fprintf (stderr, "%sGetOverlappedResult() failed\n", fxnName);
      return -1;
    }

#endif

  if (total != size)
    {
      fprintf (stderr, "%sfailed to write (%i/%i)\n", fxnName, total, size);
      return -1;
    }

  return size;
}

#endif
