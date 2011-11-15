//
//
//

#ifndef _RS422_H_
#define _RS422_H_


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#undef USE_RTLINUX


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "tty/ttys.h"

class CRS422
{
private:

  // not even implemented
  CRS422 (void);
  CRS422 (const CRS422 &);
  CRS422 &operator= (const CRS422 &);
  CRS422 &operator== (const CRS422 &);

protected:

#ifdef USE_RTLINUX
  int _fd;
#else
  CTTYS *_ttys;
#endif

  CRS422 (const int idx);

public:

  virtual ~CRS422 (void);

  void SetSpeed (const int newSpeed);

  int Read (void *ptr, const int size) const;
  int ReadTimeout (void *ptr, const int size, const int seconds) const;

  int Write (const void *ptr, const int size) const;

  void FlushInput (void) const;

  void Mark (void) const;
};

#endif
