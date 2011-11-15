//
// multicastsock.h -
// (C) 11/2001 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//


#ifndef _MULTICASTSOCK_H
#define _MULTICASTSOCK_H


#include "sock/datagramsock.h"


class CMULTICASTSOCK : public CDATAGRAMSOCK
{
private:

protected:

public:

  CMULTICASTSOCK (void);
  virtual ~CMULTICASTSOCK (void);

  int Bind (unsigned short local_ipv4_port,
	    unsigned int local_ipv4_addr = 0);

  int Connect (const unsigned int rem_ipv4_addr,
	       const unsigned short rem_ipv4_port);
};


#endif // _MULTICASTSOCK_H
