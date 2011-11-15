//
//
//

#ifndef _TRACELOG_H_
#define _TRACELOG_H_


#include "windefs.h"
#include "thread/thread.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class WINFLAGS _CGTL1
{
private:

  // not even implemented
  void *operator new (const size_t size);
  _CGTL1 (void);
  _CGTL1 (const _CGTL1 &);
  _CGTL1 operator= (const _CGTL1 &);

  THREADARGS *_threadArgs;
  const void *_addr;
  const char *_fxn;

public:

  _CGTL1 (const void *addr, const char *fxnName);

  // qnx bug (watcom c16 v10.6): if the inline destructor (any inline
  // function?) contains only code conditionalized by means of preprocessor
  // defines (only this special define?) then NO code is generated AT ALL,
  // regardless of whether the condition is true or false. there has to be at
  // least some dummy code around (here: cleaning a variable) to force
  // generation of the code inside the condition.

  // no, it's more like it sucks on inline functions in general... :-(

  virtual ~_CGTL1 (void);

  void Print (const char *fmt, ...);
};


///////////////////////////////////////////////////////////////////////////////
//
// _CGTL2 is for those part with `#undef DEBUG'. it only stores function name
// and thread information but prints nothing, with the exception of fatal
// errors which are still printed.
//
///////////////////////////////////////////////////////////////////////////////

class WINFLAGS _CGTL2
{
private:

  // not even implemented
  void *operator new (const size_t size);
  _CGTL2 (void);
  _CGTL2 (const _CGTL2 &);
  _CGTL2 operator= (const _CGTL2 &);

  const void *_addr;
  const char *_fxn;

public:

  _CGTL2 (const void *addr, const char *fxnName);

  virtual ~_CGTL2 (void);

  void Print (const char *fmt, ...);
};


extern WINFLAGS int _globGtlEnable;

#ifdef DEBUG

#define GTL(fxnName) _CGTL1 _gtl (this, fxnName);
#define GTLC(fxnName) _CGTL1 _gtl (0x0, fxnName);
#define GTLPRINT(args) if (_globGtlEnable) _gtl.Print args;
#define GTLFATAL(args) _gtl.Print args

#else

#define GTL(fxnName) _CGTL2 _gtl (this, fxnName);
#define GTLC(fxnName) _CGTL2 _gtl (0x0, fxnName);
#define GTLPRINT(args)
#define GTLFATAL(args) _gtl.Print args

#endif


extern void GTLCOUNT (unsigned char channel, const char *comment);

#endif
