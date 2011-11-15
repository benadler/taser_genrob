//
//
//

#include <stdio.h>

#include "tty/mouse.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <devicename>\n", argv[0]);
      return -1;
    }

  CMOUSE mouse (argv[1]);

  int x = 0, y = 0;

  while (42)
    {
      int buttons, dx, dy;

      mouse.GetEvent (buttons, dx, dy);

      x += dx;
      y += dy;

      printf ("buttons=%i, dx=%i, dy=%i -> pos=%i,%i\n",
	      buttons, dx, dy, x, y);
    }

  return 0;
}
