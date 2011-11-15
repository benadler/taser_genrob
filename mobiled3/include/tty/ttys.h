//
//
//

#ifndef _TTYS_H_
#define _TTYS_H_

#include "tty.h"


class CTTYS : public CTTY
{
private:

protected:

#ifdef _WIN32

  HANDLE _event;   // for asynchronous i/o

#endif

public:

  // constructors /////////////////////////////////////////////////////////////

  CTTYS (const char *dev);

  // destructor ///////////////////////////////////////////////////////////////

#ifdef _WIN32

  virtual ~CTTYS (void);

#endif

  //

  int SetSpeed (const int newSpeed);

  int SetSize (const int newSize);

  int SetStopBits (const int newBits);

  typedef enum
  {
    NONE,
    ODD,
    EVEN
  } PARITY;

  int SetParity (const PARITY parity = NONE);

  int SetCtsRts (const int newCtsRts);

  //

#ifdef _WIN32

  virtual int Read (void *ptr, const int size) const;
  virtual int Write (const void * const ptr, const int size) const;

#else

  void SetLowLatency (void);
  int SetVmin (const int vmin);
  int SetVtime (const int vtime);

#endif
};

#endif
