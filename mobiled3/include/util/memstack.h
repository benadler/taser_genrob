//
//
//

#ifndef _MEMSTACK_H
#define _MEMSTACK_H

#include <unistd.h>   // wg. `size_t'

#include "util/linlist.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CMEMSTACKITEM : public CLINLISTITEM
{
  friend class CMEMSTACK;

private:

  // not even implemented
  CMEMSTACKITEM (void);
  CMEMSTACKITEM (const CMEMSTACKITEM &);
  CMEMSTACKITEM &operator= (const CMEMSTACKITEM &);

protected:

  char *_name;
  void *_ptr;
  void (*_free) (void *);

public:

  CMEMSTACKITEM (const char * const name,
		 void * const ptr,
		 void (*free) (void *));
  virtual ~CMEMSTACKITEM (void);

  void Dump (void) const;
};


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CMEMSTACK : protected CLINLISTITEM
{
private:

  // not even implemented
  void *operator new (size_t size);   // cannot be allocated on stack
  CMEMSTACK (const CMEMSTACK &);
  CMEMSTACK &operator= (const CMEMSTACK &);

protected:

  CLINLIST _items;

public:

  CMEMSTACK (void);
  virtual ~CMEMSTACK (void);

  void Dump (void);

  // the only things allowed are pushing of and searching for elements

  void Push (const char * const name,
	     void * const ptr,
	     void (*free) (void *));
  void *Search (const char * const name);

  //

  int GetSize (void) const
    {
      return _items.GetSize ();
    }

  void *operator[] (const int idx) const
    {
      CMEMSTACKITEM *item = (CMEMSTACKITEM *)_items[idx];

      return item ? item->_ptr : 0x0;
    }
};


#endif
