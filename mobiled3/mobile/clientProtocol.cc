//
// clientProtocol.cc
// (C) 12/2002 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#ifdef DEBUG
#undef DEBUG
#endif

#include "thread/tracelog.h"
#include "util/args.h"
#include "mobile/mobiletypes.h"

#include "./client.h"
#include "./protocol.h"
#include "./defines.h"
#include "pathplanner/point.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::Ping (void) const
{
  GTL ("CCLIENT::Ping()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_PING);

  snd.PushS32 (0);   // status "ok"

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::CheckProtocol (void) const
{
  GTL ("CCLIENT::CheckProtocol()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_CHECKPROTOCOL);

  snd.PushS32 (0);   // status "ok"
  snd.PushU32 (0);   // version?

  snd.PushU32 (38);  // ++westhoff20060421: decreased from 40 to 38

  snd.PushU32 (CMD_PING);
  snd.PushU32 (CMD_CHECKPROTOCOL);
  snd.PushU32 (CMD_DEBUGGING);

  snd.PushU32 (CMD_GETMODE);
  snd.PushU32 (CMD_SETMODE);

  snd.PushU32 (CMD_GETPOSITION);
  snd.PushU32 (CMD_SETPOSITION);
  snd.PushU32 (CMD_GETPOSITIONANDVARIANCE);
  snd.PushU32 (CMD_GETPOSITIONODO);
  snd.PushU32 (CMD_GETALLMARKS);
  snd.PushU32 (CMD_GETALLMARKSINITIAL);
  snd.PushU32 (CMD_GETALLMARKSWITHVARIANCES);
  snd.PushU32 (CMD_GETSCALE);
  snd.PushU32 (CMD_SETSCALE);
  snd.PushU32 (CMD_MODIFYSCALE);
  snd.PushU32 (CMD_STOP);
  snd.PushU32 (CMD_TRANSLATE);
  snd.PushU32 (CMD_FORWARD);
  snd.PushU32 (CMD_ROTATEANGLE);
  //  snd.PushU32 (CMD_ROTATEPOINT);    // --westhoff20060905
  snd.PushU32 (CMD_TURNANGLE);          // ++westhoff20060421
  snd.PushU32 (CMD_MOVE);
  //  snd.PushU32 (CMD_MOVEPOINT);      // --westhoff20060905
  snd.PushU32 (CMD_ISCOMPLETED);
  snd.PushU32 (CMD_WAITFORCOMPLETED);
  snd.PushU32 (CMD_STALLED);            // ++westhoff20050506
  snd.PushU32 (CMD_APPLYBRAKES);        // ++westhoff20050803
  snd.PushU32 (CMD_RELEASEBRAKES);      // ++westhoff20050803
  snd.PushU32 (CMD_AREBRAKESRELEASED);  // ++westhoff20050803

  // 0x0004 prefix
  snd.PushU32 (CMD_GETNUMSCANNERS);
  snd.PushU32 (CMD_GETSCANNERPOSITION);
  snd.PushU32 (CMD_GETSCANSCANNER);         // ++westhoff20050808
  snd.PushU32 (CMD_GETSCANRADIALSCANNER);   // ++westhoff20050715
  snd.PushU32 (CMD_GETSCANPLATFORM);
  snd.PushU32 (CMD_GETSCANWORLD);

  snd.PushU32 (CMD_GETALLLINES);

  // ++westhoff20050202: Start-/StopOdoLogging
  // 0x0006 prefix
  snd.PushU32 (CMD_STARTODOLOGGING);
  snd.PushU32 (CMD_STOPODOLOGGING);

  // ++westhoff20050330: Batterie und Temperaturesensoren
  // 0x0007 prefix
  snd.PushU32 (CMD_GETBATTERYVOLTAGE);
  snd.PushU32 (CMD_GETDRIVETEMPERATURES);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::Debugging (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::Debugging()");

  const int flag = telegram->PopS32 ();

  CSNDTELEGRAM snd (CMD_REPLY | CMD_DEBUGGING);

  snd.PushS32 (0);   // status "ok"
  snd.PushS32 (_globGtlEnable);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }

  _globGtlEnable = flag;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetPosition (void) const
{
  GTL ("CCLIENT::GetPosition()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETPOSITION);

  snd.PushS32 (0);   // status "ok"

  meter_t x, y;
  radiant_t a;

  _genBase->GetPos (x, y, a);

  snd.PushF32 (x);
  snd.PushF32 (y);
  snd.PushF32 (a);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


void CCLIENT::GetPositionOdo (void) const
{
  GTL ("CCLIENT::GetPositionOdo()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETPOSITIONODO);

  meter_t x, y;
  radiant_t a;

  snd.PushS32 (_genBase->GetPosOdo (x, y, a));   // status?

  snd.PushF32 (x);
  snd.PushF32 (y);
  snd.PushF32 (a);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetPositionAndVariance (void) const
{
  GTL ("CCLIENT::GetPositionAndVariance()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETPOSITIONANDVARIANCE);

  snd.PushS32 (0);   // status "ok"

  meter_t x, y;
  radiant_t a;

  double varXX, varXY, varYX, varYY, varAA;

  _genBase->GetPosAndVar (x, y, a, varXX, varXY, varYX, varYY, varAA);

  snd.PushF32 (x);
  snd.PushF32 (y);
  snd.PushF32 (a);
  snd.PushF32 (varXX);
  snd.PushF32 (varXY);
  snd.PushF32 (varYX);
  snd.PushF32 (varYY);
  snd.PushF32 (varAA);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::SetPosition (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::SetPosition()");

  meter_t x = telegram->PopF32 ();
  meter_t y = telegram->PopF32 ();
  radiant_t a = telegram->PopF32 ();

  _genBase->SetPos (x, y, a);

  CSNDTELEGRAM snd (CMD_REPLY | CMD_SETPOSITION);

  snd.PushS32 (0);   // status "ok"

  _genBase->GetPos (x, y, a);

  snd.PushF32 (x);
  snd.PushF32 (y);
  snd.PushF32 (a);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetAllMarks (void) const
{
  GTL ("CCLIENT::GetAllMarks()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETALLMARKS);

  snd.PushS32 (0);   // status "ok"

  CVEC marks;

  const int numMarks = _genBase->GetMarks (marks);

  snd.PushU32 (numMarks);

  for (int i=0; i<numMarks; i++)
    {
      snd.PushU32 (i);   // id
      snd.PushF32 (marks[2*i+0]);
      snd.PushF32 (marks[2*i+1]);
      snd.PushF32 (0.0);   // angle
      snd.PushF32 (0.0);   // range
    }

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetAllMarksInitial (void) const
{
  GTL ("CCLIENT::GetAllMarksInitial()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETALLMARKSINITIAL);

  snd.PushS32 (0);   // status "ok"

  //

  const CMAP &map = _genBase->_map;

  const int numMarks = map._numMarks;
  const CMAP::SMARK_t *mark = map._mark;

  snd.PushU32 (numMarks);

  GTLPRINT (("Anzahl der Marken: %i\n", numMarks));

  for (int i=0; i<numMarks; i++)
    {
      snd.PushU32 (i);   // id
      snd.PushF32 (mark[i].x);
      snd.PushF32 (mark[i].y);
      snd.PushF32 (0.0);   // angle
      snd.PushF32 (0.0);   // range
    }

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetAllMarksWithVariances (void) const
{
  GTL ("CCLIENT::GetAllMarksWithVariances()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETALLMARKSWITHVARIANCES);

  snd.PushS32 (0);   // status "ok"

  CVEC marks;
  CVEC variances;

  const int numMarks = _genBase->GetMarks (marks, variances);

  snd.PushU32 (numMarks);

  for (int i=0; i<numMarks; i++)
    {
      snd.PushU32 (i);   // id
      snd.PushF32 (marks[2*i+0]);
      snd.PushF32 (marks[2*i+1]);
      snd.PushF32 (0.0);   // angle
      snd.PushF32 (0.0);   // range
      snd.PushF32 (variances[4*i+0]);
      snd.PushF32 (variances[4*i+1]);
      snd.PushF32 (variances[4*i+2]);
      snd.PushF32 (variances[4*i+3]);
    }

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetAllLines (void) const
{
  GTL ("CCLIENT::GetAllLines()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETALLLINES);

  snd.PushS32 (0);   // status "ok"

  //

  const CMAP &map = _genBase->_map;

  const int numLines = map._numLines;
  const CMAP::SLINE_t *line = map._line;

  snd.PushU32 (numLines);

  for (int i=0; i<numLines; i++)
    {
      snd.PushU32 (i);   // id

      snd.PushF32 (line[i].x0);
      snd.PushF32 (line[i].y0);
      snd.PushF32 (line[i].xe);
      snd.PushF32 (line[i].ye);
    }

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetNumScanners (void) const
{
  GTL ("CCLIENT::GetNumScanners()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETNUMSCANNERS);

  snd.PushS32 (0);   // status "ok"

  //

  snd.PushU32 (_genBase->GetNumScanners ());

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetScannerPosition (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::GetScannerPosition()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETSCANNERPOSITION);

  //

  const unsigned int idx = telegram->PopU32 ();

  meter_t x;
  meter_t y;
  radiant_t a;

  snd.PushS32 (_genBase->GetScannerPosition (x, y, a, idx));

  snd.PushF32 (x);
  snd.PushF32 (y);
  snd.PushF32 (180.0 * a / M_PI);

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetScanScanner (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::GetScanScanner()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETSCANSCANNER);

  // ID des Scanners

  const unsigned int idx = telegram->PopU32 ();

  CRADIALSCANMATCHED scan;

  if (_genBase->GetLaserScan (idx, scan) < 0)   // radial!
    {
      snd.PushS32 (-1);   // status
      snd.PushU32 (0);   // no scans
      snd.PushU32 (0);   // no marks
    }
  else
    {
      snd.PushS32 (0);   // status "ok"

      // convert scan data to scanner coordinates

      snd.PushU32 (scan._numScans);

      for (int i=0; i<scan._numScans; i++)
        {
          const double a = scan._scanAngle[i];
          const double x = cos (a) * scan._scanDist[i];
          const double y = sin (a) * scan._scanDist[i];

          snd.PushF32 (x);
          snd.PushF32 (y);
        }

      // convert mark data to world coordinates

      snd.PushU32 (scan._numMarks);

      for (int m=0; m<scan._numMarks; m++)
        {
          const double a = scan._markAngle[m];
          const double x = cos (a) * scan._markDist[m];
          const double y = sin (a) * scan._markDist[m];

          snd.PushF32 (x);
          snd.PushF32 (y);
          snd.PushS32 (scan._markIdx[m]);
        }
    }

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetScanPlatform (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::GetScanPlatform()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETSCANPLATFORM);

  //

  const unsigned int idx = telegram->PopU32 ();

  CRADIALSCANMATCHED scan;

  if (_genBase->GetLaserScan (idx, scan) < 0)   // radial!
    {
      snd.PushS32 (-1);   // status
      snd.PushU32 (0);   // no scans
      snd.PushU32 (0);   // no marks
    }
  else
    {
      snd.PushS32 (0);   // status "ok"

      //

      meter_t x0, y0;
      radiant_t a0;

      _genBase->GetScannerPosition (x0, y0, a0, idx);

      // convert scan data to platform coordinates

      snd.PushU32 (scan._numScans);

      for (int i=0; i<scan._numScans; i++)
	{
	  const double a = scan._scanAngle[i] + a0;
	  const double x = x0 + cos (a) * scan._scanDist[i];
	  const double y = y0 + sin (a) * scan._scanDist[i];

	  snd.PushF32 (x);
	  snd.PushF32 (y);
	}

      // convert mark data to world coordinates

      snd.PushU32 (scan._numMarks);

      for (int m=0; m<scan._numMarks; m++)
	{
	  const double a = scan._markAngle[m] + a0;
	  const double x = x0 + cos (a) * scan._markDist[m];
	  const double y = y0 + sin (a) * scan._markDist[m];

	  snd.PushF32 (x);
	  snd.PushF32 (y);
	  snd.PushS32 (scan._markIdx[m]);
	}
    }

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetScanRadialScanner (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::GetScanRadialScanner()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETSCANRADIALSCANNER);

  //

  const unsigned int idx = telegram->PopU32 ();

  CRADIALSCANMATCHED scan;

  if (_genBase->GetLaserScan (idx, scan) < 0)   // radial!
    {
      snd.PushS32 (-1);   // status
    }
  else
    {
      snd.PushS32 (0);   // status "ok"

      for (int i=0; i < 361; ++i)
      {
        snd.PushF32 (scan._scanDist[i]);
      }
    }

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetScanWorld (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::GetScanWorld()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETSCANWORLD);

  //

  const unsigned int idx = telegram->PopU32 ();

  CRADIALSCANMATCHED scan;

  if (_genBase->GetLaserScanPlatform (idx, scan) < 0)   // radial!
    {
      snd.PushS32 (-1);  // status
      snd.PushU32 (0);   // no scans
      snd.PushU32 (0);   // no marks
    }
  else
    {
      snd.PushS32 (0);   // status "ok"

      //

      meter_t x0, y0;
      radiant_t a0;

      _genBase->GetPos (x0, y0, a0);

      // convert scan data to world coordinates

      snd.PushU32 (scan._numScans);

      for (int i=0; i<scan._numScans; i++)
	{
	  const double a = scan._scanAngle[i] + a0;
	  const double x = x0 + cos (a) * scan._scanDist[i];
	  const double y = y0 + sin (a) * scan._scanDist[i];

	  snd.PushF32 (x);
	  snd.PushF32 (y);
	}

      // convert mark data to world coordinates

      snd.PushU32 (scan._numMarks);

      for (int m=0; m<scan._numMarks; m++)
	{
	  const double a = scan._markAngle[m] + a0;
	  const double x = x0 + cos (a) * scan._markDist[m];
	  const double y = y0 + sin (a) * scan._markDist[m];

	  snd.PushF32 (x);
	  snd.PushF32 (y);
	  snd.PushS32 (scan._markIdx[m]);
	}
    }

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::StopMotion (void) const
{
  GTL ("CCLIENT::StopMotion()");

  _genBase->AbortCurrentMotion ();

   // ++westhoff20050805: Warten bis die Bewegung wirklich angehalten wurde.
  _genBase->WaitIdleCondition ();

  _genBase->UnlockIdleCondition ();

  _genBase->EnableMotion (); // westhoff20050805: Neue Bewegungen zulassen

  CSNDTELEGRAM snd (CMD_REPLY | CMD_STOP);

  snd.PushS32 (0);   // ++westhoff20040728: Somehow i did it :-)
                     // ++itschere20021219: not yet operational

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::Translate (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::Translate()");

  double x = telegram->PopF32 ();
  double y = telegram->PopF32 ();

  int error = _genBase->MotionTranslate (x, y);

  CSNDTELEGRAM snd (CMD_REPLY | CMD_TRANSLATE);

  // Status (wenn Status == 1, dann waren die Bremsen nicht gelöst)
  snd.PushS32 (error);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

// ANMERKUNG: Drehung mit Kollisionsvermeidung
void CCLIENT::RotateAngle (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::RotateAngle()");

  double a = telegram->PopF32 ();

  // Rotation mit eingeschalteter Kollisionsvermeidung
  int error = _genBase->MotionRotateAngle (a, true);

  CSNDTELEGRAM snd (CMD_REPLY | CMD_ROTATEANGLE);

  // Status (wenn Status == 1, dann waren die Bremsen nicht gelöst)
  snd.PushS32 (error);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}

// ANMERKUNG: Drehung ohne Kollisionsvermeidung
void CCLIENT::TurnAngle (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::RotateAngle()");

  double a = telegram->PopF32 ();

  // Rotation mit ausgeschalteter Kollisionsvermeidung
  int error = _genBase->MotionRotateAngle (a, false);

  CSNDTELEGRAM snd (CMD_REPLY | CMD_TURNANGLE);

  // Status (wenn Status == 1, dann waren die Bremsen nicht gelöst)
  snd.PushS32 (error);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// ++itschere20021107: direct access to path planner
// ++westhoff20060628: Kein Pathplanner mehr
//
///////////////////////////////////////////////////////////////////////////////

static int GetPath (vector<Point> &path,
		    const double x0, const double y0,
		    const double xe, const double ye)
{
  GTLC ("GetPath()");

  path.clear ();

  Point pe;
  pe.setX (xe);
  pe.setY (ye);
  path.insert (path.begin (), pe);

  Point p0;
  p0.setX (x0);
  p0.setY (y0);
  path.insert (path.begin (), p0);

  const int size = path.size ();

  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::Move (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::Move()");

  double xe = telegram->PopF32 ();
  double ye = telegram->PopF32 ();
  double ae = telegram->PopF32 ();

  //
  // have to wait for the platform to be idle here because we need a proper
  // `current' position as start point of the path.
  //

  _genBase->WaitIdleCondition ();

  GTLPRINT (("is idle\n"));

  CSNDTELEGRAM snd (CMD_REPLY | CMD_MOVE);

  //

  meter_t x0, y0;
  radiant_t a0;

  _genBase->GetPos (x0, y0, a0);

  vector<Point> path;

  int size = ::GetPath (path, x0, y0, xe, ye);

  if (size < 0)
    {
      _genBase->UnlockIdleCondition ();   // ++itschere20021115: hmmm...

      // Status: Fehler, Start/Ziel in Hinderniss oder kein Pfad gefunden
      snd.PushS32 (size);
    }
  else
    {
      //

      CVEC points (2 * size);

      vector<Point>::const_iterator point = path.begin ();

      for (int i=0; i<size; i++)
	{
	  points[2*i+0] = (*point  ).getX ();
	  points[2*i+1] = (*point++).getY ();
	}

      // Status (wenn Status == 1, dann waren die Bremsen nicht gelöst)
      snd.PushS32 (_genBase->MotionMove (points, ae));
    }

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::Forward (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::Forward()");

  double d = telegram->PopF32 ();

  int error = _genBase->MotionForward (d);

  CSNDTELEGRAM snd (CMD_REPLY | CMD_FORWARD);

  // Status (wenn Status == 1, dann waren die Bremsen nicht gelöst)
  snd.PushS32 (error);

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::IsCompleted (void) const
{
  GTL ("CCLIENT::IsCompleted()");

  unsigned int flag = _genBase->IsCompleted ();

  CSNDTELEGRAM snd (CMD_REPLY | CMD_ISCOMPLETED);

  snd.PushS32 (0);   // status

  snd.PushU32 (flag);

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::WaitForCompleted (void) const
{
  GTL ("CCLIENT::WaitForCompleted()");

  _genBase->WaitIdleCondition ();

  GTLPRINT (("is idle\n"));

  _genBase->UnlockIdleCondition ();

  CSNDTELEGRAM snd (CMD_REPLY | CMD_WAITFORCOMPLETED);

  snd.PushS32 (0);

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::Stalled (void) const
{
  GTL ("CCLIENT::Stalled()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_STALLED);

  // Status

  snd.PushS32 (0);

  // Blocked: yes or no

  snd.PushS32 (_genBase->Stalled ());

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
  {
    GTLFATAL (("Send() failed\n"));
    throw -1;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::ApplyBrakes (void) const
{
  GTL ("CCLIENT::ApplyBrakes()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_APPLYBRAKES);

  // Status

  snd.PushS32 (_genBase->SetBrakes (true));

  // Send telegram

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
  {
    GTLFATAL (("Send() failed\n"));
    throw -1;
  }
}


void CCLIENT::ReleaseBrakes (void) const
{
  GTL ("CCLIENT::ReleaseBrakes()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_RELEASEBRAKES);

  // Status

  snd.PushS32 (_genBase->SetBrakes (false));

  // Send telegram

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
  {
    GTLFATAL (("Send() failed\n"));
    throw -1;
  }
}


void CCLIENT::AreBrakesReleased (void) const
{
  GTL ("CCLIENT::AreBrakesReleased()");

  CSNDTELEGRAM snd (CMD_REPLY | CMD_AREBRAKESRELEASED);

  // Status

  snd.PushS32 (0);

  // released: yes or no?

  snd.PushS32 (!_genBase->AreBrakesEnabled());

  // Send telegram

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
  {
    GTLFATAL (("Send() failed\n"));
    throw -1;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetScale (void) const
{
  GTL ("CCLIENT::GetScale()");

  //

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETSCALE);

  snd.PushS32 (0);

  double tScale, rScale;

  _genBase->GetScale (tScale, rScale);

  snd.PushF32 (tScale);
  snd.PushF32 (rScale);

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


void CCLIENT::SetScale (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::SetScale()");

  //

  const double tScale = telegram->PopF32 ();
  const double rScale = telegram->PopF32 ();

  //

  CSNDTELEGRAM snd (CMD_REPLY | CMD_SETSCALE);

  snd.PushS32 (0);

  double tScaleRet, rScaleRet;

  _genBase->SetScale (tScale, rScale, tScaleRet, rScaleRet);

  snd.PushF32 (tScaleRet);
  snd.PushF32 (rScaleRet);

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


void CCLIENT::ModifyScale (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::ModifyScale()");

  //

  const double tScaleFactor = telegram->PopF32 ();
  const double rScaleFactor = telegram->PopF32 ();

  //

  CSNDTELEGRAM snd (CMD_REPLY | CMD_MODIFYSCALE);

  snd.PushS32 (0);

  double tScaleRet, rScaleRet;

  _genBase->ModifyScale (tScaleFactor, rScaleFactor, tScaleRet, rScaleRet);

  snd.PushF32 (tScaleRet);
  snd.PushF32 (rScaleRet);

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetMode (void) const
{
  GTL ("CCLIENT::GetMode()");

  //

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETMODE);

  snd.PushS32 (0);   // status

  snd.PushS32 (_genBase->GetMode ());

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}


void CCLIENT::SetMode (CRCVTELEGRAM *telegram) const
{
  GTL ("CCLIENT::SetMode()");

  //

  const MOBILEMODE mode = (MOBILEMODE)telegram->PopS32 ();

  //

  CSNDTELEGRAM snd (CMD_REPLY | CMD_SETMODE);

  snd.PushS32 (0);   // status

  snd.PushS32 (_genBase->SetMode (mode));
  snd.PushS32 (_genBase->GetMode ());

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::StartOdoLogging (void) const
{
  GTL ("CCLIENT::StartOdoLogging()");

  // activate logging

  _genBase->_logOdometry = true;

  // send reply

  CSNDTELEGRAM snd (CMD_REPLY | CMD_STARTODOLOGGING);

  snd.PushS32 (0);   // status

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}

void CCLIENT::StopOdoLogging (void) const
{
  GTL ("CCLIENT::StopOdoLogging()");

  // stop logging

  _genBase->_logOdometry = false;

  //

  CSNDTELEGRAM snd (CMD_REPLY | CMD_STOPODOLOGGING);

  snd.PushS32 (0);   // status

  //

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      throw -1;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::GetBatteryVoltage (void) const
{
  GTL ("CCLIENT::GetBatteryVoltage()");

  // Spannung der Batterie ermitteln

  double voltage = _genBase->GetBatteryVoltage ();

  // Telegramm erstellen

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETBATTERYVOLTAGE);

  snd.PushS32 (0);         // Status
  snd.PushF32 (voltage);   // Spannung

  // Telegramm senden

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
  {
    GTLFATAL (("SEND() failed\n"));
    throw -1;
  }
}


void CCLIENT::GetDriveTemperatures (void) const
{
  GTL ("CCLIENT::GetDriveTemperatures()");

  // Temperaturen dern Antriebe ermitteln

  double* temperature = _genBase->GetDriveTemperatures ();

  //printf ("%f\t%f\n", temperature[0], temperature[1]);

  // Telegramm erstellen

  CSNDTELEGRAM snd (CMD_REPLY | CMD_GETDRIVETEMPERATURES);

  snd.PushS32 (0);              // Status
  snd.PushF32 (temperature[0]); // linker Antrieb
  snd.PushF32 (temperature[1]); // rechter Antrieb

  // Telegramm senden

  if (snd.Send ((CSTREAMSOCK *)_sock) < 0)
  {
    GTLFATAL (("SEND() failed\n"));
    throw -1;
  }
}
