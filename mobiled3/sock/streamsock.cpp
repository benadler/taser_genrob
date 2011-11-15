//
// streamsock.cpp -
// (C) 01/1999-04/2001 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#if defined (_WIN32)
  // ++itschere20020111: must be before some yet unknown include to prevent troubles
  #include <winsock2.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>

#ifndef _WIN32
  #include <sys/ioctl.h>
  #include <sys/time.h>
#else
  #define ioctl ioctlsocket
#endif

#if defined (__GNUC__)
  #include <netdb.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <string.h>
  #if defined (__svr4__)   // solaris
    #include <sys/filio.h>
  #endif
#elif defined (_WIN32)
  #include <io.h>
#elif defined (__WATCOMC__)
  #include <netdb.h>
  #include <sys/select.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
#else
  #error "operating system not supported!"
#endif

#include "sock/streamsock.h"
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSTREAMSOCK::CSTREAMSOCK (void) : _CSOCK (STREAM)
{
  GTL ("CSTREAMSOCK::CSTREAMSOCK(void)");

  if ((_fd = ::socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      GTLFATAL (("socket() failed\n"));
      abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSTREAMSOCK::CSTREAMSOCK (const int fd,
			  unsigned int *rem_ipv4_addr,
			  unsigned short *rem_ipv4_port) : _CSOCK (STREAM)
{
  GTL ("CSTREAMSOCK::CSTREAMSOCK(int,...)");

  // this one is used internally for Accept().

  struct sockaddr_in remoteAddr;

#if defined(__GNUC__)
  unsigned int remoteAddrLen = sizeof (struct sockaddr);
#elif defined (_WIN32) || defined (__WATCOMC__)
  int remoteAddrLen = sizeof (struct sockaddr);
#else
#error "operating system not supported"
#endif

  if ((_fd = accept (fd,
		     (struct sockaddr *)&remoteAddr,
		     &remoteAddrLen)) < 0)
    {
      GTLFATAL (("accept() failed\n"));
      abort ();
    }

  if (rem_ipv4_addr)
    {
      *rem_ipv4_addr = ntohl (remoteAddr.sin_addr.s_addr);
    }

  if (rem_ipv4_port)
    {
      *rem_ipv4_port = ntohs (remoteAddr.sin_port);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSTREAMSOCK::~CSTREAMSOCK (void)
{
  GTL ("CSTREAMSOCK::~CSTREAMSOCK()");

  if (_fd >= 0)
    {
      shutdown (_fd, 2);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::Bind (unsigned short local_ipv4_port,
		       unsigned int local_ipv4_addr) const
{
  GTL ("CSTREAMSOCK::Bind()");

  //

  int opt = 1;

#if defined (sparc) || defined (_WIN32)
  if (::setsockopt (_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof (opt)) < 0)
#elif defined (linux) || defined (__WATCOMC__)
  if (::setsockopt (_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) < 0)
#else
#error "operating system not supported"
#endif
    {
      GTLFATAL (("setsockopt(SO_REUSEADDR) failed\n"));
      abort ();
    }

  //

  struct sockaddr_in local_addr;

  local_addr.sin_family = AF_INET;

  if (local_ipv4_addr)
    {
      local_addr.sin_addr.s_addr = htonl (local_ipv4_addr);
    }
  else
    {
      local_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    }

  local_addr.sin_port = htons (local_ipv4_port);

  int ret;

  if ((ret = bind (_fd,
		   (struct sockaddr *)&local_addr,
		   sizeof (struct sockaddr))) < 0)
    {
      GTLPRINT (("bind() failed\n"));
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::Listen (const int backlog) const
{
  GTL ("CSTREAMSOCK::Listen()");

  int ret;

  if ((ret = listen (_fd, backlog)) < 0)
    {
      GTLPRINT (("listen() failed\n"));
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSTREAMSOCK *CSTREAMSOCK::Accept (unsigned int *rem_ipv4_addr,
				  unsigned short *rem_ipv4_port) const
{
  GTL ("CSTREAMSOCK::Accept()");

  CSTREAMSOCK *sock = new CSTREAMSOCK (_fd, rem_ipv4_addr, rem_ipv4_port);

  if (!sock)
    {
      GTLPRINT (("out of memory?\n"));
      return 0x0;
    }

  if (sock->_fd < 0)
    {
      GTLPRINT (("accept() failed\n"));
      delete sock;
      return 0x0;
    }

  return sock;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::Connect (const unsigned int rem_ipv4_addr,
			  const unsigned short rem_ipv4_port,
			  const int timeout_s) const
{
  GTL ("CSTREAMSOCK::Connect(int,short,int)");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons (rem_ipv4_port);
  addr.sin_addr.s_addr = htonl (rem_ipv4_addr);

  int ret;

  if ((ret = connect (_fd,
		      (struct sockaddr *)&addr,
		      sizeof (struct sockaddr))) < 0)
    {
      GTLPRINT (("connect() failed\n"));
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::Connect (const char *rem_ipv4_addr,
			  const unsigned short rem_ipv4_port,
			  const int timeout_s) const
{
  GTL ("CSTREAMSOCK::Connect(char*,short,int)");

  struct hostent *he;

  if (!(he = ::gethostbyname ((char *)rem_ipv4_addr)))
    {
      GTLPRINT (("failed to resolve \"%s\"\n", rem_ipv4_addr));
      return -1;
    }

  struct sockaddr_in remoteAddr;

  remoteAddr.sin_family = AF_INET;
  remoteAddr.sin_port = htons (rem_ipv4_port);
  remoteAddr.sin_addr.s_addr = *((unsigned int *)he->h_addr_list[0]);

  int ret;

  if ((ret = connect (_fd,
		      (struct sockaddr *)&remoteAddr,
		      sizeof (struct sockaddr))) < 0)
    {
      GTLPRINT (("connect() failed\n"));
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
// ReadSome() -- non blocking, but size > 0
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::ReadSome (int size, void *buf) const
{
  GTL ("CSTREAMSOCK::ReadSome()");

  fd_set rfd;

  while (42)
    {
      FD_ZERO (&rfd);
      FD_SET (_fd, &rfd);

      // there used to be times and systems where there were problems when
      // select()ing for data without a timeout, so I grew the habit of always
      // select()ing in a loop and with a timeout. if the timeout is large
      // enough this does not mean any noticeable overhead penalty. on the
      // other hand, if you would use a timeout of just a few milliseconds it
      // would practically stop your system... ;-)

      struct timeval tv = {1, 0};

      int ret;

      if ((ret = select (_fd + 1, &rfd, 0x0, 0x0, &tv)) > 0)   // data avail
	{
	  break;
	}

      if (!ret)   // timeout
	{
	  continue;
	}

      if (errno == EINTR)
	{
	  GTLPRINT (("EINTR\n"));
	  continue;
	}

      GTLFATAL (("select() failed\n"));

      sleep (1);
    }

  if (!FD_ISSET (_fd, &rfd))
    {
      GTLFATAL (("select() failed\n"));
      return -1;
    }

  //
  // now we know that "some" data is available, but not yet how much.
  //

#ifdef _WIN32
  unsigned long cnt;
#else
  int cnt;
#endif

  if (ioctl (_fd, FIONREAD, &cnt) < 0)
    {
      GTLFATAL (("ioctl(FIONREAD) failed\n"));
      return -1;
    }

  if (cnt > size)
    {
      cnt = size;
    }

  //
  // now we know how much data we can safely read.
  //

#ifdef _WIN32
  int done = recv (_fd, (char *)buf, cnt, 0);
#else
  int done = read (_fd, buf, cnt);
#endif

  if (done != cnt)
    {
      GTLFATAL (("read() failed\n"));
#if 0
      int nWSAError = WSAGetLastError();
      printf("done: %d, WSAError: %d\n", done, nWSAError);
#endif
    }

  return done;
}


///////////////////////////////////////////////////////////////////////////////
//
// ReadSomeTimeout() -- timed blocking
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::ReadSomeTimeout (int size,
				  void *buf,
				  const milliseconds_t timeout) const
{
  GTL ("CSTREAMSOCK::ReadSomeTimeout()");

  if (timeout < 0)
    {
      return ReadSome (size, buf);
    }

  //

  struct timeval start;
  gettimeofday (&start, 0x0);

  milliseconds_t timeRemain = timeout;

  //

  while (42)
    {
      fd_set rfd;

      FD_ZERO (&rfd);
      FD_SET (_fd, &rfd);

      struct timeval tv = {timeRemain / 1000, (timeRemain % 1000) * 1000};

      int ret = select (_fd + 1, &rfd, 0x0, 0x0, &tv);

      if (ret > 1)
	{
	  GTLFATAL (("select() weirdness, returned %i\n", ret));
	  abort ();
	}

      if (ret == 1)   // got data
	{
	  if (!FD_ISSET (_fd, &rfd))
	    {
	      GTLFATAL (("select() failed\n"));
	      return -1;
	    }

	  break;
	}

      if (!ret)   // timeout
	{
	  return 0;
	}

      if (errno == EINTR)   // the only case where we restart
	{
	  // ++itschere20030318: linux manpage says that only linux select()
	  // changes the timeout argument to indicate how much time was left,
	  // other systems (probably qnx?) don't do that. so we can't rely on
	  // this behaviour and have to calculate the remaining timeout on our
	  // own.

	  struct timeval now;
	  gettimeofday (&now, 0x0);

	  const milliseconds_t elapsed = 1000 * (now.tv_sec - start.tv_sec)
	    + (now.tv_usec - start.tv_usec) / 1000;

	  timeRemain = elapsed > timeout ? 0 : timeout - elapsed;

	  continue;
	}

      GTLFATAL (("select() failed\n"));

      return -1;
    }

  //
  // now we know that "some" data is available, but not yet how much.
  //

#ifdef _WIN32
  unsigned long cnt;
#else
  int cnt;
#endif

  if (ioctl (_fd, FIONREAD, &cnt) < 0)
    {
      GTLFATAL (("ioctl(FIONREAD) failed\n"));
      return -1;
    }

  if (cnt > size)
    {
      cnt = size;
    }

  //
  // now we know how much data we can safely read.
  //

#ifdef _WIN32
  int done = recv (_fd, (char *)buf, cnt, 0);
#else
  int done = read (_fd, buf, cnt);
#endif

  if (done != cnt)
    {
      GTLFATAL (("read() failed\n"));
#if 0
      int nWSAError = WSAGetLastError();
      printf("done: %d, WSAError: %d\n", done, nWSAError);
#endif
    }

  return done;
}


///////////////////////////////////////////////////////////////////////////////
//
// Read() -- blocking
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::Read (int size, void *buf) const
{
  GTL ("CSTREAMSOCK::Read()");

  int done = 0;

  while (size > 0)
    {
#ifdef _WIN32
      int now = recv (_fd, (char *)buf, size, 0);
#else
      int now = read (_fd, buf, size);
#endif

      int err = errno;

      GTLPRINT (("read(%i) -> %i\n", size, now));

      if (now < 0)
	{
	  if (err == EINTR)
	    {
	      GTLPRINT (("EINTR\n"));
	      continue;
	    }

	  GTLFATAL (("read() failed\n"));

	  if (!done)
	    {
	      return now;   // the error
	    }

	  return done;
	}

      if (!now)
	{
	  // remote site has closed the connection (this is no error as such),
	  // return whatever we've got

	  break;
	}

      done += now;
      size -= now;
      buf = (char *)buf + now;
    }

  return done;
}


///////////////////////////////////////////////////////////////////////////////
//
// ReadTimeout1(), ReadTimeout2()
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::ReadTimeout1 (int size,
			       void *buf,
			       const milliseconds_t timeout) const
{
  GTL ("CSTREAMSOCK::ReadTimeout1()");

  int done = 0;

  while (size > 0)
    {
      int now = ReadSomeTimeout (size, buf, timeout);

      if (now < 0)   // error
	{
	  if (!done)
	    {
	      return now;
	    }

	  return done;
	}

      if (!now)   // timeout
	{
	  return done;
	}

      //

#if defined (__WATCOMC__)
      buf = (char *)buf + now;
#else
      (char *)buf += now;
#endif
      size -= now;
    }

  return done;
}


int CSTREAMSOCK::ReadTimeout2 (int size,
			       void *buf,
			       const milliseconds_t timeout) const
{
  GTL ("CSTREAMSOCK::ReadTimeout2()");

  int done = 0;

  struct timeval tvStart;
  gettimeofday (&tvStart, 0x0);

  milliseconds_t timeRemain = timeout;

  while ((size > 0) && (timeRemain > 0))
    {
      int now = ReadSomeTimeout (size, buf, timeRemain);

      if (now < 0)   // error
	{
	  if (!done)
	    {
	      return now;
	    }

	  return done;
	}

      if (!now)   // timeout
	{
	  return done;
	}

      //

#if defined (__WATCOMC__)
      buf = (char *)buf + now;
#else
      (char *)buf += now;
#endif
      size -= now;

      struct timeval tvNow;
      gettimeofday (&tvNow, 0x0);

      const milliseconds_t elapsed = 1000 * (tvNow.tv_sec - tvStart.tv_sec)
	+ (tvNow.tv_usec - tvStart.tv_usec) / 1000;

      timeRemain = elapsed > timeout ? 0 : timeout - elapsed;
    }

  return done;
}


///////////////////////////////////////////////////////////////////////////////
//
// Write() -- blocking
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::Write (int size, const void *buf) const
{
  GTL ("CSTREAMSOCK::Write()");

  int totalWritten = 0;

  while (size)
    {
#ifdef __GNUC__
      int written = write (_fd, buf, size);
#else
      int written = send (_fd, (char *)buf, size, 0);
#endif

      GTLPRINT (("write(%i) -> %i\n", size, written));

      if (written < 0)
	{
	  if (!totalWritten)
	    {
	      totalWritten = written;                       // the error number
	    }
	  break;
	}

      buf = (char *)buf + written;
      size -= written;
      totalWritten += written;
    }

  return totalWritten;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSTREAMSOCK::Shutdown (void) const
{
  GTL ("CSTREAMSOCK::Shutdown()");

  return shutdown (_fd, 2);
}
