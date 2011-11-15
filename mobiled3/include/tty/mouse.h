//
// mouse.h
// (C) 12/2001 by TeSche (Torsten Scherer)
// itschere@techfak.uni-bielefeld.de
//

#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "tty/ttys.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CMOUSE : protected CTTYS
{
  typedef int milliseconds_t;

private:

  // not even implemented

  CMOUSE (void);
  CMOUSE (const CMOUSE &);
  CMOUSE &operator= (const CMOUSE &);

protected:

public:

  CMOUSE (const char *devName);
  virtual ~CMOUSE (void);

  void GetEvent (int &buttons, int &dx, int &dy);
  int GetEvent (int &buttons, int &dx, int &dy, const milliseconds_t timeout);
};


#endif
