//
// voltageDisplayThread.h
//

#ifndef _VOLTAGEDISPLAYTHREAD_H_
#define _VOLTAGEDISPLAYTHREAD_H_

#include "thread/thread.h"

#include "mobile/motorFeeder.h"


class CVOLTAGEDISPLAYTHREAD : protected CDETACHEDTHREAD
{
private:

  // not even implemented
  CVOLTAGEDISPLAYTHREAD (const CVOLTAGEDISPLAYTHREAD &);
  CVOLTAGEDISPLAYTHREAD &operator= (const CVOLTAGEDISPLAYTHREAD &);
  bool &operator== (const CVOLTAGEDISPLAYTHREAD &);

protected:

  CMOTORFEEDER *_motorFeeder;

  virtual void Fxn (void);

public:

  CVOLTAGEDISPLAYTHREAD (CMOTORFEEDER *motorFeeder)
  {
    _motorFeeder = motorFeeder;

    Start ();
  }

  virtual ~CVOLTAGEDISPLAYTHREAD (void);
};


#endif
