//
//
//

#ifndef _FIFO_H_
#define _FIFO_H_


#include "thread/condition.h"


#if HAS_THREADS

///////////////////////////////////////////////////////////////////////////////
//
// CFIFOs are basically cut down CLISTs: a CFIFOMSG can only be on one list
// (fifo) at a time and you cannot freely browse the list (fifo).
//
///////////////////////////////////////////////////////////////////////////////


class WINFLAGS CFIFOMSG
{
  friend class CFIFO;

private:

  // not even implemented
  CFIFOMSG (const CFIFOMSG &);
  CFIFOMSG &operator= (const CFIFOMSG &);

protected:

  CFIFO *_fifo;
  CFIFOMSG *_prev, *_next;

public:

  CFIFOMSG (void)
  {
    _fifo = 0x0;
    _prev = _next = 0x0;
  }

  virtual ~CFIFOMSG (void);
};


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class WINFLAGS CFIFO
{
private:

  // not even implemented
  CFIFO (const CFIFO &);
  CFIFO &operator= (const CFIFO &);

protected:

  CCONDITION _condition;
  CFIFOMSG *_head, *_tail;
  int _count;

public:

  CFIFO (void)
  {
    _count = 0;
    _head = _tail = 0x0;
  }

  virtual ~CFIFO (void);

  //

  void Push (CFIFOMSG *msg);

  CFIFOMSG *Pop (void);
};

#endif // HAS_THREADS

#endif
