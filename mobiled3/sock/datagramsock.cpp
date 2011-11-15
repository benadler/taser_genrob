//
// datagramsock.cc -
// (C) 01/1999-04/2001 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
  #include <sys/ioctl.h>
#endif

#if defined (linux) || defined (__svr4__)
  #include <unistd.h>
  #include <netdb.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
//#include <sys/time.h>   // linux: fd_set
#elif defined (_WIN32)
  #include <io.h>
  #include <winsock2.h>
#elif defined (__WATCOMC__)
  #include <unistd.h>
  #include <netdb.h>
//#include <sys/select.h>   // QNX: fd_set
  #include <sys/socket.h>
  #include <netinet/in.h>
#else
  #error "operating system not supported!"
#endif


#include "sock/datagramsock.h"
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CDATAGRAMSOCK::CDATAGRAMSOCK (void) : _CSOCK (DATAGRAM)
{
  GTL ("CDATAGRAMSOCK::CDATAGRAMSOCK()");

  if ((_fd = ::socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      GTLFATAL (("socket() failed\n"));
      abort ();
    }

#if defined (linux) || defined (__WATCOMC__) || defined (_WIN32)

  // from some (to me) unknown kernel version on this seems to be necessary to
  // be able to send broadcasts. I don't know, I think I can remember times
  // when this was not necessary...

  int opt = 1;

#ifdef _WIN32
  if (setsockopt (_fd, SOL_SOCKET, SO_BROADCAST, (const char *)&opt, sizeof (opt)) < 0)
#else
  if (setsockopt (_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof (opt)) < 0)
#endif
    {
      GTLFATAL (("setsockopt(SO_BROADCAST) failed\n"));
      abort ();
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CDATAGRAMSOCK::~CDATAGRAMSOCK (void)
{
  GTL ("CDATAGRAMSOCK::~CDATAGRAMSOCK()");

  if (_fd >= 0)
    {
      ::close (_fd);
      _fd = -1;   // paranoia
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CDATAGRAMSOCK::Connect (const unsigned int rem_ipv4_addr,
			    const unsigned short rem_ipv4_port)
{
  GTL ("CDATAGRAMSOCK::Connect()");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons (rem_ipv4_port);
  addr.sin_addr.s_addr = htonl (rem_ipv4_addr);

  int ret;

  if ((ret = ::connect (_fd,
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

int CDATAGRAMSOCK::Bind (unsigned short local_ipv4_port,
			 unsigned int local_ipv4_addr)
{
  GTL ("CDATAGRAMSOCK:Bind()");

  int opt = 1;

#if defined (sparc) || defined (__GNUC__) || defined (__WATCOMC__)
  if (::setsockopt (_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) < 0)
#elif defined (_WIN32)
  if (::setsockopt (_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof (opt)) < 0)
#else
#error "operating system not supported!"
#endif
    {
      GTLPRINT (("setsockopt(SO_REUSEADDR) failed\n"));
    }

  //

  struct sockaddr_in local_addr;

  if (local_ipv4_addr)
    {
      local_addr.sin_addr.s_addr = htonl (local_ipv4_addr);
    }
  else
    {
      local_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    }

#ifdef _WIN32
  local_addr.sin_family = AF_INET;
#endif
  local_addr.sin_port = htons (local_ipv4_port);

  int ret = ::bind (_fd,
		   (struct sockaddr *)&local_addr,
		   sizeof (struct sockaddr));

  if (ret < 0)
    {
      GTLPRINT (("bind() failed\n"));
      return -1;
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CDATAGRAMSOCK::Recv (int maxSize,
			 void *buf,
			 unsigned int *remote_ipv4_addr,
			 unsigned short *remote_ipv4_port)
{
  GTL ("CDATAGRAMSOCK::Recv()");

  struct sockaddr_in remote_addr;
#if defined (__WATCOMC__) || defined (_WIN32)
  int fromLen = sizeof (struct sockaddr);
#else
  unsigned int fromLen = sizeof (struct sockaddr);
#endif

  int ret;

  if ((ret = ::recvfrom (_fd,
#ifdef _WIN32
			 (char *)buf,
#else
			 buf,
#endif
			 maxSize,
			 0,
			 (struct sockaddr *)&remote_addr,
			 &fromLen)) < 0)
    {
      GTLPRINT (("recvfrom() failed\n"));
    }

  if (remote_ipv4_addr)
    {
      *remote_ipv4_addr = ntohl (remote_addr.sin_addr.s_addr);
    }

  if (remote_ipv4_port)
    {
      *remote_ipv4_port = ntohs (remote_addr.sin_port);
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CDATAGRAMSOCK::Send (int size,
			 void *buf,
			 unsigned int remote_ipv4_addr,
			 unsigned short remote_ipv4_port)
{
  GTL ("CDATAGRAMSOCK::Send()");
  GTLPRINT (("addr=0x%08x, port=%i\n", remote_ipv4_addr, remote_ipv4_port));

  int ret;

#if defined (linux)

  // under linux you'll get an error when sending datagrams to ports that
  // don't exist. since other operating systems (solaris) do not do this you
  // cannot rely on this and the mechanism is therefore useless.
  //
  // to get around this we need to catch the error, for example with a loop.

  //int loops = 0;

  do
    {

#endif

      if (!remote_ipv4_addr && !remote_ipv4_port)
	{
	  ret = ::send (_fd,
#ifdef _WIN32
			(char *)buf,
#else
			buf,
#endif
			size,
			MSG_DONTROUTE);
	}
      else
	{
	  struct sockaddr_in remote_addr;
	  remote_addr.sin_family = AF_INET;
	  remote_addr.sin_addr.s_addr = htonl (remote_ipv4_addr);
	  remote_addr.sin_port = htons (remote_ipv4_port);

	  ret = sendto (_fd,
#ifdef _WIN32
			(char *)buf,
#else
			buf,
#endif
			size,
#if 1
			0,
#else
			MSG_DONTROUTE,   // ++itschere20011203: bad idea... :-(
#endif
			(struct sockaddr *)&remote_addr,
			sizeof (struct sockaddr));

	  GTLPRINT (("sendto() -> %i, errno=%i\n", ret, errno));
	}

#if defined (linux)

    } while ((ret == -1) &&
	     (errno == 101)   // ENETUNREACH
	     //(errno == ECONNREFUSED)
	     );

#endif

  return ret;
}
