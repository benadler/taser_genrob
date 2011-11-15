//
//
//

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "math/mat.h"
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT::CMAT (const CMAT &right)
{
  // static const char * const fxnName = "CMAT::CMAT(CMAT&) -- ";

  // whatever we copy from, we assume that we're creating a normal matrix

  _flags = 0;

  if (right._flags & STEALME)
    {
      _rows = right._rows;
      _cols = right._cols;
      _size = right._size;
      _data = right._data;

      right._rows = 0;
      right._cols = 0;
      right._size = 0;
      right._data = 0x0;
    }
  else
    {
      _size = (_rows = right._rows) * (_cols = right._cols);

      if (!(_data = (float *)::malloc (_size * sizeof (float))))
	{
	  ::fprintf (stderr, "CMAT::CMAT(CMAT&) -- out of memory\n");
	  ::abort ();
	}

      ::memcpy (_data, right._data, _size * sizeof (float));
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT::CMAT (const int rows, const int cols, const float *data)
{
  static const char * const fxnName = "CMAT::CMAT(int,int,float*) -- ";

  _flags = 0;
  _size = (_rows = rows) * (_cols = cols);

  if (!(_data = (float *)::malloc (_size * sizeof (float))))
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  if (data)
    {
      ::memcpy (_data, data, _size * sizeof (float));
    }
  else
    {
      ::memset (_data, 0, _size * sizeof (float));
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CMAT::Create (const int rows, const int cols)
{
  static const char * const fxnName = "CMAT::Create(int,int) -- ";

  if (_flags & STEALME)
    {
      fprintf (stderr, "%screating a temporary\n", fxnName);
      abort ();
    }

  if (_data)
    {
      free (_data);
    }

  _flags = 0;
  _size = (_rows = rows) * (_cols = cols);

  if (!(_data = (float *)malloc (_size * sizeof (float))))
    {
      fprintf (stderr, "%sout of memory\n", fxnName);
      abort ();
    }

  memset (_data, 0, _size * sizeof (float));
}


void CMAT::CreateCopy (const int rows, const int cols, float *data)
{
  static const char * const fxnName = "CMAT::CreateCopy(int,int,float*) -- ";

  if (_flags & STEALME)
    {
      fprintf (stderr, "%screating a temporary\n", fxnName);
      abort ();
    }

  if (_data)
    {
      free (_data);
    }

  _flags = 0;
  _size = (_rows = rows) * (_cols = cols);

  if (!(_data = (float *)::malloc (_size * sizeof (float))))
    {
      fprintf (stderr, "%sout of memory\n", fxnName);
      abort ();
    }

  memcpy (_data, data, _size * sizeof (float));
}


void CMAT::CreateSteal (const int rows, const int cols, float *data)
{
  static const char * const fxnName = "CMAT::CreateSteal(int,int,float*) -- ";

  if (_flags & STEALME)
    {
      fprintf (stderr, "%screating a temporary\n", fxnName);
      abort ();
    }

  if (_data)
    {
      free (_data);
    }

  _flags = 0;
  _size = (_rows = rows) * (_cols = cols);
  _data = data;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT &CMAT::operator= (const CMAT &right)
{
  static const char * const fxnName = "CMAT::operator=() -- ";

  if (_flags & STEALME)
    {
      ::fprintf (stderr, "%sassigning to temporary\n", fxnName);
      ::abort ();
    }

  //#warning "minor optimizations possible here"

  if (_data)
    {
      ::free (_data);
    }

  _flags = 0;

  _rows = right._rows;
  _cols = right._cols;
  _size = right._size;

  if (right._flags & STEALME)
    {
      _data = right._data;

      right._rows = 0;
      right._cols = 0;
      right._size = 0;
      right._data = 0x0;
    }
  else
    {
      if (!(_data = (float *)::malloc (_size * sizeof (float))))
	{
	  ::fprintf (stderr, "%sout of memory\n", fxnName);
	  ::abort ();
	}

      ::memcpy (_data, right._data, _size * sizeof (float));
    }

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT CMAT::operator+ (const CMAT &right) const
{
  static const char * const fxnName = "CMAT::operator+() -- ";

  if ((_rows != right._rows) || (_cols != right._cols))
    {
      ::fprintf (stderr, "%ssizes don't match\n", fxnName);
      ::abort ();
    }

  float *ptr = (float *)::malloc (_size * sizeof (float));

  if (!ptr)
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  int cnt = _size;
  float *src1 = _data, *src2 = right._data, *dst = ptr;

  while (--cnt >= 0)
    {
      *dst++ = *src1++ + *src2++;
    }

  return CMAT (ptr, _rows, _cols);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT CMAT::operator- (const CMAT &right) const
{
  static const char * const fxnName = "CMAT::operator-() -- ";

  if ((_rows != right._rows) || (_cols != right._cols))
    {
      ::fprintf (stderr, "%ssizes don't match\n", fxnName);
      ::abort ();
    }

  float *ptr = (float *)::malloc (_size * sizeof (float));

  if (!ptr)
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  int cnt = _size;
  float *src1 = _data, *src2 = right._data, *dst = ptr;

  while (--cnt >= 0)
    {
      *dst++ = *src1++ - *src2++;
    }

  return CMAT (ptr, _rows, _cols);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT CMAT::operator* (const CMAT &right) const
{
  static const char * const fxnName = "CMAT::operator*(CMAT&) -- ";

  if (_cols != right._rows)
    {
      ::fprintf (stderr, "%ssizes don't match\n", fxnName);
      ::abort ();
    }

  float *ptr = (float *)::malloc (_rows * right._cols * sizeof (float));

  if (!ptr)
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  float *dst = ptr;

  for (int r=0; r<_rows; r++)
    {
      for (int c=0; c<right._cols; c++)
	{
	  // row(r) * col(c)

	  float *src1 = _data + r * _cols;
	  float *src2 = right._data + c;

	  int cnt = _cols;
	  double sum = 0.0;

	  while (--cnt >= 0)
	    {
	      sum += *src1++ * *src2;
	      src2 += right._cols;
	    }

	  *dst++ = sum;
	}
    }

  return CMAT (ptr, _rows, right._cols);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC CMAT::operator* (const CVEC &right) const
{
  static const char * const fxnName = "CMAT::operator*(CVEC&) -- ";

  if (_cols != right._size)
    {
      ::fprintf (stderr, "%ssizes don't match\n", fxnName);
      ::abort ();
    }

  float *ptr = (float *)::malloc (_rows * sizeof (float));

  if (!ptr)
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  float *dst = ptr;

  for (int r=0; r<_rows; r++)
    {
      // row(r) * col

      float *src1 = _data + r * _cols;
      float *src2 = right._data;

      int cnt = _cols;
      double sum = 0.0;

      while (--cnt >= 0)
	{
	  sum += *src1++ * *src2++;
	}

      *dst++ = sum;
    }

  return CVEC (ptr, _rows, 1);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT CMAT::operator* (const double arg) const
{
  static const char * const fxnName = "CMAT::operator*(double) -- ";

  float *ptr = (float *)::malloc (_size * sizeof (float));

  if (!ptr)
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  int cnt = _size;
  float *src = _data, *dst = ptr;

  while (--cnt >= 0)
    {
      *dst++ = *src++ * arg;
    }

  return CMAT (ptr, _rows, _cols);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT CMAT::operator/ (const double arg) const
{
  static const char * const fxnName = "CMAT::operator/() -- ";

  float *ptr = (float *)::malloc (_size * sizeof (float));

  if (!ptr)
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  int cnt = _size;
  float *src = _data, *dst = ptr;

  while (--cnt >= 0)
    {
      *dst++ = *src++ / arg;
    }

  return CMAT (ptr, _rows, _cols);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT &CMAT::operator+= (const CMAT &right)
{
  static const char * const fxnName = "CMAT::operator+=() -- ";

  if ((_rows != right._rows) || (_cols != right._cols))
    {
      ::fprintf (stderr, "%ssizes don't match\n", fxnName);
      ::abort ();
    }

  int cnt = _size;
  float *src1 = _data, *src2 = right._data;

  while (--cnt >= 0)
    {
      *src1++ += *src2++;
    }

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT &CMAT::operator-= (const CMAT &right)
{
  static const char * const fxnName = "CMAT::operator-=() -- ";

  if ((_rows != right._rows) || (_cols != right._cols))
    {
      ::fprintf (stderr, "%ssizes don't match\n", fxnName);
      ::abort ();
    }

  int cnt = _size;
  float *src1 = _data, *src2 = right._data;

  while (--cnt >= 0)
    {
      *src1++ -= *src2++;
    }

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT &CMAT::operator*= (const CMAT &right)
{
  static const char * const fxnName = "CMAT::operator*=(CMAT&) -- ";

  if (_cols != right._rows)
    {
      ::fprintf (stderr, "%ssizes don't match\n", fxnName);
      ::abort ();
    }

  float *ptr = (float *)::malloc (_rows * right._cols * sizeof (float));

  if (!ptr)
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  float *dst = ptr;

  for (int r=0; r<_rows; r++)
    {
      for (int c=0; c<right._cols; c++)
	{
	  // row(r) * col(c)

	  //#warning "minor optimizations possible here"

	  float *src1 = _data + r * _cols;
	  float *src2 = right._data + c;

	  int cnt = _cols;
	  double sum = 0.0;

	  while (--cnt >= 0)
	    {
	      sum += *src1++ * *src2;
	      src2 += right._cols;
	    }

	  *dst++ = sum;
	}
    }

  ::free (_data);

  _size = (_rows = _rows) * (_cols = right._cols);
  _data = ptr;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT &CMAT::operator*= (const double arg)
{
  // static const char * const fxnName = "CMAT::operator*=(double) -- ";

  int cnt = _size;
  float *src = _data;

  while (--cnt >= 0)
    {
      *src++ *= arg;
    }

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT &CMAT::operator/= (const double arg)
{
  // static const char * const fxnName = "CMAT::operator/=(double) -- ";

  int cnt = _size;
  float *src = _data;

  while (--cnt >= 0)
    {
      *src++ /= arg;
    }

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMAT::Read (FILE *fp)
{
  static const char * const fxnName = "CMAT::Read(FILE*) -- ";

  if (_flags & STEALME)   // CTMPMAT
    {
      ::fprintf (stderr, "%sreading of temporary\n", fxnName);
      ::abort ();
    }

  int total = ::fscanf (fp, "# matrix\n");
  total += ::fscanf (fp, "%i\n", &_rows);
  total += ::fscanf (fp, "%i\n", &_cols);

  //#warning "minor optimizations possible here"

  if (_data)
    {
      ::free (_data);
    }

  if (!(_data = (float *)::malloc ((_size = _rows * _cols) * sizeof (float))))
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  float *ptr = _data;

  for (int r=0; r<_rows; r++)
    {
      for (int c=0; c<_cols; c++)
	{
	  total += ::fscanf (fp, "%e\n", ptr++);
	}
    }

  return total;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMAT::Write (FILE *fp) const
{
  int total = ::fprintf (fp, "# matrix\n");
  total += ::fprintf (fp, "%i\n", _rows);
  total += ::fprintf (fp, "%i\n", _cols);

  float *ptr = _data;

  for (int r=0; r<_rows; r++)
    {
      for (int c=0; c<_cols; c++)
	{
	  total += ::fprintf (fp, "%e\n", *ptr++);
	}
    }

  ::fflush (fp);

  return total;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMAT::Print (FILE *fp) const
{
  float *ptr = _data;

  fprintf (fp, "CMAT::Print() -- %ix%i\n", _rows, _cols);

  for (int r=0; r<_rows; r++)
    {
      for (int c=0; c<_cols; c++)
	{
#if 0
	  fprintf (fp, "%+.3f ", *ptr++);
#else
	  fprintf (fp, "%+.4e ", *ptr++);
#endif
	}
      fprintf (fp, "\n");
    }

  fprintf (fp, "\n");
  fflush (fp);

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

CMAT CMAT::Trans (void) const
{
  static const char * const fxnName = "CMAT::Trans() -- ";

  float *data = (float *)::malloc (_size * sizeof (float));

  if (!data)
    {
      fprintf (stderr, "%sout of memory\n", fxnName);
      abort ();
    }

  float *from = _data;

  for (int r=0; r<_rows; r++)
    {
      float *to = data + r;
      int cnt = _cols;

      while (--cnt >= 0)
	{
	  *to = *from++;
	  to += _rows;
	}
    }

  return CMAT (data, _cols, _rows);
}


//
//
//

CMAT &CMAT::TransSelf (void)
{
  static const char * const fxnName = "CMAT::TransSelf() -- ";

  float *data = (float *)::malloc (_size * sizeof (float));

  if (!data)
    {
      fprintf (stderr, "%sout of memory\n", fxnName);
      abort ();
    }

  float *from = _data;

  for (int r=0; r<_rows; r++)
    {
      float *to = data + r;
      int cnt = _cols;

      while (--cnt >= 0)
	{
	  *to = *from++;
	  to += _rows;
	}
    }

  int tmp = _rows;
  _rows = _cols;
  _cols = tmp;

  free (_data);
  _data = data;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


//
//
//

CVEC CMAT::Solve (const CVEC &v) const
{
  static const char * const fxnName = "CMAT::Solve() -- ";

  // get some private copy to play on

  CMAT mCopy (_rows, _cols, _data);
  float *mData = &mCopy[0][0];

  CVEC vCopy (v._size, v._data);
  float *vData = &vCopy[0];

  //printf ("%s\n", fxnName);
  //mCopy.Print ();

  // no pivotizing

  {

  for (int r=0; r<_rows-1; r++)
    {
      double f = ::fabs (mData[r*_cols]);   // ++itschere20010813: ist das denn richtig???
      int fIdx = 0;

      for (int c=1; c<_cols; c++)
	{
	  double cmp = ::fabs (mData[r*_cols+c]);

	  if (cmp > f)
	    {
	      f = cmp;
	      fIdx = c;
	    }
	}

      //::printf ("*** %f @ %i\n", f, fIdx);

      // the largest element in row `r' is in column `fIdx'

      for (int r2=r+1; r2<_rows; r2++)
	{
	  double f1 = mData[r*_cols+fIdx];
	  double f2 = mData[r2*_cols+fIdx];

	  //::printf ("f1 = %f, f2 = %f\n", f1, f2);

#if 1
	  for (int c=0; c<_cols; c++)
	    {
	      // standard version

	      mData[r2*_cols+c] -= f2 * mData[r*_cols+c] / f1;
	    }

	  vData[r2] -= f2 * vData[r] /f1;
#else
	  for (int c=0; c<_cols; c++)
	    {
	      // this version avoids the division and therefore never generates
	      // fractional numbers on its own, but on the other hand it tends
	      // to yield unnecessary large numbers. when these numbers rise
	      // beyond MAX_FLOAT (~1e+37) you'll get problems.

	      mData[r2*_cols+c] = f2 * mData[r*_cols+c] - f1 * mData[r2*_cols+c];
	    }
	  
	  vData[r2] = f2 * vData[r] - f1 * vData[r2];
#endif
	}

      //mCopy.Print ();
    }

  }

  //::printf ("*** this is kinda upper triangle matrix...\n");

  //

  float *ret = (float *)::malloc (_rows * sizeof (float));

  if (!ret)
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  //

  for (int r2=_rows-1; r2>=0; r2--)
    {
      double f = ::fabs (mData[r2*_cols]);
      int fIdx = 0;

      for (int c=1; c<_cols; c++)
	{
	  double cmp = ::fabs (mData[r2*_cols+c]);

	  if (cmp > f)
	    {
	      f = cmp;
	      fIdx = c;
	    }
	}

      //::printf ("*** %f @ %i\n", f, fIdx);

      if (f < 1e-6)
	{
	  ::printf ("%swarning: matrix almost singular\n", fxnName);
	}

      // the largest element in row `r' is in column `fIdx'

      f = mData[r2*_cols+fIdx];

      f = ret[fIdx] = vData[r2] / f;

      for (int r3=r2-1; r3>=0; r3--)
	{
	  vData[r3] -= f * mData[r3*_cols+fIdx];
	  mData[r3*_cols+fIdx] = 0.0;
	}

      //mCopy.Print ();
    }

  //

  return CVEC (ret, _rows, 42);
}


//
//
//

CMAT CMAT::Inv (void) const
{
  static const char * const fxnName = "CMAT::Inv() -- ";

  if (_rows != _cols)
    {
      ::fprintf (stderr, "%smatrix not square\n", fxnName);
      ::abort ();
    }

  float *data = (float *)::malloc (_size * sizeof (float));

  if (!data)
    {
      ::fprintf (stderr, "%sout of memory\n", fxnName);
      ::abort ();
    }

  for (int c=0; c<_cols; c++)
    {
      CVEC out (_rows);   // initialized to zero
      out[c] = 1.0;

      CVEC v = this->Solve (out);

      for (int r=0; r<_rows; r++)
	{
	  data[r*_cols+c] = v[r];
	}
    }

  return CMAT (data, _rows, _cols);
}
