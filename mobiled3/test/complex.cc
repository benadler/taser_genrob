//
//
//

#include <complex>
#include <iostream>


using namespace std;
typedef complex<double> complex_t;


int main (void)
{
  srandom (time (0));

  for (int i=0; i<20; i++)
    {
      const double angle1 = 2.0 * M_PI * (random () / (double)RAND_MAX) - M_PI;
      const double angle2 = 2.0 * M_PI * (random () / (double)RAND_MAX) - M_PI;

      complex_t c0 = polar (1.0, angle1);
      complex_t ce = polar (1.0, angle2);

      complex_t d = ce / c0;

      printf ("%f° -> %f° : %f°\n",
	      180.0 * angle1 / M_PI,
	      180.0 * angle2 / M_PI,
	      180.0 * arg (d) / M_PI);
    }
}
