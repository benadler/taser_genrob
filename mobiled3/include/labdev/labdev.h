//
// labdev.h
// (C) 02/2000 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//


#ifndef _LABDEV_H
#define _LABDEV_H


#include "infobase/infobase.h"
#include "labdev/labmsg.h"


class CLABDEV
{
private:

  // not even implemented
  CLABDEV (void);
  CLABDEV (const CLABDEV &);
  CLABDEV &operator= (const CLABDEV &);

protected:

  char *_name;

  CINFOBASE *_ib;

  CLABDEV (const char *name);   // not meant for public construction

  int RequestService (CLABMSG &msg);

public:

  virtual ~CLABDEV (void);
};


#endif // _LABDEV_H
