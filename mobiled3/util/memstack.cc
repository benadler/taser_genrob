//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/memstack.h"
#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

CMEMSTACKITEM::CMEMSTACKITEM (const char * const name,
			      void * const ptr,
			      void (*free) (void *))
{
  GTL ("CMEMSTACKITEM::CMEMSTACKITEM()");

  GTLPRINT (("name=\"%s\", ptr=%p, freeFxn=%p\n", name, ptr, free));

  if (name)
    {
      if (!(_name = ::strdup (name)))
	{
	  GTLFATAL (("out of memory\n"));
	  abort ();
	}
    }
  else
    {
      _name = 0x0;
    }

  _ptr = ptr;
  _free = free;
}

//
//
//

CMEMSTACKITEM::~CMEMSTACKITEM (void)
{
  GTL ("CMEMSTACKITEM::~CMEMSTACKITEM()");

  if (_list)
    {
      _list->Unlink (this);
    }

  if (_name)
    {
      ::free (_name);
    }

  _free (_ptr);
}


//
//
//

void CMEMSTACKITEM::Dump (void) const
{
  printf ("\"%s\" @ %p (free=%p)\n", _name, (void*)_ptr, (void*)_free);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static CLINLIST globMemstack;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

CMEMSTACK::CMEMSTACK (void)
{
  GTL ("CMEMSTACK::CMEMSTACK()");

  _prev = _next = 0x0;

  globMemstack.Append (this);
}


//
// delete a memstack. here we must and can delete all items first because we
// know they were created using new().
//

CMEMSTACK::~CMEMSTACK (void)
{
  GTL ("CMEMSTACK::~CMEMSTACK()");

  if (_next)
    {
      printf ("attempt to unlink an inner memstack\n");
      abort ();
    }

  // unlink/delete items

  _items.WindToLast ();

  CMEMSTACKITEM *item;

  while ((item = (CMEMSTACKITEM *)_items.GetPrev ()))
    {
      delete item;   // ~CLINLISTITEM() will unlink it
    }

  // unlink stack frame

  globMemstack.Unlink (this);
}


//
//
//

void CMEMSTACK::Dump (void)
{
  GTL ("CMEMSTACK::Dump()");

  _items.WindToLast ();

  CMEMSTACKITEM *item;

  while ((item = (CMEMSTACKITEM *)_items.GetPrev ()))
    {
#if 1
      GTLPRINT (("(%p<-%p->%p) -> %p,%p,%p\n",
		 item->_prev,
		 item,
		 item->_next,
		 item->_name,
		 item->_ptr,
		 item->_free));
#else
      ::fprintf (stderr,"(%p<-%p->%p) -> %p,%p,%p\n",
		 item->_prev,
		 item,
		 item->_next,
		 item->_name,
		 item->_ptr,
		 item->_free);

      ::fprintf (stderr,
		 "\"%s\" @ %p, freeFxn=%p\n",
		 item->_name ? item->_name : "<unnamed>",
		 item->_ptr,
		 item->_free);
#endif
    }

  if (_prev)
    {
      ((CMEMSTACK *)_prev)->Dump ();
    }
}


//
//
//

void CMEMSTACK::Push (const char * const name,
		      void * const ptr,
		      void (*free) (void *))
{
  GTL ("CMEMSTACK::Push()");

  GTLPRINT (("name=\"%s\", ptr=%p, free=%p\n", name, ptr, free));

  CMEMSTACKITEM *item = new CMEMSTACKITEM (name, ptr, free);

  GTLPRINT (("name=\"%s\", ptr=%p, free=%p\n", name, ptr, free));

  if (!item)
    {
      printf ("out of memory\n");
      abort ();
    }

  _items.Append (item);
}


//
//
//

void *CMEMSTACK::Search (const char * const name)
{
  GTL ("CMEMSTACK::Search()");

  GTLPRINT (("name= \"%s\"\n", name));

  _items.WindToLast ();

  CMEMSTACKITEM *item;

  while ((item = (CMEMSTACKITEM *)_items.GetPrev ()))
    {
      if (item->_name)   // cannot search for items without a name
	{
	  if (!::strcmp (item->_name, name))
	    {
	      GTLPRINT (("found %p\n", item->ptr));

	      return item->_ptr;
	    }
	}
    }

  if (_prev)
    {
      return ((CMEMSTACK *)_prev)->Search (name);
    }

  GTLPRINT (("not found\n"));

  return 0x0;
}


#ifdef TEST

//
//
//

int main (void)
{
  while (42)   // use a loop to test for memory leaks
    {
      CMEMSTACK ds;
      ds.Push ("speicher", malloc (13), free);

      {
	TRSF *t;
	CMEMSTACK ds;
	ds.Push ("mehr speicher", malloc (42), free);
	ds.Push ("mehr speicher #2", malloc (42), free);

	{
	  CMEMSTACK ds;
	  ds.Push ("ganz viel speicher", malloc (1024), free);

	  ds.Search ("speicher");
	  ds.Search ("tGrasp");
	}
      }
    }
}

#endif
