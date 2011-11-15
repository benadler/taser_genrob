//  spawner.cc
//  (c) 06/20006, Torsten Scherer, Daniel Westhoff
//  westhoff@informatik.uni-hamburg.de

// Die folgenden drei Zeilen einkommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
//#ifdef DEBUG
//#undef DEBUG
//#endif

#include "spawner.h"
#include "client.h"

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

CSPAWNER::CSPAWNER (CSTREAMSOCK *sock,
		    CGENBASE *genBase)
  : CCONNECTION (sock)
{
  GTL ("CSPAWNER::CSPAWNER()");

  _genBase = genBase;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

CSPAWNER::~CSPAWNER (void)
{
  GTL ("CSPAWNER::~CSPAWNER()");
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

void CSPAWNER::DataAvailable (CDAEMON *daemon)
{
  GTL ("CSPAWNER::DataAvailable()");

  CSTREAMSOCK *sock = (CSTREAMSOCK *)_sock;
  unsigned int rem_ipv4_addr;
  unsigned short rem_ipv4_port;

  CSTREAMSOCK *newSock = sock->Accept (&rem_ipv4_addr,
				       &rem_ipv4_port);

  if (!newSock)
    {
      GTLPRINT (("Accept() failed\n"));
      return;
    }

  GTLPRINT (("connection from %i.%i.%i.%i:%i\n",
	     (rem_ipv4_addr & 0xff000000) >> 24,
	     (rem_ipv4_addr & 0xff0000) >> 16,
	     (rem_ipv4_addr & 0xff00) >> 8,
	     rem_ipv4_addr & 0xff,
	     rem_ipv4_port));

  // establish connection

  CCLIENT *newClient = new CCLIENT (newSock, _genBase);

  if (!newClient)
    {
      GTLPRINT (("failed to create new client\n"));
      delete newSock;   // probably...
      return;
    }

  daemon->AddConnection (newClient);
}
