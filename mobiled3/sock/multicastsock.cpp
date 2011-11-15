//
// multicastsock.h -
// (C) 11/2001 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <stdlib.h>

#if defined (linux) || defined (__svr4__)
  #include <unistd.h>
  #include <netdb.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <sys/time.h>   // linux: fd_set
#elif defined (_WIN32)
  #include <io.h>
  #include <winsock2.h>
#elif defined (__WATCOMC__)
  #include <unistd.h>
  #include <netdb.h>
  #include <sys/select.h>   // QNX: fd_set
  #include <sys/socket.h>
  #include <netinet/in.h>
#else
  #error "operating system not supported!"
#endif

#include "sock/multicastsock.h"
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMULTICASTSOCK::CMULTICASTSOCK (void)
{
  GTL ("CMULTICASTSOCK::CMULTICASTSOCK()");

  //

  GTLPRINT (("fd = %i\n", _fd));

  //

  const char ttl = 1;

  if (setsockopt (_fd,
		  IPPROTO_IP,
		  IP_MULTICAST_TTL,
		  &ttl,
		  sizeof (ttl)))
    {
      GTLFATAL (("setsockopt(IP_MULTICAST_TTL) failed\n"));
      abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMULTICASTSOCK::~CMULTICASTSOCK (void)
{
  GTL ("CMULTICASTSOCK::~CMULTICASTSOCK()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMULTICASTSOCK::Bind (unsigned short local_ipv4_port,
			  unsigned int local_ipv4_addr)
{
  GTL ("CMULTICASTSOCK:Bind()");
  GTLPRINT (("addr=0x%08x, port=%i\n", local_ipv4_addr, local_ipv4_port));

  //

  int opt = 1;

  if (setsockopt (_fd,
		  SOL_SOCKET,
		  SO_REUSEADDR,
#if defined (linux) || defined (__svr4__) || defined (__WATCOMC__)
		  &opt,
#elif defined (_WIN32)
		  (char *)&opt,
#endif
		  sizeof (opt)) < 0)
    {
      GTLPRINT (("setsockopt(SO_REUSEADDR) failed\n"));
      return -1;
    }

  // set what multicasts we want to receive

  struct ip_mreq mreq;

  mreq.imr_interface.s_addr = INADDR_ANY;   // on any interface
  mreq.imr_multiaddr.s_addr = 
    local_ipv4_addr ? htonl (local_ipv4_addr) : INADDR_ANY;

  if (setsockopt (_fd,
		  IPPROTO_IP,
		  IP_ADD_MEMBERSHIP,
		  (char *)&mreq,
		  sizeof (mreq)) < 0)
    {
      GTLPRINT (("setsockopt(IP_ADD_MEMBERSHIP) failed\n"));
      return -1;
    }

  //
  // whether we want to receive a copy of sent datagrams
  //
  // ++itschere20011108: attention! this seems to affect the whole computer,
  // not just this socket!
  //

  const char yes = 1;

  if (setsockopt (_fd,
		  IPPROTO_IP,
		  IP_MULTICAST_LOOP,
		  &yes,
		  sizeof (yes)) < 0)
    {
      GTLPRINT (("setsockopt(IP_MULTICAST_LOOP) failed\n"));
      return -1;
    }

  //

  struct sockaddr_in local_addr;

#ifdef _WIN32
  local_addr.sin_family = AF_INET;
#endif
  local_addr.sin_addr.s_addr =
    local_ipv4_addr ? htonl (local_ipv4_addr) : INADDR_ANY;
  local_addr.sin_port = htons (local_ipv4_port);

  if (bind (_fd,
	    (struct sockaddr *)&local_addr,
	    sizeof (struct sockaddr)) < 0)
    {
      GTLPRINT (("bind() failed\n"));
      return -1;
    }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMULTICASTSOCK::Connect (const unsigned int rem_ipv4_addr,
			     const unsigned short rem_ipv4_port)
{
  GTL ("CMULTICASTSOCK::Connect()");
  GTLPRINT (("addr=0x%08x, port=%i\n", rem_ipv4_addr, rem_ipv4_port));

  // set what multicasts we want to receive from which interface cards

  struct ip_mreq mreq;

  mreq.imr_interface.s_addr = INADDR_ANY;
  mreq.imr_multiaddr.s_addr = 
    rem_ipv4_addr ? htonl (rem_ipv4_addr) : INADDR_ANY;

  if (setsockopt (_fd,
		  IPPROTO_IP,
		  IP_ADD_MEMBERSHIP,
		  (char *)&mreq,
		  sizeof (mreq)) < 0)
    {
      perror ("setsockopt(IP_ADD_MEMBERSHIP)");
      GTLPRINT (("setsockopt(IP_ADD_MEMBERSHIP) failed\n"));
      return -1;
    }

  // whether we want to receive a copy of sent datagrams

  const char yes = 1;

  if (setsockopt (_fd,
		  IPPROTO_IP,
		  IP_MULTICAST_LOOP,
		  &yes,
		  sizeof (yes)) < 0)
    {
      GTLPRINT (("setsockopt(IP_MULTICAST_LOOP) failed\n"));
      return -1;
    }

  //

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
