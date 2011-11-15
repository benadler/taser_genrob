//
//
//

#ifndef _PERIODICTIMER_H_
#define _PERIODICTIMER_H_

#include "thread/thread.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CPERIODICTIMER : protected CTHREAD
{
private:

  // not even implemented

  CPERIODICTIMER (const CPERIODICTIMER &);
  CPERIODICTIMER &operator= (const CPERIODICTIMER &);
  bool operator== (const CPERIODICTIMER &);

protected:

  bool _realtime;
  int _fd;
  virtual void Fxn (void);

  virtual void Event (void) = 0x0;

public:

  CPERIODICTIMER (const bool realtime = false);
  virtual ~CPERIODICTIMER (void);
};

#endif
