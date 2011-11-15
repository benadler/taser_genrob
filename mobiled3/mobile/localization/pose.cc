#include "thread/tracelog.h"
#include "pose.h"

CPOSE::CPOSE (meter_t xKoord, meter_t yKoord, radiant_t thetaOrient)
{
  x = xKoord;
  y = yKoord;
  theta = thetaOrient;
}

CPOSE::~CPOSE (void)
{

}

CVEC CPOSE::getAsVector (void)
{
  return CVEC (x, y, theta);
}
