//
//
//

#ifndef _LINLIST_H
#define _LINLIST_H

#if defined (__GNUC__) && defined (_REENTRANT)
#warning "CLINLIST is not MT-safe!!!"
#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CLINLIST;

class CLINLISTITEM
{
  friend class CLINLIST;

private:

  // not even implemented
  CLINLISTITEM (const CLINLISTITEM &);
  CLINLISTITEM &operator= (const CLINLISTITEM &);

protected:

  CLINLIST *_list;
  CLINLISTITEM *_prev, *_next;

public:

  CLINLISTITEM (void)
  {
    _list = 0x0;
    _prev = _next = 0x0;
  }

  virtual ~CLINLISTITEM (void);
};


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CLINLIST
{
private:

  // not even implemented
  CLINLIST (const CLINLIST &);
  CLINLIST &operator= (const CLINLIST &);

protected:

  int _numElems;

  CLINLISTITEM *_head, *_tail;
  CLINLISTITEM *_curr;

public:

  //

  CLINLIST (void)
  {
    _numElems = 0;
    _head = _tail = _curr = 0x0;
  }

  virtual ~CLINLIST (void);

  //

  void Dump (void) const;

  // direct access to list elements

  int GetSize (void) const
    {
      return _numElems;
    }

  CLINLISTITEM *operator[] (int idx) const;

  //

  void Append (CLINLISTITEM *item);
  void Append (CLINLISTITEM &item)
  {
    Append (&item);
  }

  void Unlink (CLINLISTITEM *item);
  void Unlink (CLINLISTITEM &item)
  {
    Unlink (&item);
  }

  // browse the list

  void WindToFirst (void);
  void WindToLast (void);
  CLINLISTITEM *GetNext (void);
  CLINLISTITEM *GetPrev (void);
};

#endif
