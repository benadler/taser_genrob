//
//
//

#ifndef _FMAP_H_
#define _FMAP_H_

#include <math.h>


static double fmap (const double arg, const double range)
{
  if (arg < 0.0)
    {
      return range - fmod (-arg, range);
    }

  return fmod (arg, range);
}


#endif
