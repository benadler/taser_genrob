//
// datagramsock.h -
// (C) 08/1999 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//


#ifndef _DATAGRAMSOCK_H
#define _DATAGRAMSOCK_H


#include "sock/sock.h"


class WINFLAGS CDATAGRAMSOCK : public _CSOCK
{
private:

protected:

public:

  CDATAGRAMSOCK (void);
  virtual ~CDATAGRAMSOCK (void);

  int Bind (unsigned short local_ipv4_port,
	    unsigned int local_ipv4_addr = 0);

  int Connect (const unsigned int rem_ipv4_addr,
	       const unsigned short rem_ipv4_port);

  int Recv (int maxSize,
	    void *buf,
	    unsigned int *remote_ipv4_addr = 0,
	    unsigned short *remote_ipv4_port = 0);

  int Send (int size,
	    void *buf,
	    unsigned int remote_ipv4_addr = 0,
	    unsigned short remote_ipv4_port = 0);
};


#endif // _DATAGRAMSOCK_H
