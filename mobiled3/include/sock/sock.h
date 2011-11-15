//
// sock.h -
// (C) 08/1999 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#ifndef _SOCK_H_
#define _SOCK_H_


#include "windefs.h"
#include "iotypes.h"


class WINFLAGS _CSOCK
{
  friend class CDAEMON;
#if 1
  friend class CSELECTOR;   // ++itschere20011101: needed by infobase
#endif

private:

  // not even implemented
  _CSOCK (const _CSOCK &);
  void operator= (const _CSOCK &);

  CDAEMON *_daemon;

protected:

  int _fd;

  typedef enum
  {
    STREAM,
    DATAGRAM
  } ESOCKTYPE;

  ESOCKTYPE _type;

  _CSOCK (const ESOCKTYPE type)
  {
    _type = type;
    _daemon = 0x0;
  };

public:

  virtual ~_CSOCK (void);

  int GetLocalAddr (unsigned short *portPtr, unsigned int *addrPtr);
  //int GetRemoteAddr (unsigned short *portPtr, unsigned int *addrPtr);
  
  int WaitIn (const milliseconds_t timeout) const;
  int WaitOut (const milliseconds_t timeout) const;
};


#endif // _SOCK_H
