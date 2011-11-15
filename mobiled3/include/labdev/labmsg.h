//
// labmsg.h
// (C) 03/2000 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//


#ifndef _LABMSG_H
#define _LABMSG_H


#include "netobj/netobj.h"


class CLABMSG : public CNETOBJ
{
private:

  // not even implemented

  CLABMSG (const CLABMSG &);
  CLABMSG &operator= (const CLABMSG &);

protected:

public:

  // all constructors/descructors/copyconstructors default

  CLABMSG (void);
  virtual ~CLABMSG (void);

  int _cmd;
  int _intData[8];
  float _floatData[8];
#define MAXSTRINGDATA 4
  char *_stringData[MAXSTRINGDATA];

  int _auxDataSize;
  void *_auxData;

  NETOBJ_DECLARE (CLABMSG);
};


#endif // _LABMSG_H
