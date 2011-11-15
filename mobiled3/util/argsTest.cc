//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "util/args.h"
#include "util/strfxns.h"


int main (void)
{
  {
    CARGS args (0x0);
    printf ("%s\n", args.Dump ());
  }

  {
    CARGS args ("");
    printf ("%s\n", args.Dump ());
  }

  {
    CARGS args ("hallo");
    printf ("%s\n", args.Dump ());
  }

  {
    CARGS args ("hallo Du");
    printf ("%s\n", args.Dump ());
  }

  {
    CARGS args (" hallo Du nase!!! ");
    printf ("%s\n", args.Dump ());
  }

  return 0;
}
