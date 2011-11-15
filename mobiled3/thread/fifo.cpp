//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "thread/fifo.h"


#if HAS_THREADS

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CFIFOMSG::~CFIFOMSG (void)
{
  static const char * const fxnName = "CFIFOMSG::~CFIFOMSG() -- ";

  if (_fifo)
    {
      ::fprintf (stderr, "%smessage still on fifo\n", fxnName);
      ::abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CFIFO::~CFIFO (void)
{
  static const char * const fxnName = "CFIFO::~CFIFO() -- ";

  if (_head)
    {
      ::fprintf (stderr, "%sfifo not empty\n", fxnName);
      ::abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CFIFO::Push (CFIFOMSG *msg)
{
  static const char * const fxnName = "CFIFO::Push() -- ";

  if (msg->_fifo)
    {
      ::fprintf (stderr, "%smessage already on a fifo\n", fxnName);
      ::abort ();
    }

  _condition.Lock ();

  msg->_fifo = this;

  if (_tail)
    {
      _tail->_next = msg;
      msg->_prev = _tail;
      msg->_next = 0x0;
      _tail = msg;
    }
  else
    {
      msg->_prev = msg->_next = 0x0;
      _head = _tail = msg;
    }

  static int maxCount = 0;

  _count++;

  if (_count > maxCount)
    {
      maxCount = _count;
      ::printf ("%smaxCount = %i\n", fxnName, maxCount);
    }

  _condition.Broadcast ();

  _condition.Unlock ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CFIFOMSG *CFIFO::Pop (void)
{
  // static const char * const fxnName = "CFIFO::Pop() -- ";

  _condition.Lock ();

  while (!(volatile CFIFOMSG *)_head)
    {
      _condition.Wait ();
    }

  CFIFOMSG *ret = _head;

  if (_head->_next)
    {
      _head->_next->_prev = 0x0;
    }
  else
    {
      _tail = 0x0;
    }

  _head = _head->_next;

  ret->_fifo = 0x0;
  ret->_prev = ret->_next = 0x0;

  _count--;

  _condition.Unlock ();

  return ret;
}

#endif // HAS_THREADS
