//
//
//

#include <stdio.h>

#include "tty/kbd.h"

int main (void)
{
  CKBD kbd;
  unsigned long key;

  while ((key = kbd.GetKey ()) != 3)
    {
      fprintf (stderr, "key pressed: 0x%08lx\r\n", key);
    }

  return 0;
}
