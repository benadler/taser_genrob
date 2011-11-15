//
//
//

#include "mat.h"
#include "vec.h"


static void show (CTMPMAT &m)
{
  ::printf ("show()\n");

  float *ptr = &m[0][0];

  for (int r=0; r<m.GetRows(); r++)
    {
      for (int c=0; c<m.GetCols(); c++)
	{
	  ::printf ("%f ", *ptr++);
	}
      ::printf ("\n");
    }
  ::printf ("\n");
}


int main (void)
{
  CMAT m1 (2, 3);
  m1[0][0] = 1;
  m1[0][1] = -2;
  m1[0][2] = 2;
  m1[1][0] = 5;
  m1[1][1] = -3;
  m1[1][2] = 2;
  show (m1);

  CMAT m2 (3, 3);
  m2[0][0] = 1;
  m2[0][1] = 4;
  m2[0][2] = 2;
  m2[1][0] = -6;
  m2[1][1] = 5;
  m2[1][2] = -9;
  m2[2][0] = -3;
  m2[2][1] = 6;
  m2[2][2] = -5;
  show (m2);

  CMAT m = m1 * m2;
  show (m);

  m[1] = m2[1];
  show (m);
}
