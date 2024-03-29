//
//
//

#include <math.h>
#include <stdio.h>

#include "thread/tracelog.h"

#include "./motion.h"

#undef LOG_TARGETS


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CMOTION::_CMOTION (const MOTIONTYPE type)
{
  GTL ("_CMOTION::_CMOTION()");

  _type = type;

  _velocitiesChanged = false;

  _motorStopped = false;

  _abort = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CMOTION::~_CMOTION (void)
{
}
