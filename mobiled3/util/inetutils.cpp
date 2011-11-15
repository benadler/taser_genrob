//
//
//

#include <stdio.h>

#ifdef __WATCOMC__
#include <unix.h>
#endif

#ifdef _WIN32
  #include <winsock2.h>
#else
  #include <netdb.h>
  #include <unistd.h>
  #include <netinet/in.h>
#endif

#include "sock/datagramsock.h"
#include "util/inetutils.h"


//
//
//

char *_gethostname (void)
{
  static char buf[256];
  static int initialized = 0;

  if (initialized)
    {
      return buf;
    }

  if (::gethostname (buf, 256) < 0)
    {
      return 0x0;
    }

  initialized = 1;

  return buf;
}


//
//
//

unsigned int _gethostaddr (void)
{
  static unsigned int addr = 0;

  if (addr)
    {
      return addr;
    }

  addr = _gethostbyname (_gethostname ());

  if (!addr)
  {
    fprintf (stderr, "_gethostaddr(): can't resolve local address, trying 127.0.0.1\n");
    addr = 0x7f000001;
  }

  return addr;
}


//
//
//

unsigned int _getbroadcastaddr (void)
{
  static unsigned int addr = 0;

#ifdef __GNUC__
#warning "determine real broadcast adddress / network mask"
#endif

  if (!addr)
    {
      addr = _gethostaddr ();

      switch (addr & 0xffffff00)
	{
	case 0x7f000000:
	  break;
#if 1
	case 0x81468A00:
	  // 129.70.138.22 (dyn-laptop-19) needs .63 as broadcast address. pk
	  // wants it this way and I've got no portable way to determine the
	  // netmask.
	  addr |= 0x0000003f;
	  break;

	case 0x81468B00:
	  // 129.70.139.43 (lassalle) needs .127 as broadcast address. pk wants
	  // it this way and I've got no portable way to determine the netmask.
	  addr |= 0x0000007f;
	  break;
#endif
	default:
	  addr |= 0x000000ff;   // *.255 is valid broadcast for class c subnet
	}
    }

  return addr;
}


//
//
//

unsigned int _gethostbyname (const char *hostName)
{
  struct hostent *he;

#ifdef __WATCOMC__
  if (!(he = ::gethostbyname ((char *)hostName)))
#else
  if (!(he = ::gethostbyname (hostName)))
#endif
    {
#ifdef _WIN32
      int err = WSAGetLastError ();

      switch (err)
	{
	case WSANOTINITIALISED:
	  fprintf (stderr, "NOTINITIALISED\n");
	  break;
	case WSAENETDOWN:
	  fprintf (stderr, "ENETDOWN\n");
	  break;
	case WSAHOST_NOT_FOUND:
	  fprintf (stderr, "HOST_NOT_FOUND\n");
	  break;
	case WSATRY_AGAIN:
	  fprintf (stderr, "TRY_AGAIN\n");
	  break;
	case WSANO_RECOVERY:
	  fprintf (stderr, "NO_RECOVERY\n");
	  break;
	case WSANO_DATA:
	  fprintf (stderr, "NO_DATA\n");
	  break;
	case WSAEINPROGRESS:
	  fprintf (stderr, "EINPROGRESS\n");
	  break;
	case WSAEFAULT:
	  fprintf (stderr, "EFAULT\n");
	  break;
	case WSAEINTR:
	  fprintf (stderr, "EINTR\n");
	  break;
	default:
	  fprintf (stderr, "unknown error: %i\n", err);
	}

#endif
	  
      perror ("gethostbyname()");
      return 0;
    }

  return ntohl (*(unsigned int *)he->h_addr_list[0]);
}


//
//
//

char *_inet_n2a (const unsigned int addr)
{
  static char buf[256];

  sprintf (buf,
	   "%i.%i.%i.%i",
	   (addr & 0xff000000) >> 24,
	   (addr & 0x00ff0000) >> 16,
	   (addr & 0x0000ff00) >> 8,
	   (addr & 0x000000ff));

  return buf;
}


//
//
//

char *_inet_n2a (const unsigned int addr, char *buf, const int bufSize)
{
#ifdef __GNUC__
#warning "use snprintf()"
#endif

  sprintf (buf,
	   "%i.%i.%i.%i",
	   (addr & 0xff000000) >> 24,
	   (addr & 0x00ff0000) >> 16,
	   (addr & 0x0000ff00) >> 8,
	   (addr & 0x000000ff));

  return buf;
}
