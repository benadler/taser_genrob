//
//
//

#ifndef _RCCLSTACK_H
#define _RCCLSTACK_H

#include <rccl.h>

#include "util/memstack.h"
#include "infobase/infobase.h"
#include "util/args.h"


class CRCCLSTACK : public CMEMSTACK
{
private:

  // not even implemented
  void *operator new (size_t size);   // cannot be allocated on stack
  CRCCLSTACK (const CRCCLSTACK &);
  CRCCLSTACK &operator= (const CRCCLSTACK &);

protected:

public:

  CRCCLSTACK (void);
  virtual ~CRCCLSTACK (void);

  //

  void PushTransform (const char *name, TRSF *t)
    {
      CMEMSTACK::Push (name, t, (void (*) (void *))::freeTrans);
    }

  void PushScript (const char *name, char *script)
    {
      CMEMSTACK::Push (name, script, ::free);
    }

  TRSF *SearchTransform (const char *name)
    {
      return (TRSF *)CMEMSTACK::Search (name);
    }

  // the following allocates a memory block holding the array of pointers. the
  // block itself is put on this CRCCLSTACK so that it doesn't have to be
  // free()d manually, in fact it really mustn't be. it depends on the context
  // of the calling entity how long an orphaned block will remain allocated.

  TRSF **SearchTransforms (CARGS &args, int count);

  char *SearchScript (const char *name)
    {
      return (char *)CMEMSTACK::Search (name);
    }

  int LoadTransforms (const CINFOBASE *ib, const char *prefix);
  int LoadScripts (const CINFOBASE *ib, const char *prefix);

  POS *MakePosition (const CARGS &args);
};

#endif
