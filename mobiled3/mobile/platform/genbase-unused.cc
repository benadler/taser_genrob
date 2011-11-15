/////////////////////////////////////////////////////////////////////////////
//
// localization related stuff
//
/////////////////////////////////////////////////////////////////////////////

private:

mutable long _xLast, _yLast, _aLast;

mutable double _cntLSF;
mutable double _xLSFs, _yLSFs, _aLSFs;
mutable double _xLSFqs, _yLSFqs, _aLSFqs;

public:

// ++itschere20011015: GetFrameLSF() - computes the precise position based on
// least squares fit of laser marks. uses internal averaging as long as the
// position as reported by GetFrame() doesn't change. only to be called when
// the mobile is standing still. needs at least two marks. returns the number
// of marks used for computation. returns -1 in case no position could be
// computed.

int GetFrameLSF (double &x, double &y, double &a,
		 double *eGenRobPtr = 0x0, double *eTeSchePtr = 0x0) const;

/////////////////////////////////////////////////////////////////////////////
//
// hi-level motion related stuff
//
/////////////////////////////////////////////////////////////////////////////

// ++itschere20011025: MoveAccurate() starts a motion to the target, waits
// until the target is reached with respect to a catching radius and the
// platform is standing still, then tries to compute the precise position
// using a least square fit on the laser marks (with gencontrol as fallback)
// and thus the positioning error. this error may be taken into account by
// outer functions to compensate for KNOWN parts of malpositioning. the
// unknown parts (due to wrong triangulation) are still unhandled...

int MoveAccurate (const double x, const double y, const double a,
		  double *xErr, double *yErr, double *aErr) const;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CGENBASE::GetFrameLSF (double &xLSF, double &yLSF, double &aLSF,
			   double *eGenRobPtr, double *eTeSchePtr) const
{
  GTL ("CGENBASE::GetFrameLSF()");

  //

  GetFrame (xLSF, yLSF, aLSF);   // in case everything below here should fail have some defaults

  CSCAN scan1;
  Scan (scan1, FRONT);

  CSCAN scan2;
  Scan (scan2, REAR);

  int numMarks = scan1.numMarkers + scan2.numMarkers;

  if (numMarks < 2)
    {
      GTLPRINT (("not enough marks found, using fallback position\n"));
      return -1;
    }

  //
  // reset rolling average if current (estimated) position of mobile has
  // changed
  //

  long x = (long)floor (xLSF * 1000.0 + 0.5);   // 0.001 m
  long y = (long)floor (yLSF * 1000.0 + 0.5);   // 0.001 m
  long a = (long)floor (aLSF *   10.0 + 0.5);   // 0.1 °

  GTLPRINT (("pos (last) = %i,%i,%i\n", x, y, a));
  GTLPRINT (("pos (last) = %i,%i,%i\n", _xLast, _yLast, _aLast));

  if ((x != _xLast) || (y != _yLast) || (a != _aLast))
    {
      _xLast = x;
      _yLast = y;
      _aLast = a;

      _cntLSF = 0.0;
      _xLSFs = _yLSFs = _aLSFs = 0.0;
      _xLSFqs = _yLSFqs = _aLSFqs = 0.0;
    }

  //
  //
  //

  typedef struct
  {
    int idx;
    float dist;
  } ASSOC;

  static ASSOC *assoc = 0x0;

  if (!assoc)
    {
      if (!(assoc = (ASSOC *)malloc (_map.numMarks * sizeof (ASSOC))))
	{
	  GTLFATAL (("out of memory\n"));
	  abort ();
	}
    }

  memset (assoc, 255, _map.numMarks * sizeof (ASSOC));

  //
  // rearrange image marks into one big array (iVec)
  //

  CVEC iVecTmp (2 * numMarks);

  int i, idx = 0;

  for (i=0; i<scan1.numMarkers; i++)
    {
      iVecTmp[2*idx+0] = scan1.mx[i];
      iVecTmp[2*idx+1] = scan1.my[i];
      idx++;
    }

  for (i=0; i<scan2.numMarkers; i++)
    {
      iVecTmp[2*idx+0] = scan2.mx[i];
      iVecTmp[2*idx+1] = scan2.my[i];
      idx++;
    }

  //
  // associate image marks (iVec) with model marks (mVec) on the map
  //

  int falseMarks = 0;

  CVEC mVecTmp (2 * numMarks);

  for (i=0; i<numMarks; i++)
    {
      double ix = iVecTmp[2*i+0];
      double iy = iVecTmp[2*i+1];
      double dMin = -1.0;
      int dMinIdx = -1;

      // search nearest mark in map

      int j;

#ifdef USE_MOVEABLE_MARKS
      CVEC mapMarks;
      GetMarks (mapMarks);
#else
      CVEC mapMarks (_map.numMarks, _map.mark);
#endif

      for (j=0; j<_map.numMarks; j++)
	{
	  double x = mapMarks[2*j+0];
	  double y = mapMarks[2*j+1];

	  double d = sqr (x - ix) + sqr (y - iy);

	  if ((dMin < 0) || (d < dMin))
	    {
	      dMin = d;
	      dMinIdx = j;
	    }
	}

      if (assoc[dMinIdx].idx < 0)
	{
	  GTLPRINT (("iMark %i (%.3f,%.3f) mapped to mMark %i (%.3f,%.3f) at dist %f\n",
		     i, ix, iy,
		     dMinIdx, mapMarks[2*dMinIdx+0], mapMarks[2*dMinIdx+1],
		     dMin));

	  assoc[dMinIdx].idx = i;
	  assoc[dMinIdx].dist = dMin;
	}
      else if (dMin < assoc[dMinIdx].dist)
	{
	  GTLPRINT (("iMark %i (%.3f,%.3f) improves mMark %i (%.3f,%.3f) from dist %f to %f\n",
		     i, ix, iy,
		     dMinIdx, mapMarks[2*dMinIdx+0], mapMarks[2*dMinIdx+1],
		     assoc[dMinIdx].dist, dMin));

	  falseMarks++;
	  assoc[dMinIdx].idx = i;
	  assoc[dMinIdx].dist = dMin;
	}
      else
	{
	  GTLPRINT (("iMark %i (%.3f,%.3f) would worsen mMark %i (%.3f,%.3f) from %f to %f\n",
		     i, ix, iy,
		     dMinIdx, mapMarks[2*dMinIdx+0], mapMarks[2*dMinIdx+1],
		     dMin, assoc[dMinIdx].dist));

	  falseMarks++;
	}

      mVecTmp[2*i+0] = mapMarks[2*dMinIdx+0];
      mVecTmp[2*i+1] = mapMarks[2*dMinIdx+1];
    }

  //

  numMarks -= falseMarks;

  CVEC iVec (2 * numMarks);
  CVEC mVec (2 * numMarks);

  idx = 0;

  for (i=0; i<_map.numMarks; i++)
    {
      int mIdx = assoc[i].idx;

      if (mIdx >= 0)
	{
	  iVec[2*idx+0] = iVecTmp[2*mIdx+0];
	  iVec[2*idx+1] = iVecTmp[2*mIdx+1];
	  mVec[2*idx+0] = mVecTmp[2*mIdx+0];
	  mVec[2*idx+1] = mVecTmp[2*mIdx+1];
	  idx++;
	}
    }

  //

  double eGenRob = (iVec - mVec).Norm ();

  if (eGenRobPtr)
    {
      *eGenRobPtr = eGenRob;
    }

#if 1

  if (eGenRob > 0.2)
    {
      GTLPRINT (("wrong mark2map association in gencontrol?\n"));
    }

#endif

  //
  //
  //

  GTLPRINT (("number of legal marks = %i\n", numMarks));

  for (i=0; i<numMarks; i++)
    {
      GTLPRINT (("mark %i: (%.3f,%.3f) mapped to (%.3f,%.3f) at distance %f\n",
		 i+1,
		 iVec[2*i+0], iVec[2*i+1],
		 mVec[2*i+0], mVec[2*i+1],
		 sqrt (sqr (iVec[2*i+0] - mVec[2*i+0]) + sqr (iVec[2*i+1] - mVec[2*i+1]))));
    }

  //
  // map marks Mi are computed as Tb * Ti = Mi by gencontrol, where Tb is the
  // base trsf. what we need is Ti = Tb^-1 * Mi. note that this Ti is NOT the
  // map mark as seen in the mobile coordinate system.
  //

  for (i=0; i<numMarks; i++)
    {
#if 1
      const double a = 3.14159 * aLSF / 180.0;
      const double c = cos (a);
      const double s = sin (a);

      //printf ("# test %f,%f == ", iVec[2*i+0], iVec[2*i+1]);

      double ix =  c * iVec[2*i+0] + s * iVec[2*i+1] - (s * yLSF + c * xLSF);
      double iy = -s * iVec[2*i+0] + c * iVec[2*i+1] + (s * xLSF - c * yLSF);

      iVec[2*i+0] = ix;
      iVec[2*i+1] = iy;

      //ix = c * iVec[2*i+0] - s * iVec[2*i+1] + xLSF;
      //iy = s * iVec[2*i+0] + c * iVec[2*i+1] + yLSF;

      //printf ("%f,%f\n", ix, iy);

#else
      double dx = iVec[2*i+0] - xLSF;
      double dy = iVec[2*i+1] - yLSF;

      double a = atan2 (dy, dx);
      double l = sqrt (sqr (dx) + sqr (dy));

      iVec[2*i+0] = -l * cos (a - 3.14159 * aLSF / 180.0);
      iVec[2*i+1] = -l * sin (a - 3.14159 * aLSF / 180.0);
#endif
    }

  //
  //
  //

  CMAT m (2 * numMarks, 4);

  for (i=0; i<numMarks; i++)
    {
      m[2*i+0][0] = iVec[2*i+0];
      m[2*i+0][1] = -iVec[2*i+1];
      m[2*i+0][2] = 1.0;
      m[2*i+0][3] = 0.0;

      m[2*i+1][0] = iVec[2*i+1];
      m[2*i+1][1] = iVec[2*i+0];
      m[2*i+1][2] = 0.0;
      m[2*i+1][3] = 1.0;
    }

  //

  CMAT mi = (m.Trans () * m).Inv () * m.Trans ();

  CVEC p = mi * mVec;

  double xReally = p[2];
  double yReally = p[3];
  double aReally = 180.0 * atan2 (p[1], p[0]) / 3.14159;

  CVEC mCheck = m * p;

  double eTeSche = (mVec - mCheck).Norm ();

  if (eTeSchePtr)
    {
      *eTeSchePtr = eTeSche;
    }

#if 0
  GTLPRINT (("marks=%i : qual=%.3f, pos=%.3f,%.3f, ori=%.2f (qual=%f)\n",
	     numMarks,
	     eTeSche,
	     xReally, yReally,
	     aReally, sqrt (sqr (p[0]) + sqr (p[1]))));
#endif

  //
  //
  //

  _cntLSF += 1.0;
  _xLSFs += xReally;
  _yLSFs += yReally;
  _aLSFs += aReally;

  //

  xLSF = _xLSFs / _cntLSF;
  yLSF = _yLSFs / _cntLSF;
  aLSF = _aLSFs / _cntLSF;

#if 1

  _xLSFqs += sqr (xReally);
  _yLSFqs += sqr (yReally);
  _aLSFqs += sqr (aReally);

  double sx = sqrt (_xLSFqs / _cntLSF - sqr (xLSF));
  double sy = sqrt (_yLSFqs / _cntLSF - sqr (yLSF));
  double sa = sqrt (_aLSFqs / _cntLSF - sqr (aLSF));

  GTLPRINT (("cnt=(%.0f) pos=(%.4f,%.4f,%.4f) var=(%.5f,%.5f,%.5f)\n",
	     _cntLSF,
	     xLSF, yLSF, aLSF,
	     sx, sy, sa));

#endif

  return numMarks;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CGENBASE::MoveAccurate (const double x, const double y, const double a,
			    double *xErr, double *yErr, double *aErr) const
{
  GTL ("CGENBASE::MoveAccurate()");
  GTLPRINT (("%+.3f,%+.3f,%+.1f°\n", x, y, a));

  // go!

  GTLPRINT (("moving...\n"));

  double xLast, yLast , aLast;

  if (GetFrame (xLast, yLast, aLast) < 0)
    {
      GTLFATAL (("GetFrame() failed\n"));
      return -1;
    }

  fprintf (stderr, "\ngoing to    %f,%f,%f\n", x, y, a);

  if (Move (x, y, a) < 0)
    {
      GTLFATAL (("Move() failed\n"));
      return -1;
    }

  // wait until we're standing still near the target

  GTLPRINT (("waiting...\n"));

  double xReally, yReally, aReally;

  while (42)
    {
      // re-implementation of parts of `IsCompleted()' and `GetDistance()'

      GetFrameLSF (xReally, yReally, aReally);

      double dx = fabs (xReally - xLast);
      double dy = fabs (yReally - yLast);
      double da = fabs (aReally - aLast);

      xLast = xReally;
      yLast = yReally;
      aLast = aReally;

      double distXY = sqrt (sqr (xReally - _xe) + sqr (yReally - _ye));
      double distA = fabs (aReally - _ae);

      if (distA > 180.0)
	{
	  distA = fabs (distA - 360.0);
	}

      GTLPRINT (("distXY=%f, distA=%f, dx=%f, dy=%f, da=%f\n", distXY, distA, dx, dy, da));

      if ((distXY < 0.1) &&
	  (distA < 5.0) &&
	  (dx < 0.003) &&
	  (dy < 0.003) &&
	  (da < 0.2))
	{
	  break;
	}

      usleep (100000);
    }

  //

  if (SetMode (STANDBY) < 0)
    {
      GTLFATAL (("SetMode() failed\n"));
      return -1;
    }

  {
    double xe, ye, ae;
    GetFrame (xe, ye, ae);
    fprintf (stderr, "arrived at  %f,%f,%f\n", xe, ye, ae);
  }

  // compute the current position

  GTLPRINT (("computing current position...\n"));

#if 1

  if (GetFrame (xReally, yReally, aReally) < 0)
    {
      GTLFATAL (("GetFrameLSF() failed\n"));
      return -1;
    }

#else

  for (int i=0; i<20; i++)
    {
      usleep (100000);

      if (GetFrameLSF (xReally, yReally, aReally) < 0)
	{
	  GTLPRINT (("GetFrameLSF() failed (WARNING)\n"));
	}
    }

#endif

  fprintf (stderr, "taken to be %f,%f,%f\n\n", xReally, yReally, aReally);

  // compute the positioning error

  *xErr = xReally - x;
  *yErr = yReally - y;
  *aErr = aReally - a;

#if 1

  if (*aErr < -180.0)
    {
      *aErr += 360.0;
    }
  else if (*aErr > 180.0)
    {
      *aErr -= 360.0;
    }

#endif

  GTLPRINT (("computing positioning error: (%.4f,%.4f,%.3f)\n",
	     *xErr, *yErr, *aErr));

  return 0;
}
