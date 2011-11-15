//
// arrays.cc
// (C) 11/2003,02/2004 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <stdlib.h>

#include "thread/tracelog.h"

#include "arrays.h"


///////////////////////////////////////////////////////////////////////////////
//
// CDYNARRAY
//
// a dynamically growing one-dimensional array. internally computes the average
// and standard deviation of its data.
//
///////////////////////////////////////////////////////////////////////////////

void CDYNARRAY::RemoveAverage (void)
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


void CDYNARRAY::RemoveAverage (double &avg)
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
      GTLFATAL (("index %i out of range 0..%i\n", idx, _used-1));
      abort ();
    }

  return _ptr[idx];
}


double &CDYNARRAY::operator[] (const int idx)
{
  GTL ("CDYNARRAY::operator[]()");

  if ((idx < 0) || (idx >= _used))
    {
      GTLFATAL (("index %i out of range 0..%i\n", idx, _used-1));
      abort ();
    }

  return _ptr[idx];
}


void CDYNARRAY::Reset (void)
{
  sum = sumq = 0.0;

  for (int i=0; i<_used; i++)
    {
      sum += _ptr[i];
      sumq += sqr (_ptr[i]);
    }
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

void CDYNDEGREEARRAY::Add (degree_t arg)
{
  const double angle = M_PI * arg / 180.0;

  _sin.Add (sin (angle));
  _cos.Add (cos (angle));
}


CDYNDEGREEARRAY::degree_t CDYNDEGREEARRAY::Dev (void) const
{
  // sqrt (E(X^2) - E(X)^2)

  double sum = 0.0, sumq = 0.0;
  double cnt = _sin.GetSize ();

  for (int i=0; i<_sin.GetSize(); i++)
    {
      const double angle = 180.0 * atan2 (_sin[i], _cos[i]) / M_PI;

      sum += angle;
      sumq += sqr (angle);
    }

  sum /= cnt;

  if (fabs (sum) > 1.0)
    {
      printf ("# CDYNDEGREEARRAY::Dev() -- assumes angles around zero, %lf %lf (==0?)\n", sumq, sum);
    }

  return sqrt (sumq / cnt - sqr (sum));
}


double CDYNDEGREEARRAY::Avg (void) const
{
  const double sAvg = _sin.Avg ();
  const double cAvg = _cos.Avg ();

  double avg = 180.0 * atan2 (sAvg, cAvg) / M_PI;

  //printf ("CDYNDEGREE::Avg() -- %f %f -> %f\n", sAvg, cAvg, avg);

  return avg;
}


void CDYNDEGREEARRAY::RemoveAverage (degree_t &avg)
{
  const double sAvg = _sin.Avg ();
  const double cAvg = _cos.Avg ();

  const double angle = atan2 (sAvg, cAvg);

  avg = 180.0 * angle / M_PI;

  //printf ("CDYNDEGREE::RemoveAverage() -- %f %f -> %f\n", sAvg, cAvg, avg);

  for (int i=0; i<_sin.GetSize(); i++)
    {
      const double corr = atan2 (_sin[i], _cos[i]) - angle;

      _sin[i] = sin (corr);
      _cos[i] = cos (corr);
    }

  _sin.Reset ();
  _cos.Reset ();
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


#if 0

///////////////////////////////////////////////////////////////////////////////
//
// CPOSARRAY
//
// a dynamic array of 2d-positions, say, two linear coordinates `x' and `y' and
// an angle `a'.
//
///////////////////////////////////////////////////////////////////////////////

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

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CPOSARRAY::FindMaximum (meter_t &dMax, degree_t &aMax)
{
  const int size = X.GetSize ();

  dMax = aMax = 0.0;

  for (int i=0; i<size; i++)
    {
      const double d = sqrt (sqr (X[i]) + sqr (Y[i]));

      if (d > dMax)
	{
	  dMax = d;
	}

      if (fabs (A[i]) > aMax)
	{
	  aMax = fabs (A[i]);
	}
    }
}
