//
//
//

#include <string.h>
#include <stdlib.h>

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"

#include "./radialscan.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRADIALSCAN::CRADIALSCAN (void)
{
  GTL ("CRADIALSCAN::CRADIALSCAN()");

  _maxScans = 361;
  _numScans = 0;

  if (!(_scanAngle = (float *)malloc (_maxScans * sizeof (float))))
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  if (!(_scanDist = (float *)malloc (_maxScans * sizeof (float))))
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  _maxMarks = 361;
  _numMarks = 0;

  if (!(_markAngle = (float *)malloc (_maxMarks * sizeof (float))))
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  if (!(_markDist = (float *)malloc (_maxMarks * sizeof (float))))
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRADIALSCAN::~CRADIALSCAN (void)
{
  GTL ("CRADIALSCAN::~CRADIALSCAN()");

  free (_markDist);
  free (_markAngle);
  free (_scanDist);
  free (_scanAngle);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRADIALSCAN &CRADIALSCAN::operator= (const CRADIALSCAN &right)
{
  GTL ("CRADIALSCAN::operator=()");

  if ((_maxScans != right._maxScans) || (_maxMarks != right._maxMarks))
    {
      GTLFATAL (("scan sizes don't fit\n"));
      abort ();
    }

  _numScans = right._numScans;

  memcpy (_scanAngle, right._scanAngle, _numScans * sizeof (float));
  memcpy (_scanDist,  right._scanDist,  _numScans * sizeof (float));

  _numMarks = right._numMarks;

  memcpy (_markAngle, right._markAngle, _numScans * sizeof (float));
  memcpy (_markDist,  right._markDist,  _numScans * sizeof (float));

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
// CRADIALSCANMATCHED
//
// a class that extends CRADIALSCAN by a vector of indexes for matched marks
//
///////////////////////////////////////////////////////////////////////////////

CRADIALSCANMATCHED::CRADIALSCANMATCHED (void)
{
  GTL ("CRADIALSCANMATCHED::CRADIALSCANMATCHED()");

  if (!(_markIdx = (int *)malloc (_maxMarks * sizeof (int))))
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRADIALSCANMATCHED::~CRADIALSCANMATCHED (void)
{
  GTL ("CRADIALSCANMATCHED::~CRADIALSCANMATCHED()");

  free (_markIdx);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRADIALSCANMATCHED &CRADIALSCANMATCHED::operator= (const CRADIALSCAN &right)
{
  GTL ("CRADIALSCANMATCHED::operator=(CRADIALSCAN)");

  if ((_maxScans != right._maxScans) || (_maxMarks != right._maxMarks))
    {
      GTLFATAL (("scan sizes don't fit\n"));
      abort ();
    }

  _numScans = right._numScans;

  memcpy (_scanAngle, right._scanAngle, _numScans * sizeof (float));
  memcpy (_scanDist,  right._scanDist,  _numScans * sizeof (float));

  _numMarks = right._numMarks;

  memcpy (_markAngle, right._markAngle, _numScans * sizeof (float));
  memcpy (_markDist,  right._markDist,  _numScans * sizeof (float));

  for (int i=0; i<_numMarks; i++)
    {
      _markIdx[i] = -1;
    }

  return *this;
}


CRADIALSCANMATCHED &CRADIALSCANMATCHED::operator= (const CRADIALSCANMATCHED &right)
{
  GTL ("CRADIALSCANMATCHED::operator=(CRADIALSCANMATCHED&)");

  if ((_maxScans != right._maxScans) || (_maxMarks != right._maxMarks))
    {
      GTLFATAL (("scan sizes don't fit\n"));
      abort ();
    }

  _numScans = right._numScans;

  memcpy (_scanAngle, right._scanAngle, _numScans * sizeof (float));
  memcpy (_scanDist,  right._scanDist,  _numScans * sizeof (float));

  _numMarks = right._numMarks;

  memcpy (_markAngle, right._markAngle, _numScans * sizeof (float));
  memcpy (_markDist,  right._markDist,  _numScans * sizeof (float));
  memcpy (_markIdx,  right._markIdx,  _numScans * sizeof (int));

  return *this;
}
