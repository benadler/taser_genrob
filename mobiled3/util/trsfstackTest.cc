//
//
//

#include <stdlib.h>

#include "infobase/infobase.h"
#include "util/trsfstack.h"


//
//
//

int main (void)
{
  CINFOBASE ib;

  ib.Unlink ("/");   // clear infobase

  ib.Mkdir ("/trsfs");
  ib.Creat ("/trsfs/tBase");
  ib.WriteString ("/trsfs/tBase", "0 0 0 0 0 0");
  ib.Creat ("/trsfs/tObj");
  ib.WriteString ("/trsfs/tObj", "0 0 0 0 0 0");

  //

  CTRSFSTACK ts;

  int cnt = ts.Load (&ib, "/trsfs");

  printf ("%i trsfs loaded\n", cnt);

  while (42)   // use a loop to check for memory leaks
    {
      CTRSFSTACK ts;

      ts.Push ("tTool", allocTrans (NULL, UNDEF));

      {
	CTRSFSTACK ts;

	ts.Push ("tGrasp", allocTransXyz (NULL, UNDEF, 1, 1, 1));
	ts.Push ("tOffset", allocTransXyz (NULL, UNDEF, 2, 2, 2));
	ts.Push ("tGripper", allocTransXyz (NULL, UNDEF, 3, 3, 3));

	{
	  CTRSFSTACK ts;

	  ts.Push ("tTarg", allocTrans (NULL, UNDEF));

	  POS *p = ts.MakePosition ("tBase T6 tTool EQ tObj tGrasp");

	  ::freePosition (p);
	}
      }
    }
}
