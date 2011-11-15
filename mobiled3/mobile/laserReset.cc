//
// laserReset.cc
//

#include <stdio.h>
#include <stdlib.h>

#include "./sicklaser.h"


int main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <id>\n", *argv);
      return -1;
    }

  CSICKLASER laser (atoi (argv[1]), 0x0, false);

  return 0;
}
