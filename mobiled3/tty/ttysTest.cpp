//
//
//

#include <stdio.h>

#include "tty/ttys.h"

int main (void)
{
  CTTYS ttys ("/dev/ttyS0");

#ifdef __GNUC__
#warning "needs real check"
#endif

  return -1;
}
