//
// mobileCalibrateEvaluateImages2.cc
// (C) 11/2003 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//
// second version, averages the position as reported by the localization and
// the coordinates of the regions of the marker. computes a new model from the
// averaged coordinates and associates that with the averaged position. then
// computes displacements based on these averaged values.
//
// is (rather) invariant against errors in the initial (first) position and
// therefore better allows to detect errors in the localization.
//

#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread/tracelog.h"
#include "vision/model.h"
#include "vision/modelClassifier.h"

#undef VERBOSE


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline double sqr (const double arg)
{
  return arg * arg;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if 0

class CAVERAGE
{
private:

  double cnt, sum, sumq;

public:

  CAVERAGE (void)
  {
    cnt = 0.0;
    sum = 0.0;
    sumq = 0.0;
  }

  virtual ~CAVERAGE (void)
  {
  }

  void Add (const double arg)
  {
    cnt += 1.0;
    sum += arg;
    sumq += sqr (arg);
  }

  double Avg (void) const
  {
    return sum / cnt;
  }

  double Dev (void) const
  {
    const double avg = sum / cnt;
    const double ret = sumq / cnt - sqr (avg);

    return ret >= 0 ? sqrt (ret) : 0.0;
  }
};

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CARRAY
{
private:

public:

  CARRAY (void)
  {
  }

  virtual ~CARRAY (void)
  {
  }

  // cannot support anything but a `const operator' because we don't know how
  // the array is implemented - it may not be in writable memory.

  virtual const double operator[] (const int idx) const = 0x0;
};


///////////////////////////////////////////////////////////////////////////////
//
// CDYNARRAY
//
// a dynamically growing one-dimensional array. internally computes the average
// and standard deviation of its data.
//
///////////////////////////////////////////////////////////////////////////////

class CDYNARRAY : public CARRAY
{
private:

  double cnt, sum, sumq;

  int _size, _used;
  double *_ptr;

public:

  CDYNARRAY (void)
  {
    cnt = 0.0;
    sum = 0.0;
    sumq = 0.0;

    _size = _used = 0;
    _ptr = 0x0;
  }

  virtual ~CDYNARRAY (void)
  {
    if (_ptr)
      {
	free (_ptr);
      }
  }

  int GetSize (void) const
  {
    return _used;
  }

  double Add (const double arg);

  double Avg (void) const
  {
    return sum / cnt;
  }
  
  void RemoveAverage (void)
  {
    const double avg = sum / cnt;

    sum = sumq = 0.0;

    for (int i=0; i<_used; i++)
      {
	_ptr[i] -= avg;

	sum += _ptr[i];
	sumq += sqr (_ptr[i]);
      }
  }

  void RemoveAverage (double &avg)
  {
    avg = sum / cnt;

    sum = sumq = 0.0;

    for (int i=0; i<_used; i++)
      {
	_ptr[i] -= avg;

	sum += _ptr[i];
	sumq += sqr (_ptr[i]);
      }
  }

  double Dev (void) const
  {
    const double avg = sum / cnt;
    const double ret = sumq / cnt - sqr (avg);

    return ret >= 0 ? sqrt (ret) : 0.0;
  }

  virtual const double operator[] (const int idx) const;
};


double CDYNARRAY::Add (const double arg)
{
  GTL ("CDYNARRAY::Add()");

  const int idx = _used++;

  if (_used > _size)
    {
      _size += 10;

      if (!(_ptr = (double*)realloc (_ptr, _size * sizeof (double))))
	{
	  GTLFATAL (("out of memory\n"));
	  abort ();
	}
    }

  cnt += 1.0;
  sum += arg;
  sumq += sqr (arg);

  return _ptr[idx] = arg;
}


const double CDYNARRAY::operator[] (const int idx) const
{
  GTL ("CDYNARRAY::operator[]()");

  if ((idx < 0) || (idx >= _used))
    {
      GTLFATAL (("index %i out of range 0..%i\n", idx, _used));
      abort ();
    }

  return _ptr[idx];
}


///////////////////////////////////////////////////////////////////////////////
//
// CDYNDEGREEARRAY
//
// see CDYNARRAY, only that it stores the sines and cosines of its data, which
// is supposed to be angles given in degrees.
//
// hint: you cannot compute the `average angle' given in either degrees,
// radians or whatever.
//
///////////////////////////////////////////////////////////////////////////////

class CDYNDEGREEARRAY : public CARRAY
{
private:

  CDYNARRAY _sin, _cos;

public:

  typedef double degree_t;

  CDYNDEGREEARRAY (void)
  {
  }

  virtual ~CDYNDEGREEARRAY (void)
  {
  }

  void Add (degree_t arg);

  degree_t Avg (void) const;

  virtual const double operator[] (const int idx) const;
};


void CDYNDEGREEARRAY::Add (degree_t arg)
{
  const double angle = M_PI * arg / 180.0;

  _sin.Add (sin (angle));
  _cos.Add (cos (angle));
}


CDYNDEGREEARRAY::degree_t CDYNDEGREEARRAY::Avg (void) const
{
  return 180.0 * atan2 (_sin.Avg (), _cos.Avg ()) / M_PI;
}


const double CDYNDEGREEARRAY::operator[] (const int idx) const
{
  GTL ("CDYNDEGREEARRAY::operator[]()");

  if ((idx < 0) || (idx >= _sin.GetSize ()))
    {
      GTLFATAL (("index out of range\n"));
      abort ();
    }

  return 180.0 * atan2 (_sin[idx], _cos[idx]) / M_PI;
}


///////////////////////////////////////////////////////////////////////////////
//
// CPOSARRAY
//
// a dynamic array of 2d-positions, say, two linear coordinates `x' and `y' and
// an angle `a'.
//
///////////////////////////////////////////////////////////////////////////////

class CPOSARRAY
{
private:

  CDYNARRAY _x, _y;
  CDYNDEGREEARRAY _a;

public:

  typedef double meter_t;
  typedef double degree_t;

  CPOSARRAY (void)
  {
  }

  virtual ~CPOSARRAY (void)
  {
  }

  int GetSize (void) const
  {
    return _x.GetSize ();
  }

  void Add (const meter_t x,
	    const meter_t y,
	    const degree_t a)
  {
    _x.Add (x);
    _y.Add (y);
    _a.Add (a);
  }

  void Avg (meter_t &x, meter_t &y, degree_t &a) const
  {
    x = _x.Avg ();
    y = _y.Avg ();
    a = _a.Avg ();
  }

  void Dev (meter_t &x, meter_t &y, degree_t &a) const
  {
    x = _x.Dev ();
    y = _y.Dev ();
    a = -1.0; // _a.Dev ();
  }

  void RemoveAverage (meter_t &x, meter_t &y, degree_t &a)
  {
    _x.RemoveAverage (x);
    _y.RemoveAverage (y);
    a = _a.Avg ();   // _a.RemoveAverage (a);
  }

  CARRAY &CPOSARRAY::operator[] (const int idx);
};


CARRAY &CPOSARRAY::operator[] (const int idx)
{
  GTL ("CPOSARRAY::operator[]()");

  if (idx == 0)
    {
      return _x;
    }

  if (idx == 1)
    {
      return _y;
    }

  if (idx == 2)
    {
      return _a;
    }

  GTLFATAL (("index out of range\n"));
  abort ();
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static char *readline (double &px, double &py, double &pa,
		       double &dx, double &dy, double &da,
		       FILE *fp)
{
  static char buf[256];
  char *ret;

  while ((ret = fgets (buf, 256, fp)))
    {
      if (buf[0] != '#')
	{
	  break;
	}
    }

  if (ret)
    {
      sscanf (buf, "%lf %lf %lf %lf %lf %lf", &px, &py, &pa, &dx, &dy, &da);
    }

  return ret;
}


static char *readline (double &px, double &py, double &pa,
		       FILE *fp)
{
  static char buf[256];
  char *ret;

  while ((ret = fgets (buf, 256, fp)))
    {
      if (buf[0] != '#')
	{
	  break;
	}
    }

  if (ret)
    {
      sscanf (buf, "%lf %lf %lf", &px, &py, &pa);
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
// modelScale() - returns millimeter/pixel
//
// works only if the model consists only of circles which are 20mm away from
// the center. if one of the circles 40mm away is chosen it will compute
// garbage.
//
///////////////////////////////////////////////////////////////////////////////

static void modelScale (double &xScale, double &yScale, const CMODEL &model)
{
  GTLC ("modelScale()");

  if (!model._matched)
    {
      GTLFATAL (("model not yet matched\n"));
      abort ();
    }

  // find `blue' (center) pixel

  int blue;

  for (blue=0; blue<model._numParts; blue++)
    {
      if (model._part[blue].color == 25)
	{
	  break;
	}
    }

  // check horizontal/vertical pixels

  double px = 0.0, py = 0.0;
  double pxCnt = 0.0, pyCnt = 0.0;

  for (int h=0; h<model._numParts; h++)
    {
      if (model._matched[h] && (h != blue))
	{
	  const double dx = fabs (model._part[h].xm - model._part[blue].xm);
	  const double dy = fabs (model._part[h].ym - model._part[blue].ym);

	  if (dx > dy)   // obviously horizontal
	    {
	      px += dx;
	      pxCnt += 1.0;
	    }
	  else   // obviously vertical
	    {
	      py += dy;
	      pyCnt += 1.0;
	    }
	}
    }

  px /= pxCnt;
  py /= pyCnt;

  //

  xScale = 20.0 / px;   // 20mm distance between circles
  yScale = 20.0 / py;
}


///////////////////////////////////////////////////////////////////////////////
//
// computeCameraPosition()
//
// apply (approximate) offset of camera to platform base position. this is not
// meant to be really accurate, just so that it yields a position distribution
// with a larger standard deviation.
//
// camera is mounted at the forward right corner, app. x=+297mm y=-270mm in
// platform coordinates.
//
///////////////////////////////////////////////////////////////////////////////

static void computeCameraPosition (double &x,
				   double &y,
				   const double &a)
{
  const double angle = (a - 42.2) * M_PI / 180.0;
  const double dist = 0.4018;

  const double xc = x + dist * cos (angle);
  const double yc = y + dist * sin (angle);

#if 0
  printf ("# %f %f %f -> %f %f -> %f %f %f\n",
	  x, y, a,
	  cos (a), sin (a),
	  xc, yc, a);
#endif

  x = xc;
  y = yc;
}


///////////////////////////////////////////////////////////////////////////////
//
// computeLocalizationDeviation()
//
// computes the platform deviation in platform coordinates (mm, °), given world
// coordinates.
//
///////////////////////////////////////////////////////////////////////////////

static void computeLocalizationDeviation (double &xErrPltf,
					  double &yErrPltf,
					  double &aErrPltf,
					  const double px0,
					  const double py0,
					  const double pa0,
					  const double px,
					  const double py,
					  const double pa)
{
  const double xErr = 1000.0 * (px - px0);
  const double yErr = 1000.0 * (py - py0);

  const double a = M_PI * pa0 / 180.0;
  const double c = cos (a);
  const double s = sin (a);

  xErrPltf =  c * xErr + s * yErr;
  yErrPltf = -s * xErr + c * yErr;

  //printf ("# %f %f @ %f°-> %f %f\n", xErr, yErr, pa0, xErrPltf, yErrPltf);

  aErrPltf = pa - pa0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void computeCameraDisplacement (double &xErr,
				       double &yErr,
				       double &aErr,
				       const CMODEL &orig,
				       CIMAGE &image)
{
  GTLC ("computeCameraDisplacement()");

  //

  image.Show ();

  //

  CMODEL model ("calib");

  static CIMAGE regionImage;

  model.AcquireAutomatic (image, &regionImage);

  model.MarkAllRegions (image, regionImage);

  regionImage.Show ();

  //

  CMODELCLASSIFIER classifier;

  classifier.AddModel (new CMODEL (orig));

  CMODELCLASSIFIER::RESULT result;

  if (classifier.Analyze (model, 0, &result) < 0)
    {
      GTLFATAL (("failed to analyze model, press <RETURN>\n"));
      getchar ();
      abort ();
    }

  model.MarkMatchedRegions (regionImage);

  regionImage.Show ();

  //

  double xScale, yScale;

  modelScale (xScale, yScale, model);

#if 1 // coordinate system is rotated
  xErr = result.dy * yScale;
  yErr = result.dx * xScale;
#else
  xErr = result.dx * xScale;
  yErr = result.dy * yScale;
#endif
  aErr = 180.0 * result.angle / M_PI;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{
#if 0
  _globGtlEnable = 1;
#endif

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <namepref>\n", argv[0]);
      fprintf (stderr, "\twhere:\n");
      fprintf (stderr, "\t<namepref> is the prefix of the logfile (including paths)\n");
      return -1;
    }

  GTLC ("main()");

  //
  //
  //

  const int maxNameLen = strlen (argv[1]) + 16;
  char fName[maxNameLen];

  snprintf (fName, maxNameLen, "%s.log", argv[1]);

  FILE *fp = fopen (fName, "r");

  if (!fp)
    {
      GTLFATAL (("file \"%s\" not found\n", fName));
      abort ();
    }

  //
  // get initial data
  //

  // localization position

  double pxPltf0, pyPltf0, paPltf0;

  if (!readline (pxPltf0, pyPltf0, paPltf0, fp))
    {
      GTLFATAL (("failed to read file\"%s\"\n", fName));
      abort ();
    }

  double pxCam0 = pxPltf0, pyCam0 = pyPltf0, paCam0 = paPltf0;

  computeCameraPosition (pxCam0, pyCam0, paCam0);

  // image

  int cnt = 0;

  snprintf (fName, maxNameLen, "%s-%03i.ppm.gz", argv[1], cnt++);

  CIMAGE image;

  if (image.LoadPPM (fName) < 0)
    {
      GTLFATAL (("failed to load image \"%s\"\n", fName));
      abort ();
    }

  image.Show ();

  CMODEL model0 ("calib");

  model0.AcquireManual (image);

  CMODELCLASSIFIER classifier;

  classifier.AddModel (new CMODEL (model0));

  //
  // now enter the loop
  //

  CPOSARRAY posPltf, posCam, devVision;

  double px, py, pa;

  while (readline (px, py, pa, fp))
    {
      double xErr, yErr, aErr;

      // base position [m]

#ifdef VERBOSE
      printf ("platform     : %+8.5f %+8.5f %+8.4f\n", px, py, pa);
      computeLocalizationDeviation (xErr, yErr, aErr,
				    pxPltf0, pyPltf0, paPltf0,
				    px, py, pa);
      printf ("platform dev : %+8.5f %+8.5f %+8.4f\n", xErr, yErr, aErr);
#endif
      posPltf.Add (px, py, pa);

      // camera position [m]

      computeCameraPosition (px, py, pa);
#ifdef VERBOSE
      printf ("camera       : %+8.5f %+8.5f %+8.4f\n", px, py, pa);
      computeLocalizationDeviation (xErr, yErr, aErr,
				    pxCam0, pyCam0, paCam0,
				    px, py, pa);
      printf ("camera dev   : %+8.5f %+8.5f %+8.4f\n", xErr, yErr, aErr);
#endif
      posCam.Add (px, py, pa);

      // image

      snprintf (fName, maxNameLen, "%s-%03i.ppm.gz", argv[1], cnt++);

      if (image.LoadPPM (fName) < 0)
	{
	  GTLFATAL (("failed to load image \"%s\"\n", fName));
	  abort ();
	}

      image.Show ();

      static CIMAGE regionImage;

      CMODEL model ("calib");

      model.AcquireAutomatic (image, &regionImage);

      model.MarkAllRegions (image, regionImage);

      regionImage.Show ();

      //

      CMODELCLASSIFIER::RESULT result;

      if (classifier.Analyze (model, 0, &result) < 0)
	{
	  GTLFATAL (("failed to analyze model, press <RETURN>\n"));
	  getchar ();
	  abort ();
	}

      model.MarkMatchedRegions (regionImage);

      regionImage.Show ();

      computeCameraDisplacement (xErr, yErr, aErr,
				 model0,
				 image);
#ifdef VERBOSE
      printf ("vision dev   : %+8.5f %+8.5f %+8.4f\n\n", xErr, yErr, aErr);
#endif
      devVision.Add (xErr, yErr, aErr);
    }

  fclose (fp);

  //
  // remove averages
  //

  printf ("# initial platform position [m/°]     : %+8.5f %+8.5f %+8.4f\n",
	  pxPltf0, pyPltf0, paPltf0);

  double xAvg, yAvg, aAvg, xDev, yDev, aDev;

  posPltf.RemoveAverage (xAvg, yAvg, aAvg);
  posPltf.Dev (xDev, yDev, aDev);
  printf ("# average platform position [m/°]     : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xDev, yDev, aDev);

  posCam.Avg (xAvg, yAvg, aAvg);
  posCam.Dev (xDev, yDev, aDev);
  printf ("# average camera position [m/°]       : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xDev, yDev, aDev);

  CPOSARRAY devCam;

  for (int i=0; i<cnt-1; i++)
    {
      double xErr, yErr, aErr;

      computeLocalizationDeviation (xErr, yErr, aErr,
				    xAvg, yAvg, aAvg,
				    posCam[0][i], posCam[1][i], posCam[2][i]);

      devCam.Add (xErr, yErr, aErr);

#ifdef VERBOSE
      printf ("camera dev   : %+8.5f %+8.5f %+8.4f\n", xErr, yErr, aErr);
      getchar ();
#endif
    }

  //

  devVision.RemoveAverage (xAvg, yAvg, aAvg);
  devVision.Dev (xDev, yDev, aDev);
  printf ("# average vision deviation [mm/°]     : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xDev, yDev, aDev);

  printf ("# -----------------------------------------------------------------------------\n");
  printf ("# now building differences\n");
  printf ("# - between camera position and average camera position -> camera deviation\n");
  printf ("# - between vision deviation and average vision deviation -> (new) vision deviation\n");
  printf ("# -----------------------------------------------------------------------------\n");

  devCam.RemoveAverage (xAvg, yAvg, aAvg);
  double xdCam, ydCam, adCam;
  devCam.Dev (xdCam, ydCam, adCam);
  printf ("# average camera deviation [mm/°]     : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xdCam, ydCam, adCam);

  devVision.Avg (xAvg, yAvg, aAvg);
  double xdVision, ydVision, adVision;
  devVision.Dev (xdVision, ydVision, adVision);
  printf ("# average vision deviation [mm/°]     : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xdVision, ydVision, adVision);

  printf ("# -----------------------------------------------------------------------------\n");

  //
  //
  //

  CPOSARRAY devDiff;

  for (int i=0; i<cnt-1; i++)
    {
      px = devCam[0][i] - devVision[0][i];
      py = devCam[1][i] - devVision[1][i];
      pa = devCam[2][i] - devVision[2][i];

      devDiff.Add (px, py, pa);
    }

  devDiff.RemoveAverage (xAvg, yAvg, aAvg);
  double xdDiff, ydDiff, adDiff;
  devDiff.Dev (xdDiff, ydDiff, adDiff);
  printf ("# average difference [mm/°]           : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xdDiff, ydDiff, adDiff);

  //
  //
  //

  printf ("# -----------------------------------------------------------------------------\n");

  printf ("# generating gnuplot data (%s.gp_data)\n", argv[1]);

  const double scatterLimit = 20.0;
  int scatterDrop = 0;

  snprintf (fName, maxNameLen, "%s.gp_data", argv[1]);

  if (!(fp = fopen (fName, "w")))
    {
      printf ("# fopen() failed\n");
    }
  else
    {
      for (int i=0; i<cnt-1; i++)
	{
	  fprintf (fp,
		   "%f %f %f   %f %f %f   %f %f %f\n",
		   devCam[0][i],
		   devCam[1][i],
		   devCam[2][i],
		   devVision[0][i],
		   devVision[1][i],
		   devVision[2][i],
		   devDiff[0][i],
		   devDiff[1][i],
		   devDiff[2][i]);

	  if ((fabs (devCam[0][i]) > scatterLimit) ||
	      (fabs (devCam[1][i]) > scatterLimit) ||
	      (fabs (devVision[0][i]) > scatterLimit) ||
	      (fabs (devVision[1][i]) > scatterLimit))
	    {
	      scatterDrop++;
	    }
	}

      fclose (fp);
    }

  //
  // compute correlation between vision and localization (camera) deviation.
  //

  double xCorr, yCorr;

  {
    double sumX = 0.0, sumY = 0.0;

    // covariance cov(Xi,Xj) = E(Xi*Xj) - E(Xi)*E(Xj), but E(Xi)=E(Xj)=0 here

    for (int i=0; i<cnt-1; i++)
      {
	sumX += devVision[0][i] * devCam[0][i];   // cov
	sumY += devVision[1][i] * devCam[1][i];
      }

    sumX /= (double)cnt - 1.0;
    sumY /= (double)cnt - 1.0;

    // correlation = cov(Xi,Xj) / (dev(Xi)*dev(Xj))

    xCorr = sumX / (xdVision * xdCam);
    yCorr = sumY / (ydVision * ydCam);
  }

  printf ("# correlation coefficients: %f %f\n", xCorr, yCorr);

  //
  //
  //

  printf ("# generating gnuplot commands (%s.gp_cmd)\n", argv[1]);

  snprintf (fName, maxNameLen, "%s.gp_cmd", argv[1]);

  if (!(fp = fopen (fName, "w")))
    {
      printf ("# fopen() failed\n");
    }
  else
    {
      // scatter plot

      fprintf (fp, "set key box\n");
      fprintf (fp, "set parametric\n");
      //fprintf (fp, "set size square\n");
      fprintf (fp, "set size ratio -1\n");
      fprintf (fp, "set grid\n");
      fprintf (fp, "set xtics 2\n");
      fprintf (fp, "set ytics 2\n");
      fprintf (fp, "set title \"scatter plot (%i extreme outlier%s removed)\"\n", scatterDrop, scatterDrop != 1 ? "s" : "");
      fprintf (fp, "set xlabel \"x-axis [mm]\"\n");
      fprintf (fp, "set ylabel \"y-axis [mm]\"\n");

      fprintf (fp, "set term postscript eps color solid\n");
      fprintf (fp, "set output \"%s-scatter.eps\"\n", argv[1]);

      fprintf (fp, "set xrange [%.0f:%.0f]\n", -scatterLimit, scatterLimit);
      fprintf (fp, "set yrange [%.0f:%.0f]\n", -scatterLimit, scatterLimit);

      fprintf (fp, "plot \"%s.gp_data\" using 1:2 title \"camera deviation\""
	       ", \"%s.gp_data\" using 4:5 title \"vision deviation\""
	       ", \"%s.gp_data\" using 7:8 title \"difference\""
#if 1
	       ", %f*sin(t),%f*cos(t) lt 1"
	       ", %f*sin(t),%f*cos(t) lt 2"
	       ", %f*sin(t),%f*cos(t) lt 3\n"
#else
	       ", %f*sin(t),%f*cos(t) title \"\" lt 1"
	       ", %f*sin(t),%f*cos(t) title \"\" lt 2"
	       ", %f*sin(t),%f*cos(t) title \"\" lt 3\n"
#endif
	       ,
	       argv[1], argv[1], argv[1],
	       xdCam, ydCam,
	       xdVision, ydVision,
	       xdDiff, ydDiff);


      // correlation plot

      fprintf (fp, "set noparametric\n");
      fprintf (fp, "set xtics 5\n");
      fprintf (fp, "set ytics 5\n");
      fprintf (fp, "set title \"correlation plot (extreme outliers removed)\"\n");
      fprintf (fp, "set xlabel \"vision displacement [mm]\"\n");
      fprintf (fp, "set ylabel \"localization displacement [mm]\"\n");
      fprintf (fp, "set key bottom right\n");
      fprintf (fp, "set output \"%s-corr.eps\"\n", argv[1]);
      fprintf (fp, "plot [-20:20] [-20:20] \"%s.gp_data\" using 4:1 title \" x-axis (corr=%f)\", x title \"\", \"%s.gp_data\" using 5:2 title \" y-axis (corr=%f)\"\n"
	       , argv[1], xCorr, argv[1], yCorr);

      fclose (fp);
    }

  //
  //
  //

  printf ("# executing gnuplot commands\n");

  char buf[128];
  snprintf (buf, 128, "gnuplot %s.gp_cmd", argv[1]);
  system (buf);

  printf ("# converting eps to pdf\n");

  snprintf (buf, 128, "epstopdf %s-scatter.eps", argv[1]);
  system (buf);
  snprintf (buf, 128, "epstopdf %s-corr.eps", argv[1]);
  system (buf);


  return 0;
}
