//
// labdevd.h
// (C) 02/2000 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//


#ifndef _LABDEVD_H
#define _LABDEVD_H


#include "infobase/infobase.h"
#include "sock/streamsock.h"
#include "labdev/labmsg.h"


class CLABDEVD
{
private:

  // not even implemented
  CLABDEVD (void);
  CLABDEVD (const CLABDEVD &);
  CLABDEVD &operator= (const CLABDEVD &);

protected:

  CINFOBASE _ib;
  CSTREAMSOCK _sock;

  CLABDEVD (const char *name);

  virtual int Allow (unsigned int addr);
  virtual void Event (CLABMSG &msg) = 0;

public:

  virtual ~CLABDEVD (void);

  int Run (const int timeout);
};


#endif // _LABDEVD_H
