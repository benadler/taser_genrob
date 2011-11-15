//
//
//


// Die folgenden drei Zeilen kommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
//#ifdef DEBUG
//#undef DEBUG
//#endif

#include <math.h>
#include "thread/tracelog.h"
#include "./move.h"
#include "./rotation.h"
#include "./translation.h"

///////////////////////////////////////////////////////////////////////////////
//
// Konstruktor, eine Liste von Bewegungen wird abgefahren.
// 
///////////////////////////////////////////////////////////////////////////////

CMOVE::CMOVE (const int numOfMotions, _CMOTION **motion)
  : _CMOTION (MOTION_MOVE)
{
  GTL ("CMOVE::CMOVE()");

  //

  _numMotions = numOfMotions;

  _motion = (_CMOTION **) motion;

  _activeMotion = 0;
   
  _current = _motion[0];

}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMOVE::~CMOVE (void)
{
  GTL ("CMOVE::~CMOVE()");

  for (int i=0; i<_numMotions; i++)
    {
      GTLPRINT (("delete %p\n", _motion[i]));
      delete _motion[i];
    }

  GTLPRINT (("delete %p (master)\n", _motion));

  delete _motion;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

bool CMOVE::Step (const CVEC pose,
		  const CVEC velocities,
		  double &transVel,
		  double &rotVel)
{
  GTL ("CMOVE::Step()");

  //

  GTLPRINT (("current = %p (%s)\n",
  	     _current,
  	     _current->GetType() == MOTION_ROTATEANGLE ? "ROTATEANGLE" :
  	     _current->GetType() == MOTION_ROTATEPOINT ? "ROTATEPOINT" :
  	     _current->GetType() == MOTION_TRANSLATE ? "TRANSLATE" :
  	     _current->GetType() == MOTION_MOVE ? "MOVE" :
  	     _current->GetType() == MOTION_FORWARD ? "FORWARD" :
  	     "UNKNOWN"));

  if (_abort)
    {
      GTLPRINT (("Bewegung soll beendet werden.\n"));

      //++westhoff20040728: ??? What am i doing here, hmm seems to work ???
      _current->Abort();

      return true;
    }

  //

  bool done = _current->Step (pose, velocities, transVel, rotVel);

  //

  _velocitiesChanged = false;
  _motorStopped = false;

  //

  if (done)
    {
      GTLPRINT (("%p done\n", _current));

      if (++_activeMotion < _numMotions)
	{
	  done = false;
	  _current = _motion[_activeMotion];
	}
    }

  if (done)
    {
      GTLPRINT (("done\n"));
    }

  return done;
}

