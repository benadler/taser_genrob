//
// mobileTerm.cc
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util/inetutils.h"
#include "sock/streamsock.h"
#include "thread/thread.h"
#include "thread/tracelog.h"
#include "tty/kbd.h"

#include <unistd.h>
#include <sys/time.h>
#include <termios.h>


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

class KEYBOARD : public CTHREAD
{
protected:

  virtual void Fxn (void);

  CSTREAMSOCK *_sock;

public:

  KEYBOARD (CSTREAMSOCK *sock)
  {
    _sock = sock;

    Start ();
  }
};


//
//
//

void KEYBOARD::Fxn (void)
{
  ConfirmRunning();

#if 1

  char cmd[80];

  while (fgets (cmd, 80, stdin))
    {
      int len = strlen (cmd);

      if (len-- >= 0)
	{
	  cmd[len] = 0;   // cut off trailing '\n'

	  _sock->Write (len+1, cmd);
	}
    }

  abort ();

#else

  CKBD kbd;

  while (42)
    {
      unsigned char c = kbd.GetKey ();

      if (c == 13)   // RETURN
	{
	  write (fileno (stdout), "\n", 1);
	  c = 0;
	}

      _sock->Write (1, &c);
    }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

class SOCKET : public CTHREAD
{
protected:

  virtual void Fxn (void);

  CSTREAMSOCK *_sock;

public:

  SOCKET (CSTREAMSOCK *sock)
  {
    _sock = sock;

    Start ();
  }
};


//
//
//

void SOCKET::Fxn (void)
{
  ConfirmRunning();

  _sock->Write (5, "mode\000");

  while (42)
    {
      int ret;
      char buf[256];

      if ((ret = _sock->ReadSome (256, buf)) < 1)
	{
	  abort ();
	}

      write (fileno (stdout), buf, ret);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static int Term (const char * const hostName)
{
  //

  unsigned int addr = _gethostbyname (hostName);

  if (!addr)
    {
      fprintf (stderr,
	       "error: couldn't resolve host name \"%s\"\n",
	       hostName);
      return -1;
    }

  CSTREAMSOCK sock;

  if (sock.Connect (addr, 9001) < 0)
    {
      fprintf (stderr,
	       "error: couldn't connect() to host %s:9001\n",
	       hostName);
      return -1;
    }

  //

  KEYBOARD keyboard (&sock);

  SOCKET socket (&sock);

  while (42)
    {
      sleep (1);
    }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void usage (void)
{
  fprintf (stderr, "usage: genTerm [-d] [-m <host>]\n");
  fprintf (stderr, "\twhere:\n");
  fprintf (stderr, "\t-h : print this help\n");
  fprintf (stderr, "\t-d : enable debugging\n");
  fprintf (stderr, "\t-m <host> : connect to mobile <host> instead of default\n");

  exit (-1);
}


//
//
//

int main (int argc, char **argv)
{
  //

  int c;
  char *host = "localhost";

  while ((c = getopt (argc, argv, "dhm:")) >= 0)
    {
      switch (c)
	{
	case 'd':
	  _globGtlEnable = 1;
	  break;

	case 'm':
	  host = optarg;
	  break;

	case '?':
	case 'h':
	default:
	  usage ();
	}
    }


  //

  return Term (host);
}
