//
//
//

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sock/daemon.h"

#if defined (linux) || defined (sparc)
  #include <sys/time.h>   // linux: fd_set
#elif defined (__WATCOMC__)
  #include <sys/select.h>   // QNX: fd_set
#elif defined (_WIN32)
  #include <winsock2.h>
#else
  #error "operating system not supported"
#endif

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

CCONNECTION::CCONNECTION (_CSOCK *sock)
{
  _flags = 0;
  _sock = sock;
}


//
//
//

CCONNECTION::~CCONNECTION (void)
{
  delete _sock;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

CDAEMON::CDAEMON (void)
{
  GTL ("CDAEMON::CDAEMON()");

  _numConnections = 0;
  _maxConnections = 0;
  _connections = 0x0;

  _runCounter = 0;
}


//
//
//

CDAEMON::~CDAEMON (void)
{
  GTL ("CDAEMON::~CDAEMON()");

  if (_numConnections)
    {
      GTLFATAL (("not empty\n"));
      abort ();
    }
}


//
//
//

int CDAEMON::AddConnection (CCONNECTION *connection)
{
  GTL ("CDAEMON::AddConnection()");
  GTLPRINT (("connection=%p\n", connection));

  int idx = _numConnections++;

  if (_numConnections > _maxConnections)
    {
      _maxConnections += 1;   // or something bigger

      const int size = _maxConnections * sizeof (CCONNECTION *);

      if (!(_connections = (CCONNECTION **)realloc (_connections, size)))
	{
	  GTLFATAL (("out of memory\n"));
	  abort ();
	}
    }

  _connections[idx] = connection;

  return 0;
}


//
//
//

int CDAEMON::RemoveConnection (CCONNECTION *connection)
{
  GTL ("CDAEMON::RemoveConnection()");
  GTLPRINT (("connection=%p\n", connection));

  //

  int i;

  for (i=0; i<_numConnections; i++)
    {
      if (_connections[i] == connection)
	{
	  break;
	}
    }

  if (i == _numConnections)
    {
      GTLFATAL (("unknown connection\n"));
      abort ();
    }

  //

  memmove (_connections + i,
	   _connections + i + 1,
	   (_numConnections - (i + 1)) * sizeof (CCONNECTION *));

  _numConnections--;

  //

  return 0;
}


//
//
//

int CDAEMON::Run (const milliseconds_t timeout)
{
  GTL ("CDAEMON::Run()");

  //

  if (++_runCounter > 1)
    {
      GTLFATAL (("recursive call\n"));
      abort ();
    }

  // set up data

  fd_set rfd;

  FD_ZERO (&rfd);

  int i, maxFd = 0;

  for (i=0; i<_numConnections; i++)
    {
      int fd = _connections[i]->_sock->_fd;

      FD_SET (fd, &rfd);

      if (fd > maxFd)
	{
	  maxFd = fd;
	}
    }

  struct timeval tv = {timeout / 1000, (timeout % 1000) * 1000}, *tvp;

  if (timeout < 0)
    {
      tvp = 0x0;
    }
  else
    {
      tvp = &tv;
    }

  // do select

  int ret;

  do
    {
      ret = select (maxFd + 1, &rfd, 0x0, 0x0, tvp);
    }
  while ((ret < 0) && (errno == EINTR));   // restart upon signals

  // check for errors

  if (ret < 0)
    {
      GTLPRINT (("select() failed\n"));
      _runCounter--;
      return -1;
    }

  if (!ret)
    {
      _runCounter--;
      return 0;   // timeout expired without anything to do
    }

  // call callback functions to read/process data. these functions may in turn
  // add or remove arbitrary connections from the `list', which complicates
  // things a little bit. we solve this by going through the list in two steps:
  // first mark all connections to be served, and then serve them in a loop
  // searching a marked connection starting from the first one over and over
  // again until no more marked connections are found. right now this seems to
  // be the easiest solution.

  for (i=0; i<_numConnections; i++)
    {
      if (FD_ISSET (_connections[i]->_sock->_fd, &rfd))
	{
	  _connections[i]->_flags = 1;
	}
    }

  int idx = 0;

  while (idx < _numConnections)
    {
      if (_connections[idx]->_flags)
	{
	  _connections[idx]->_flags = 0;

	  _connections[idx]->DataAvailable (this);

	  // restart the whole list

	  idx = 0;

	  continue;
	}

      idx++;
    }

  //

  _runCounter--;

  return ret;
}
