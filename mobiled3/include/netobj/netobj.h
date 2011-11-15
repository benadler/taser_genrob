//
//
//


#ifndef _NETOBJ_H
#define _NETOBJ_H


#include "netobj/chunk.h"
#include "sock/streamsock.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CNETOBJ
{
private:

  // not even implemented

  CNETOBJ (const CNETOBJ &);
  CNETOBJ &operator= (const CNETOBJ &);

protected:

  CNETOBJ * (*_CreateFxn) (void);

public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //
  //
  /////////////////////////////////////////////////////////////////////////////

  CNETOBJ (void);
  virtual ~CNETOBJ (void);

  static void Register (const char *name, CNETOBJ * (*func) (void));

  int SendSocket (CSTREAMSOCK *sock);
  int SendSocket (CSTREAMSOCK &sock)
    {
      return SendSocket (&sock);
    }

  //
  // this function `receives' one unknown object from a memory block. `unknown'
  // means that the type is not predetermined but the object will instead be
  // created (if the type is registered). the block is supposed to have been
  // allocated by `new [char]' and will be removed by `delete[]' internally!
  //

  static CNETOBJ *ReceiveUnknown (void *data, int size);

  //
  // these functions receive one unknown object from a socket.
  //

  static CNETOBJ *ReceiveUnknown (CSTREAMSOCK *sock);
  static CNETOBJ *ReceiveUnknown (CSTREAMSOCK &sock)
    {
      return ReceiveUnknown (&sock);
    }

  //
  // the same as above for known objects. the function must be called from the
  // object to be received. if the received object is actually a different one
  // a negative error will be returned. otherwhise the return value will be
  // positive.
  //

  // int ReceiveKnown (void *data, int size);

  int ReceiveKnown (CSTREAMSOCK *sock);
  int ReceiveKnown (CSTREAMSOCK &sock)
    {
      return ReceiveKnown (&sock);
    }

  //
  //
  //

  CNETOBJ * (*GetCreate (void)) (void)
    {
      return _CreateFxn;
    }


  //
  // these ones will be overlayed by descendants.
  //

  virtual const char *GetName (void) const = 0x0;
  virtual void Pack (CCHUNK *chunk) = 0x0;
  virtual void Unpack (CCHUNK *chunk) = 0x0;
};


//
//
//

#define NETOBJ_DECLARE(TYPE) \
  virtual const char *GetName (void) const {return #TYPE;} \
  virtual void Pack (CCHUNK *chunk); \
  virtual void Unpack (CCHUNK *chunk); \
  static TYPE *Create (void) {return new TYPE;}

#define NETOBJ_REGISTER(TYPE) \
  TYPE::Register(#TYPE,(CNETOBJ * (*) (void))&TYPE::Create);

#define NETOBJ_DYNCAST(obj,TYPE) \
  (TYPE *) (((void *)(obj)->GetCreate () == TYPE::Create) ? (obj) : 0x0)


#endif   // _NETOBJ_H
