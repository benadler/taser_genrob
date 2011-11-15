//
// sock.cc -
// (C) 01/1999-04/2001 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#if defined (__GNUC__)
  #include <unistd.h>
  #include <netdb.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <string.h>
#elif defined (__WATCOMC__)
  #include <unistd.h>
  #include <netdb.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <sys/select.h>
  #include <unix.h>   // gethostname()
#elif defined (_WIN32)
  #include <io.h>
  #include <winsock2.h>
#else
  #error "operating system not supported!"
#endif

#include "sock/sock.h"
#include "sock/datagramsock.h"
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CSOCK::~_CSOCK (void)
{
  GTL ("_CSOCK::~_CSOCK()");

  if (_daemon)
    {
      GTLFATAL (("still on CDAEMON list\n"));
      abort ();
    }

  if (_fd >= 0)
    {
#ifdef _WIN32
      closesocket (_fd);
#else
      close (_fd);
#endif
      _fd = -1;   // paranoia
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static unsigned int globLocalAddr = 0;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

class CSTARTER
{
public:

  CSTARTER (void);
};


CSTARTER::CSTARTER (void)
{
  // winsock initialization code

  WSADATA m_WSAData;

  // Anmeldung bei Socket DLL

  switch (WSAStartup (MAKEWORD (2, 0), &m_WSAData))    // MAKEWORD(1, 1)
    {
    case 0:
      break;  //Richtige Version

    case WSASYSNOTREADY:
      fprintf (stderr, "winsock not ready\n");
      abort ();

    case WSAVERNOTSUPPORTED:
      fprintf (stderr, "winsock version not supported\n");
      abort ();

    case WSAEINPROGRESS:
      fprintf (stderr, "winsock is busy\n");
      abort ();

    case WSAEPROCLIM:
      fprintf (stderr, "winsock task limit reached\n");
      abort ();

    default:
      fprintf (stderr, "winsock unknown error\n");
      abort ();
    }

  // Ausgabe der Beschreibung von verwendeten Winsock DLL

  // fprintf (stderr, "%s\n", m_WSAData.szDescription);
}

static CSTARTER getLocalAddr;

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int _CSOCK::GetLocalAddr (unsigned short *portPtr, unsigned int *addrPtr)
{
  GTL ("_CSOCK::GetLocalAddr()");

  //

  struct sockaddr_in local_addr;

#if defined (__WATCOMC__) || defined (_WIN32)
  int len;
#else
  unsigned int len;
#endif

  //

  len = sizeof (struct sockaddr);

  int err = ::getsockname (_fd, (struct sockaddr *)&local_addr, &len);

  unsigned short port = ntohs (local_addr.sin_port);
#if 1

  if (!globLocalAddr)   // ++itschere20021126: avoid global object
    {
      char hostName[128];

      if (!gethostname (hostName, 128))
	{
	  struct hostent *he;

	  if ((he = gethostbyname (hostName)))
	    {
	      globLocalAddr = ntohl (*(unsigned int *)he->h_addr_list[0]);
	    }
	  else
	    {
	      GTLFATAL (("gethostbyname() failed\n"));
	    }
	}
      else
	{
	  GTLFATAL (("gethostname() failed\n"));
	}

      // printf ("hostname = %s\n", hostName);

      if (!globLocalAddr)
	{
	  GTLFATAL (("couldn't determine local ip address, expect problems\n"));
	}

      // printf ("local ip address = %08x\n", globLocalAddr);
    }

  unsigned int addr = globLocalAddr;

#else

  unsigned int addr = ntohl (local_addr.sin_addr.s_addr);

#endif

#ifdef _WIN32

  if (err < 0)   // under w2k getsockname() fails for an unbound socket...
    {
      port = 0;
    }

#else

  // ...whereas under everything else it just reports 0.0.0.0:0 as address.

  if (err < 0)
    {
      GTLPRINT (("getsockname(#2) failed\n"));
      return -1;
    }

#endif

  GTLPRINT (("%08x:%i\n", addr, port));

  //

  if (portPtr)
    {
      *portPtr = port;
    }

  //

  if (addrPtr)
    {
      *addrPtr = addr;
    }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int _CSOCK::WaitIn (const milliseconds_t ms) const
{
  fd_set rfd;

  FD_ZERO (&rfd);
  FD_SET (_fd, &rfd);

  struct timeval tv = {ms / 1000, (ms % 1000) * 1000}, *tvp;

  if (ms < 0)
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


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int _CSOCK::WaitOut (const milliseconds_t ms) const
{
  fd_set wfd;

  FD_ZERO (&wfd);
  FD_SET (_fd, &wfd);

  struct timeval tv = {ms / 1000, (ms % 1000) * 1000}, *tvp;

  if (ms < 0)
    {
      tvp = 0x0;
    }
  else
    {
      tvp = &tv;
    }

  // this either returns a negative error, 0 if no data is available or +1 if
  // data is available

  return select (_fd + 1, 0x0, &wfd, 0x0, tvp);
}
