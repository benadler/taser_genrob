//
// status.h -
// (C) 11-12/1999 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//
// this file implements a class CSTATUS, an - as I think - comfortable and easy
// way to return status information from a function. it often happens that you
// want to return an integer indicating an error code AND, sometimes depending
// on the error, a string describing the situation in a more detailed way. if
// you do this by means of "char *" there're two troublesome possibilities:
//
// 1) either you let the function return "char *" and malloc() the buffer
// inside it. the user will typically forget to free() that buffer again and
// therefore turn this into a memory leak.
//
// 2) or you let the user supply a buffer by means of "char *buf, int
// buflen". this typically leads to that the buffer will be passed too small to
// hold the message.
//
// the class CSTATUS gets you rid of both of these problems by taking care by
// means of con- and destructors that the memory allocated for the string is
// not left orphaned. a function returning a status creates a CSTATUS
// object. if the caller doesn't care for it it is immediately destroyed again
// and all memory is free()d. if the user does care about it he/she must make a
// copy of it. the psychological argument is that in this case it's the user
// who has made the copy, so he knows about it and it's less likely that he/she
// will forget to delete it afterwards. furthermore we disallow CSTATUS objects
// to be created on the heap so due to lexical scoping they will eventually be
// destroyed at some point in the code (when leaving the associated `{}' pair).
//
// btw: everything is inline here, because it's so short'n'simple...
//


#ifndef _STATUS_H
#define _STATUS_H


#include <stdlib.h>
#include <string.h>


class CSTATUS
{
private:

  // not even implemented
  void *operator new (size_t);

  void Dummy (void);

protected:

  int _i;
  char *_s;

public:

  CSTATUS (void)
    {
      _i = 0;
      _s = 0x0;
    }

  CSTATUS (const int i, const char *s = 0x0)
    {
      _i = i;

      if (s)
	{
	  _s = ::strdup (s);
	}
      else
	{
	  _s = 0x0;
	}
    }

  CSTATUS (const CSTATUS &orig)
    {
      _i = orig._i;

      if (orig._s)
	{
	  _s = ::strdup (orig._s);
	}
      else
	{
	  _s = 0x0;
	}
    }

  ~CSTATUS (void)
    {
      Dummy ();

      if (_s)
	{
	  ::free (_s);
	  _s = 0x0;   // paranoia
	}
    }

  CSTATUS &operator= (const CSTATUS &right)
    {
      if (_s)
      {
	::free (_s);
      }

      _i = right._i;

      if (right._s)
	{
	  _s = ::strdup (right._s);
	}
      else
	{
	  _s = 0x0;
	}

      return *this;
    }

  operator char* (void) const   // this should better be `operator const char*'
    {
      if (_s)
	{
	  return _s;
	}
      else
	{
	  return "(null)";
	}
    }

  operator int (void) const
    {
      return _i;
    }
};


#endif   // #ifndef _STATUS_H
