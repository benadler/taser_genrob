//
// mobileCalibrateEvaluateImages.cc
// (C) 11/2003-03/2004 by TeSche (Torsten Scherer)
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

#include "arrays.h"

#undef VERBOSE


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static char *readline (double &px, double &py, double &pa,
		       char *fName,
		       FILE *fp)
{
  static char buf[256];
  char *ret;

  while ((ret = fgets (buf, 256, fp)))
    {
      if ((buf[0] != '#') &&
	  (buf[0] != 0) &&
	  (buf[0] != '\r') &&
	  (buf[0] != '\n'))
	{
	  break;
	}
    }

  if (ret)
    {
      double dx, dy, da;

      sscanf (buf,
	      "%lf %lf %lf %lf %lf %lf %s",
	      &px, &py, &pa,
	      &dx, &dy, &da,
	      fName);
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
				       CMODELCLASSIFIER &classifier,
				       CIMAGE &image)
{
  GTLC ("computeCameraDisplacement()");

  //

  image.Show ();

  //

  CMODEL model ("calib");

  static CIMAGE regionImage ("region image");

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

static void computeCovariance (double &covXX,
			       double &covXY,
			       double &covYY,
			       const CDYNARRAY &arrayX,
			       const CDYNARRAY &arrayY)
{
  GTLC ("computeCovariance()");

  const int cnt = arrayX.GetSize ();

  if (cnt != arrayY.GetSize ())
    {
      GTLFATAL (("arrays don't match\n"));
      abort ();
    }

  covXX = arrayX.Var ();
  covYY = arrayY.Var ();

  // covariance cov(Xi,Xj) = E(Xi*Xj) - E(Xi)*E(Xj)

  double sumX = 0.0;
  double sumY = 0.0;

  for (int i=0; i<cnt; i++)
    {
      sumX += arrayX[i] * arrayY[i];
      sumY += arrayY[i] * arrayX[i];
    }

  sumX /= (double)cnt;
  sumY /= (double)cnt;

  const double ex = arrayX.Avg ();
  const double ey = arrayY.Avg ();

  sumX -= ex * ey;
  sumY -= ex * ey;

  covXY = (sumX + sumY) / 2.0;   // paranoia
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void computeEigenvectors (double &ev1,
				 double &ev2,
				 double &angle,
				 const double vxx,
				 const double vxy,
				 const double vyy)
{
  GTLC ("computeEigenvectors()");

  const double vyx = vxy;

  //
  // p/q solution of the eigenvalues
  //

  const double p = - (vxx + vyy);
  const double q = vxx * vyy - vxy * vyx;

  const double d = sqr (p) / 4.0 - q;

  if (d <= 0.0)
    {
      GTLFATAL (("don't exist?\n"));
    }
  else
    {
      ev1 = -p / 2.0 + sqrt (d);
      ev2 = -p / 2.0 - sqrt (d);

      if ((ev1 < 0.001) || (ev2 < 0.001))
	{
	  GTLPRINT (("too small, distribution with no variance\n"));
	  return;
	}

      //printf ("    ev1 = %f (this should be the larger one)\n", ev1);
      //printf ("    ev2 = %f\n", ev2);

      //

      //printf ("    eigenvalue %f:\n", ev1);
      {
	const double fx = vxx - ev1 - vyx;
	const double fy = vyy - ev1 - vxy;

	//printf ("      fx = %f, fy = %f\n", fx, fy);

	if (fabs (fy) > 0.001)
	  {
	    //printf ("      y = %f * x\n", fx / fy);
	  }

	angle = atan2 (-fx, -fy);

	//printf ("      angle = %f° (against x-axis)\n", 180.0 * angle / M_PI);

#if 0
	image.SetColor (255);

	image.Ellipse ((WIDTH >> 1) + ex * SCALE,
		       (HEIGHT >> 1) + ey * SCALE,
		       angle,
		       SCALE * sqrt (e1),
		       SCALE * sqrt (e2));
#endif
      }
#if 0
      //

      printf ("    eigenvalue %f:\n", e2);
      {
	const double fx = vxx - e2 - vyx;
	const double fy = vyy - e2 - vxy;

	printf ("      fx = %f, fy = %f\n", fx, fy);

	if (fabs (fy) > 0.001)
	  {
	    printf ("      y = %f * x\n", fx / fy);
	  }

	printf ("      angle = %f° (against x-axis)\n",
		180.0 * atan2 (-fx, -fy) / M_PI);
      }
#endif
    }
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

  if ((argc != 3) && (argc != 4))
    {
      fprintf (stderr, "usage: %s <directory> <namepref> [<keys>]\n", argv[0]);
      fprintf (stderr, "\twhere:\n");
      fprintf (stderr, "\t<directory> is the working directory\n");
      fprintf (stderr, "\t<namepref> is the prefix of the logfile (including paths)\n");
      fprintf (stderr, "\t<keys> are the keys to use in manual acquisition\n");

      return -1;
    }

  char *keysGiven = 0x0;

  if (argc == 4)
    {
      keysGiven = argv[3];
    }

  GTLC ("main()");

  printf ("# %s %s %s\n", argv[1], argv[2], keysGiven);

  if (chdir (argv[1]) < 0)
    {
      GTLFATAL (("chdir(\"%s\") failed\n", argv[1]));
      abort ();
    }

  //
  //
  //

  const int maxNameLen = strlen (argv[2]) + 16;
  char fName[maxNameLen];

  snprintf (fName, maxNameLen, "%s.log", argv[2]);

  FILE *fp = fopen (fName, "r");

  if (!fp)
    {
      GTLFATAL (("file \"%s\" not found\n", fName));
      abort ();
    }

  //
  // now enter the loop
  //

  CIMAGE image ("image");
  CMODEL model0 ("calib");
  CMODELCLASSIFIER classifier;
  CPOSARRAY posPltf, posCam, devVis;
  double pxPltf0, pyPltf0, paPltf0;
  double pxCam0, pyCam0, paCam0;
  double px, py, pa;

  int cnt = 0;

  while (42)
    {
      if (!readline (px, py, pa, fName, fp))
	{
	  break;
	}

      if (++cnt == 1)
	{
	  pxPltf0 = px;
	  pyPltf0 = py;
	  paPltf0 = pa;

	  pxCam0 = pxPltf0;
	  pyCam0 = pyPltf0;
	  paCam0 = paPltf0;

	  computeCameraPosition (pxCam0, pyCam0, paCam0);
	}

      //
      // base position [m]
      //

      posPltf.Add (px, py, pa);

      double xErr, yErr, aErr;

      //#ifdef VERBOSE
      //  printf ("platform     : %+8.5f %+8.5f %+8.4f\n", px, py, pa);
      //  computeLocalizationDeviation (xErr, yErr, aErr,
      //    pxPltf0, pyPltf0, paPltf0,
      //    px, py, pa);
      //  printf ("platform dev : %+8.5f %+8.5f %+8.4f\n", xErr, yErr, aErr);
      //#endif

      //
      // camera position [m]
      //

      computeCameraPosition (px, py, pa);
      posCam.Add (px, py, pa);

      //#ifdef VERBOSE
      //  printf ("camera       : %+8.5f %+8.5f %+8.4f\n", px, py, pa);
      //  computeLocalizationDeviation (xErr, yErr, aErr,
      //    pxCam0, pyCam0, paCam0,
      //    px, py, pa);
      //  printf ("camera dev   : %+8.5f %+8.5f %+8.4f\n", xErr, yErr, aErr);
      //#endif

      //
      // vision
      //

      if (image.LoadPPM (fName) < 0)
	{
	  GTLFATAL (("failed to load image \"%s\"\n", fName));
	  abort ();
	}

      if (!keysGiven)
	{
	  image.Show ();
	}

      if (cnt == 1)
	{
	  if (keysGiven)
	    {
	      printf ("# keys given : %s\n", keysGiven);

	      model0.AcquireManual (image, &keysGiven);
	    }
	  else
	    {
	      char *keys = 0x0;

	      model0.AcquireManual (image, &keys);

	      printf ("# keys used : %s\n", keys);

	      free (keys);
	    }

	  classifier.AddModel (new CMODEL (model0));
	}

      static CIMAGE regionImage ("region image");

      CMODEL model ("calib");

      model.AcquireAutomatic (image, &regionImage);

      model.MarkAllRegions (image, regionImage);

      //regionImage.Show ();

      //

      CMODELCLASSIFIER::RESULT result;

      if (classifier.Analyze (model, 0, &result) < 0)
	{
	  GTLFATAL (("failed to analyze model, press <RETURN>\n"));
	  getchar ();
	  abort ();
	}

      model.MarkMatchedRegions (regionImage);

      if (!keysGiven)
	{
	  regionImage.Show ();
	}

      //
      // vision displacement [mm]
      //

      computeCameraDisplacement (xErr, yErr, aErr,
				 classifier,
				 image);
      devVis.Add (xErr, yErr, aErr);

      //#ifdef VERBOSE
      //  printf ("vision dev   : %+8.5f %+8.5f %+8.4f\n\n", xErr, yErr, aErr);
      //#endif
    }

  fclose (fp);

  printf ("# situations : %i\n", cnt);

  if (cnt < 3)
    {
      return -1;
    }

  /////////////////////////////////////////////////////////////////////////////
  //
  // remove averages
  //
  /////////////////////////////////////////////////////////////////////////////

  printf ("# initial platform position [m/°] : %+8.5f %+8.5f %+8.4f\n",
	  pxPltf0, pyPltf0, paPltf0);

  double xAvg, yAvg, aAvg, xDev, yDev, aDev;

  posPltf.RemoveAverage (xAvg, yAvg, aAvg);
  posPltf.Dev (xDev, yDev, aDev);
  printf ("# average platform position [m/°] : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xDev, yDev, aDev);
  // actually, `posPltf' is not needed. it's just printed for information.

  printf ("# -----------------------------------------------------------------------------\n");

  posCam.Avg (xAvg, yAvg, aAvg);

  CPOSARRAY devCam;

  for (int i=0; i<cnt; i++)
    {
      double xErr, yErr, aErr;

      // this one scales from `m' to `mm'
      computeLocalizationDeviation (xErr, yErr, aErr,
				    xAvg, yAvg, aAvg,
				    posCam.X[i], posCam.Y[i], posCam.A[i]);

      devCam.Add (xErr, yErr, aErr);

      //#ifdef VERBOSE
      //  printf ("camera dev   : %+8.5f %+8.5f %+8.4f\n", xErr, yErr, aErr);
      //  getchar ();
      //#endif
    }

  posCam.RemoveAverage (xAvg, yAvg, aAvg);
  posCam.Dev (xDev, yDev, aDev);
  printf ("# average camera position [m/°]   : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xDev, yDev, aDev);

  devCam.RemoveAverage (xAvg, yAvg, aAvg);
  printf ("# average camera deviation [mm/°] : %+8.5f %+8.5f %+8.4f (should be close to zero)\n",
	  xAvg, yAvg, aAvg);

  double dMax, aMax;
  devCam.FindMaximum (dMax, aMax);
  printf ("#        maximum deviation [mm/°] : %.1f & %.2f\n",
	  dMax, aMax);

  double xDevCam, yDevCam, aDevCam;
  devCam.Dev (xDevCam, yDevCam, aDevCam);
  printf ("#              standard deviation : %.2f & %.2f & %.3f\n",
	  xDevCam, yDevCam, aDevCam);

  double covXXCam, covXYCam, covYYCam;
  computeCovariance (covXXCam, covXYCam, covYYCam, devCam.X, devCam.Y);
  printf ("#                      covariance : %f & %f & %f & %f\n",
	  covXXCam, covXYCam, covXYCam, covYYCam);

  double ev1Cam, ev2Cam, evAngleCam;
  computeEigenvectors (ev1Cam, ev2Cam, evAngleCam,
		       covXXCam, covXYCam, covYYCam);
  printf ("#                     eigenvalues : [%f %f] @ %f°, sqrt() -> %.3f & %.3f (%f)\n",
	  ev1Cam, ev2Cam, 180.0 * evAngleCam / M_PI,
	  sqrt (ev1Cam), sqrt(ev2Cam), sqrt (ev1Cam + ev2Cam));

  //

  devVis.RemoveAverage (xAvg, yAvg, aAvg);
  devVis.Dev (xDev, yDev, aDev);
  printf ("# average vision deviation [mm/°] : %+8.5f %+8.5f %+8.4f   stddev : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg, xDev, yDev, aDev);

  //

  devVis.Avg (xAvg, yAvg, aAvg);
  printf ("# average vision deviation [mm/°] : %+8.5f %+8.5f %+8.4f (should be zero)\n",
	  xAvg, yAvg, aAvg);

  devVis.FindMaximum (dMax, aMax);
  printf ("#        maximum deviation [mm/°] : %.1f & %.2f\n",
	  dMax, aMax);

  double xDevVis, yDevVis, aDevVis;
  devVis.Dev (xDevVis, yDevVis, aDevVis);
  printf ("#              standard deviation : %.2f & %.2f & %.3f\n",
	  xDevVis, yDevVis, aDevVis);

  double covXXVis, covXYVis, covYYVis;
  computeCovariance (covXXVis, covXYVis, covYYVis, devVis.X, devVis.Y);
  printf ("#                      covariance : %f & %f & %f & %f\n",
	  covXXVis, covXYVis, covXYVis, covYYVis);

  double ev1Vis, ev2Vis, evAngleVis;
  computeEigenvectors (ev1Vis, ev2Vis, evAngleVis,
		       covXXVis, covXYVis, covYYVis);
  printf ("#                     eigenvalues : [%f %f] @ %f°, sqrt() -> %.3f & %.3f (%f)\n",
	  ev1Vis, ev2Vis, 180.0 * evAngleVis / M_PI,
	  sqrt (ev1Vis), sqrt(ev2Vis), sqrt (ev1Vis + ev2Vis));

  printf ("# -----------------------------------------------------------------------------\n");

  //
  //
  //

  CPOSARRAY devDiff;

  for (int i=0; i<cnt; i++)
    {
      px = devCam.X[i] - devVis.X[i];
      py = devCam.Y[i] - devVis.Y[i];
      pa = devCam.A[i] - devVis.A[i];

      devDiff.Add (px, py, pa);
    }

  devDiff.RemoveAverage (xAvg, yAvg, aAvg);
  printf ("# average difference [mm/°]       : %+8.5f %+8.5f %+8.4f\n",
	  xAvg, yAvg, aAvg);

  devDiff.FindMaximum (dMax, aMax);
  printf ("#        maximum deviation [mm/°] : %.1f & %.2f\n",
	  dMax, aMax);

  double xDevDif, yDevDif, aDevDif;
  devDiff.Dev (xDevDif, yDevDif, aDevDif);
  printf ("#              standard deviation : %.2f & %.2f & %.3f\n",
	  xDevDif, yDevDif, aDevDif);

  double covXXDif, covXYDif, covYYDif;
  computeCovariance (covXXDif, covXYDif, covYYDif, devDiff.X, devDiff.Y);
  printf ("#                      covariance : %f & %f & %f & %f\n",
	  covXXDif, covXYDif, covXYDif, covYYDif);

  double ev1Dif, ev2Dif, evAngleDif;
  computeEigenvectors (ev1Dif, ev2Dif, evAngleDif,
		       covXXDif, covXYDif, covYYDif);
  printf ("#                     eigenvalues : [%f %f] @ %f°, sqrt() -> %.3f & %.3f (%f)\n",
	  ev1Dif, ev2Dif, 180.0 * evAngleDif / M_PI,
	  sqrt (ev1Dif), sqrt(ev2Dif), sqrt (ev1Dif + ev2Dif));

  //
  //
  //

  printf ("# -----------------------------------------------------------------------------\n");

  printf ("# generating gnuplot data (%s.gp_data)\n", argv[2]);

  const double scatterLimitTrans = 20.0;
  const double scatterLimitRot = 0.5;
  int scatterDropTrans = 0;
  int scatterDropRot = 0;

  snprintf (fName, maxNameLen, "%s.gp_data", argv[2]);

  if (!(fp = fopen (fName, "w")))
    {
      printf ("# fopen() failed\n");
    }
  else
    {
      for (int i=0; i<cnt; i++)
	{
	  fprintf (fp,
		   "%f %f %f   %f %f %f   %f %f %f\n",
		   devCam.X[i],
		   devCam.Y[i],
		   devCam.A[i],
		   devVis.X[i],
		   devVis.Y[i],
		   devVis.A[i],
		   devDiff.X[i],
		   devDiff.Y[i],
		   devDiff.A[i]);

	  if ((fabs (devCam.X[i]) > scatterLimitTrans) ||
	      (fabs (devCam.Y[i]) > scatterLimitTrans) ||
	      (fabs (devVis.X[i]) > scatterLimitTrans) ||
	      (fabs (devVis.Y[i]) > scatterLimitTrans))
	    {
	      scatterDropTrans++;
	    }

	  if ((fabs (devCam.A[i]) > scatterLimitRot) ||
	      (fabs (devVis.A[i]) > scatterLimitRot) ||
	      (fabs (devDiff.A[i]) > scatterLimitRot))
	    {
	      scatterDropRot++;
	    }
	}

      fclose (fp);
    }

  //
  // compute correlation between vision and localization (camera) deviation.
  //

  double xCorr, xCorr2, yCorr, yCorr2, aCorr, aCorr2;

  {
    double sumX = 0.0, sumY = 0.0;
    double sumX2 = 0.0, sumY2 = 0.0;
    double sumA = 0.0, sumA2 = 0.0;

    // covariance cov(Xi,Xj) = E(Xi*Xj) - E(Xi)*E(Xj), but E(Xi)=E(Xj)=0 here

    for (int i=0; i<cnt; i++)
      {
	sumX  += devVis.X[i] * devCam.X[i];   // cov
	sumX2 += devVis.X[i] * devDiff.X[i];   // cov
	sumY  += devVis.Y[i] * devCam.Y[i];
	sumY2 += devVis.Y[i] * devDiff.Y[i];
	sumA  += devVis.A[i] * devCam.A[i];
	sumA2 += devVis.A[i] * devDiff.A[i];
      }

    sumX  /= (double)cnt;
    sumX2 /= (double)cnt;
    sumY  /= (double)cnt;
    sumY2 /= (double)cnt;
    sumA  /= (double)cnt;
    sumA2 /= (double)cnt;

    // correlation = cov(Xi,Xj) / (dev(Xi)*dev(Xj))

    xCorr  = sumX  / (xDevVis * xDevCam);
    xCorr2 = sumX2 / (xDevVis * xDevDif);
    yCorr  = sumY  / (yDevVis * yDevCam);
    yCorr2 = sumY2 / (yDevVis * yDevDif);
    aCorr  = sumA  / (aDevVis * aDevCam);
    aCorr2 = sumA2 / (aDevVis * aDevDif);
  }

  printf ("# correlation coefficients : %.3f & %.3f (xy-displacement loc/vis)\n",
	  xCorr, yCorr);
  printf ("#                            %.3f & %.3f (xy-displacement diff/vis, should be lower)\n",
	  xCorr2, yCorr2);
  printf ("#                          : %.3f (rotation loc/vis)\n", aCorr);
  printf ("#                          : %.3f (rotation diff/vis)\n", aCorr2);

  //
  //
  //

  printf ("# generating gnuplot commands (%s.gp_cmd)\n", argv[2]);

  snprintf (fName, maxNameLen, "%s.gp_cmd", argv[2]);

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
      fprintf (fp, "set xtics 5\n");
      fprintf (fp, "set ytics 5\n");
      fprintf (fp, "set title \"translational displacement scatter plot (%i extreme outlier%s removed)\"\n", scatterDropTrans, scatterDropTrans != 1 ? "s" : "");
      fprintf (fp, "set xlabel \"x-axis [mm]\"\n");
      fprintf (fp, "set ylabel \"y-axis [mm]\"\n");

      fprintf (fp, "set term postscript eps color solid\n");
      fprintf (fp, "set output \"%s-scatter-trans.eps\"\n", argv[2]);

      fprintf (fp, "set xrange [%.0f:%.0f]\n", -scatterLimitTrans, scatterLimitTrans);
      fprintf (fp, "set yrange [%.0f:%.0f]\n", -scatterLimitTrans, scatterLimitTrans);

      fprintf (fp, "plot \"%s.gp_data\" using 1:2 title \"LOC\""
	       ", \"%s.gp_data\" using 4:5 title \"VIS\""
	       ", \"%s.gp_data\" using 7:8 title \"DIFF\""
#if 1
	       // gedrehte parametrische kreise :-/
	       ", (%f*sin(-pi*t/5))*cos(%f)-(%f*cos(+pi*t/5))*sin(%f),(%f*sin(-pi*t/5))*sin(%f)+(%f*cos(+pi*t/5))*cos(%f) title \"LOC EV [%.2f %.2f] @ %+.1f deg\" lt 1"
	       ", (%f*sin(-pi*t/5))*cos(%f)-(%f*cos(+pi*t/5))*sin(%f),(%f*sin(-pi*t/5))*sin(%f)+(%f*cos(+pi*t/5))*cos(%f) title \"VIS EV [%.2f %.2f] @ %+.1f deg\" lt 2"
	       ", (%f*sin(-pi*t/5))*cos(%f)-(%f*cos(+pi*t/5))*sin(%f),(%f*sin(-pi*t/5))*sin(%f)+(%f*cos(+pi*t/5))*cos(%f) title \"DIFF EV [%.2f %.2f] @ %+.1f deg\" lt 3\n"

#else
	       ", %.3f*sin(t)%+.3f*cos(t),%.3f*cos(t)%+.3f*sin(t) lt 1"
	       ", %.3f*sin(t)%+.3f*cos(t),%.3f*cos(t)%+.3f*sin(t) lt 2"
	       ", %.3f*sin(t)%+.3f*cos(t),%.3f*cos(t)%+.3f*sin(t) lt 3\n"
	       //", %f*sin(t),%f*cos(t) title \"\" lt 1"
	       //", %f*sin(t),%f*cos(t) title \"\" lt 2"
	       //", %f*sin(t),%f*cos(t) title \"\" lt 3\n"
#endif
	       ,
	       argv[2], argv[2], argv[2],

	       sqrt(ev1Cam),evAngleCam,sqrt(ev2Cam),evAngleCam, sqrt(ev1Cam),evAngleCam,sqrt(ev2Cam),evAngleCam, ev1Cam,ev2Cam,180.0*evAngleCam/M_PI,

	       sqrt(ev1Vis),evAngleVis,sqrt(ev2Vis),evAngleVis, sqrt(ev1Vis),evAngleVis,sqrt(ev2Vis),evAngleVis, ev1Vis,ev2Vis,180.0*evAngleVis/M_PI,

	       sqrt(ev1Dif),evAngleDif,sqrt(ev2Dif),evAngleDif, sqrt(ev1Dif),evAngleDif,sqrt(ev2Dif),evAngleCam, ev1Dif,ev2Dif,180.0*evAngleDif/M_PI);

      // localization/vision correlation plot

      fprintf (fp, "set noparametric\n");
      fprintf (fp, "set xtics 5\n");
      fprintf (fp, "set ytics 5\n");
      fprintf (fp, "set title \"translational displacement correlation plot (%i extreme outlier%s removed)\"\n", scatterDropTrans, scatterDropTrans != 1 ? "s" : "");
      fprintf (fp, "set xlabel \"VIS [mm]\"\n");
      fprintf (fp, "set ylabel \"LOC [mm]\"\n");
      fprintf (fp, "set key bottom right\n");
      fprintf (fp, "set output \"%s-corr-trans.eps\"\n", argv[2]);
      fprintf (fp, "plot [-%f:%f] [-%f:%f] \"%s.gp_data\" using 4:1 title \" x-axis (corr=%f)\", x title \"\", \"%s.gp_data\" using 5:2 title \" y-axis (corr=%f)\"\n"
	       , scatterLimitTrans, scatterLimitTrans, scatterLimitTrans, scatterLimitTrans, argv[2], xCorr, argv[2], yCorr);

      // localization/vision/difference rotation plot

      fprintf (fp, "set xtics 0.2\n");
      fprintf (fp, "set ytics 0.2\n");
      fprintf (fp, "set title \"rotational displacement scatter plot (%i extreme outlier%s removed)\"\n", scatterDropRot, scatterDropRot != 1 ? "s" : "");
      fprintf (fp, "set xlabel \"VIS [deg]\"\n");
      fprintf (fp, "set ylabel \"LOC/DIFF displacement [deg]\"\n");
      fprintf (fp, "set key bottom right\n");
      fprintf (fp, "set output \"%s-scatter-rot.eps\"\n", argv[2]);
      fprintf (fp, "plot [-%f:%f] [-%f:%f] \"%s.gp_data\" using 6:3 title \"LOC (corr=%f)\", x title \"\", \"%s.gp_data\" using 6:9 title \"DIFF (corr=%f)\"\n"
	       , scatterLimitRot, scatterLimitRot, scatterLimitRot, scatterLimitRot, argv[2], aCorr, argv[2], aCorr2);

      fclose (fp);
    }

  //
  //
  //

  printf ("# executing gnuplot commands\n");

  char buf[128];
  snprintf (buf, 128, "gnuplot %s.gp_cmd", argv[2]);
  system (buf);

  snprintf (buf, 128, "eps2square %s-scatter-trans.eps", argv[2]);
  system (buf);
  snprintf (buf, 128, "eps2square %s-corr-trans.eps", argv[2]);
  system (buf);
  snprintf (buf, 128, "eps2square %s-scatter-rot.eps", argv[2]);
  system (buf);

  printf ("# converting eps to pdf\n");

  snprintf (buf, 128, "epstopdf %s-scatter-trans.eps", argv[2]);
  system (buf);
  snprintf (buf, 128, "epstopdf %s-corr-trans.eps", argv[2]);
  system (buf);
  snprintf (buf, 128, "epstopdf %s-scatter-rot.eps", argv[2]);
  system (buf);

  //
  //
  //

  printf ("\n\n");

  return 0;
}
