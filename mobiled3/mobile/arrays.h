//
// arrays.h
// (C) 11/2003,02/2004 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#ifndef _ARRAYS_H_
#define _ARRAYS_H_

#include <math.h>
#include <stdlib.h>


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

class CARRAY
{
private:

protected:

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

  int _used, _size;
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
  
  void RemoveAverage (void);

  void RemoveAverage (double &avg);

  double Var (void) const   // varianz
  {
    const double avg = sum / cnt;
    const double ret = sumq / cnt - sqr (avg);

    return ret >= 0 ? ret : 0.0;
  }

  double Dev (void) const   // standardabweichung = sqrt (varianz)
  {
    const double avg = sum / cnt;
    const double ret = sumq / cnt - sqr (avg);

    return ret >= 0 ? sqrt (ret) : 0.0;
  }

  virtual const double operator[] (const int idx) const;
  virtual double &operator[] (const int idx);

  void Reset (void);
};


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
  degree_t Dev (void) const;

  void RemoveAverage (degree_t &avg);

  virtual const double operator[] (const int idx) const;
};


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

public:

  CDYNARRAY X, Y;
  CDYNDEGREEARRAY A;

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
    return X.GetSize ();
  }

  void Add (const meter_t x,
	    const meter_t y,
	    const degree_t a)
  {
    X.Add (x);
    Y.Add (y);
    A.Add (a);
  }

  void Avg (meter_t &x, meter_t &y, degree_t &a) const
  {
    x = X.Avg ();
    y = Y.Avg ();
    a = A.Avg ();
  }

  void Dev (meter_t &x, meter_t &y, degree_t &a) const
  {
    x = X.Dev ();
    y = Y.Dev ();
    a = A.Dev ();
  }

  void RemoveAverage (meter_t &x, meter_t &y, degree_t &a)
  {
    X.RemoveAverage (x);
    Y.RemoveAverage (y);
    A.RemoveAverage (a);
  }

  void FindMaximum (meter_t &dMax, degree_t &aMax);
};


#endif
