//
//
//


#include <stdio.h>


static void itoa (int i, char *s)
{
}


static void ftoa (float f, char *s)
{
}


//
// 0x80000000 - mantissa sign
// 0x7f800000 - exponent
//

//
//  0.0 -> 0x00000000
//  1.0 -> 0x3f800000
// -1.0 -> 0xbf800000
//  2.0 -> 0x40000000
// -2.0 -> 0xc0000000
//  4.0 -> 0x40800000
// -4.0 -> 0xc0000000
//


int main (void)
{
  float f = -4.0;
  unsigned int i, ii = 0;

  while (42)
    {
      i = *(unsigned int *)&f;
      ii |= i;
      printf ("%e -> 0x%x, 0x%x\n", f, i, ii);

      //f *= 2.0;
      f /= 2.0;
    }
}
