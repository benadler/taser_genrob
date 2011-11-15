//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "util/linlist.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
// you should better unlink() an item before deleting it, but just in case
// you've forgotten we'll do this for you...
//

CLINLISTITEM::~CLINLISTITEM (void)
{
  if (_list)
    {
      _list->Unlink (this);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
// cannot delete() items because we don't know if they weren't create on the
// stack. instead it abort()s.
//

CLINLIST::~CLINLIST (void)
{
#if 1

  if (_head)
    {
      printf ("error: deleting list that is not empty!\n");
      abort ();
    }

#else

  WindToLast ();

  CLINLISTITEM *item;

  while ((item = GetPrev ()))
    {
      delete item;   // will unlink() it first
    }

#endif
}


//
//
//

void CLINLIST::Dump (void) const
{
  printf ("\t(%p,%p)\n", (void*)_head, (void*)_tail);

  CLINLISTITEM *item = _head;

  while (item)
    {
      printf ("\t %p (%p,%p)\n", (void*)item, (void*)item->_prev, (void*)item->_next);

      item = item->_next;
    }
}


//
//
//

CLINLISTITEM *CLINLIST::operator[] (int idx) const
{
  if ((idx < 0) || (idx >= _numElems))
    {
      return 0x0;
    }

  CLINLISTITEM *item;

  if (idx < (_numElems >> 1))   // search from head or tail?
    {
      item = _head;

      while (--idx >= 0)
	{
	  item = item->_next;
	}
    }
  else
    {
      item = _tail;
      idx = _numElems - idx - 1;

      while (--idx >= 0)
	{
	  item = item->_prev;
	}
    }

  return item;
}


//
//
//

void CLINLIST::Append (CLINLISTITEM *item)
{
  //cout << __PRETTY_FUNCTION__ << endl;

  if (item->_list == this)
    {
      printf ("item already on this list\n");
      abort ();
    }

  if (item->_list)
    {
      printf ("item already on a list\n");
      abort ();
    }

  item->_list = this;

  item->_next = 0x0;

  if (!(item->_prev = _tail))
    {
      _head = item;
    }
  else
    {
      _tail->_next = item;
    }

  _tail = item;

  _numElems++;
}


//
//
//

void CLINLIST::Unlink (CLINLISTITEM *item)
{
  //cout << __PRETTY_FUNCTION__ << endl;

  if (item->_list != this)
    {
      printf ("item not on this list\n");
      abort ();
    }

  //

#ifdef __GNUC__
#warning "Unlink() only safe for GetPrev()!!!"
#endif

  if (item == _curr)
    {
      _curr = item->_prev;
    }

  // unlink item

  if (item->_prev)   // has precessor
    {
      item->_prev->_next = item->_next;
    }
  else   // has no precessor, is first item in list
    {
      if ((_head = item->_next))
	{
	  _head->_prev = 0x0;
	}
    }

  if (item->_next)   // has successor
    {
      item->_next->_prev = item->_prev;
    }
  else   // has no successor, is last item in list
    {
      if ((_tail = item->_prev))
	{
	  _tail->_next = 0x0;
	}
    }

  //

  item->_list = 0x0;
  item->_prev = item->_next = 0x0;

  _numElems--;
}


//
//
//

#ifdef __GNUC__
#warning "browsing CLINLIST is not Unlink()-safe!!!"
#endif

void CLINLIST::WindToFirst (void)
{
  _curr = _head;
}


//
//
//

void CLINLIST::WindToLast (void)
{
  _curr = _tail;
}


//
//
//

CLINLISTITEM *CLINLIST::GetNext (void)
{
  CLINLISTITEM *ret = _curr;

  if (_curr)
    {
      _curr = _curr->_next;
    }

  return ret;
}


//
//
//

CLINLISTITEM *CLINLIST::GetPrev (void)
{
  CLINLISTITEM *ret = _curr;

  if (_curr)
    {
      _curr = _curr->_prev;
    }

  return ret;
}


#ifdef TEST

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  CLINLIST list;

  list.Dump ();

  CLINLISTITEM item1;
  list.Append (item1);

  list.Dump ();

  list.Unlink (item1);

  list.Dump ();

  return 0;
}

#endif
