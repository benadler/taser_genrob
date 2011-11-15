//
//
//

#ifndef _DAEMON_H_
#define _DAEMON_H_

#include "sock/sock.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class WINFLAGS CCONNECTION
{
  friend class CDAEMON;

private:

  // not even implemented
  CCONNECTION (void);
  CCONNECTION (const CCONNECTION &);
  CCONNECTION &operator= (const CCONNECTION &);

protected:

  int _flags;
  _CSOCK *_sock;

  virtual void DataAvailable (CDAEMON *daemon) = 0x0;

public:

  CCONNECTION (_CSOCK *sock);
  virtual ~CCONNECTION (void);
};


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

typedef int milliseconds_t;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class WINFLAGS CDAEMON
{
private:

protected:

  int _numConnections;
  int _maxConnections;
  CCONNECTION **_connections;

  int _runCounter;

public:

  CDAEMON (void);
  virtual ~CDAEMON (void);

  int AddConnection (CCONNECTION *connection);
  int RemoveConnection (CCONNECTION *connection);

  int Run (const milliseconds_t timeout = -1);
};

#endif
