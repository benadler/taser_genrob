//
//
//

#if !defined (_KBD_H_) && !defined (_WIN32)
#define _KBD_H_


#include "tty/tty.h"
#include "iotypes.h"


class CKBD : public CTTY
{
private:

  // not even implemented
  CKBD (const CKBD &);
  CKBD operator= (const CKBD &);

protected:

public:

  CKBD (void);
  virtual ~CKBD (void);

  int WaitIn (const milliseconds_t timeout) const;

  unsigned long GetKey (const milliseconds_t timeout = -1) const;
};


#endif
