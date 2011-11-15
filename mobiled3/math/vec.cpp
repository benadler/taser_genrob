//
//
//

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "math/vec.h"
#include "math/mat.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC::CVEC (const CVEC &right)
{
  // whatever we copy from, we assume that we create a normal vector

  if (right._flags & STEALME)   // CTMPVEC
    {
      _flags = FREEME;
      _size = right._size;
      _data = right._data;

      right._flags = 0;
      right._size = -1;
      right._data = 0x0;
    }
  else
    {
      // CVEC, CMATVEC

      if ((_size = right._size) <= MAXLOCALDATA)
	{
	  _flags = 0;
	  _data = _localData;
	}
      else
	{
	  _flags = FREEME;

	  if (!(_data = (datatype_t *)malloc (_size * sizeof (datatype_t))))
	    {
	      fprintf (stderr, "CVEC::CVEC(CVEC&) -- out of memory\n");
	      abort ();
	    }
	}

      memcpy (_data, right._data, _size * sizeof (datatype_t));
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC::CVEC (const int size)
{
  if ((_size = size) <= MAXLOCALDATA)
    {
      _flags = 0;
      _data = _localData;
    }
  else
    {
      _flags = FREEME;

      if (!(_data = (datatype_t *)malloc (_size * sizeof (datatype_t))))
	{
	  fprintf (stderr, "CVEC::CVEC(int) -- out of memory\n");
	  abort ();
	}
    }

  memset (_data, 0, _size * sizeof (datatype_t));
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUC__
#warning "doesn't obey MAXLOCALDATA"
#endif

CVEC::CVEC (const int size, const datatype_t *data)
{
  _flags = FREEME;
  _size = size;

  if (!(_data = (datatype_t *)malloc (_size * sizeof (datatype_t))))
    {
      fprintf (stderr, "CVEC::CVEC(int,datatype_t*) -- out of memory\n");
      abort ();
    }

  if (data)
    {
      memcpy (_data, data, _size * sizeof (datatype_t));
    }
  else
    {
      memset (_data, 0, _size * sizeof (datatype_t));
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC::CVEC (const double arg1)
{
  _flags = 0;
  _size = 1;
  _data = _localData;

  _data[0] = arg1;
}


CVEC::CVEC (const double arg1, const double arg2)
{
  _flags = 0;
  _size = 2;
  _data = _localData;

  _data[0] = arg1;
  _data[1] = arg2;
}


CVEC::CVEC (const double arg1, const double arg2, const double arg3)
{
  _flags = 0;
  _size = 3;
  _data = _localData;

  _data[0] = arg1;
  _data[1] = arg2;
  _data[2] = arg3;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CVEC::Create (const int size)
{
  static const char * const fxnName = "CVEC::Create(int) -- ";

  //

  if (_flags & STEALME)
    {
      fprintf (stderr, "%screation of temporary vector\n", fxnName);
      abort ();
    }

  //

  if (_flags & KEEPME)
    {
      fprintf (stderr, "%screation of matrix row vector\n", fxnName);
      abort ();
    }

  //

  if (_flags & FREEME)
    {
      free (_data);
    }

  if ((_size = size) <= MAXLOCALDATA)
    {
      _flags = 0;
      _data = _localData;
    }
  else
    {
      _flags = FREEME;

      if (!(_data = (datatype_t *)malloc (_size * sizeof (datatype_t))))
	{
	  fprintf (stderr, "CVEC::CVEC(int) -- out of memory\n");
	  abort ();
	}
    }

  memset (_data, 0, _size * sizeof (datatype_t));
}


#if 0

void CVEC::Create (const int size, const datatype_t *data)
{
  if (_flags & FREEME)
    {
      free (_data);
    }

  if ((_size = size) <= MAXLOCALDATA)
    {
      _flags = 0;

      _data = _localData;
    }
  else
    {
      _flags = FREEME;

      if (!(_data = (datatype_t *)malloc (_size * sizeof (datatype_t))))
	{
	  fprintf (stderr, "CVEC::CVEC(int,datatype_t*) -- out of memory\n");
	  abort ();
	}
    }

  if (data)
    {
      memcpy (_data, data, _size * sizeof (datatype_t));
    }
  else
    {
      memset (_data, 0, _size * sizeof (datatype_t));
    }
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CVEC::Create (const double arg1)
{
  if (_flags & FREEME)
    {
      free (_data);
    }

  _flags = 0;
  _size = 1;
  _data = _localData;

  _data[0] = arg1;
}


void CVEC::Create (const double arg1, const double arg2)
{
  if (_flags & FREEME)
    {
      free (_data);
    }

  _flags = 0;
  _size = 2;
  _data = _localData;

  _data[0] = arg1;
  _data[1] = arg2;
}


void CVEC::Create (const double arg1, const double arg2, const double arg3)
{
  if (_flags & FREEME)
    {
      free (_data);
    }

  _flags = 0;
  _size = 3;
  _data = _localData;

  _data[0] = arg1;
  _data[1] = arg2;
  _data[2] = arg3;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC &CVEC::operator= (const CVEC &right)
{
  static const char * const fxnName = "CVEC::operator=() -- ";

  // CTMPVEC?

  if (_flags & STEALME)
    {
      fprintf (stderr, "%sassignment to temporary\n", fxnName);
      abort ();
    }

  // CMATVEC?

  if (_flags & KEEPME)
    {
      if (_size != right._size)
	{
	  fprintf (stderr, "%ssizes don't match\n", fxnName);
	  abort ();
	}

      memcpy (_data, right._data, _size * sizeof (datatype_t));

      return *this;
    }

  // CVEC

  if (_flags & FREEME)
    {
      free (_data);
    }

  _size = right._size;

  if (right._flags & STEALME)
    {
      _flags = FREEME;
      _data = right._data;

      right._flags = 0;
      right._size = -1;
      right._data = 0x0;
    }
  else
    {
      if (_size <= MAXLOCALDATA)
	{
	  _flags = 0;
	  _data = _localData;
	}
      else
	{
	  _flags = FREEME;

	  if (!(_data = (datatype_t *)malloc (_size * sizeof (datatype_t))))
	    {
	      fprintf (stderr, "%sout of memory\n", fxnName);
	      abort ();
	    }
	}

      memcpy (_data, right._data, _size * sizeof (datatype_t));
    }

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC CVEC::operator+ (const CVEC &right) const
{
  static const char * const fxnName = "CVEC::operator+() -- ";

  if (_size != right._size)
    {
      fprintf (stderr, "%ssizes don't match\n", fxnName);
      abort ();
    }

  datatype_t *ptr = (datatype_t *)malloc (_size * sizeof (datatype_t));

  if (!ptr)
    {
      fprintf (stderr, "%sout of memory\n", fxnName);
      abort ();
    }

  int cnt = _size;
  datatype_t *src1 = _data, *src2 = right._data, *dst = ptr;

  while (--cnt >= 0)
    {
      *dst++ = *src1++ + *src2++;
    }

  return CVEC (ptr, _size, 1);   // temporary
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC CVEC::operator- (const CVEC &right) const
{
  static const char * const fxnName = "CVEC::operator-(CVEC&) -- ";

  if (_size != right._size)
    {
      fprintf (stderr, "%ssizes don't match\n", fxnName);
      abort ();
    }

  datatype_t *ptr = (datatype_t *)malloc (_size * sizeof (datatype_t));

  if (!ptr)
    {
      fprintf (stderr, "%sout of memory\n", fxnName);
      abort ();
    }

  int cnt = _size;
  datatype_t *src1 = _data, *src2 = right._data, *dst = ptr;

  while (--cnt >= 0)
    {
      *dst++ = *src1++ - *src2++;
    }

  return CVEC (ptr, _size, 1);   // temporary
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

double CVEC::operator* (const CVEC &right) const
{
  // static const char * const fxnName = "CVEC::operator*(CVEC&) -- ";

  if (_size != right._size)
    {
      ::fprintf (stderr, "CVEC::operator*(CVEC&) -- sizes don't match\n");
      ::abort ();
    }

  double ret = 0.0;

  int cnt = _size;
  datatype_t *src1 = _data, *src2 = right._data;

  while (--cnt >= 0)
    {
      ret += *src1++ * *src2++;
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC CVEC::operator* (const double arg) const
{
  // static const char * const fxnName = "CVEC::operator*(double) -- ";

  datatype_t *ptr = (datatype_t *)::malloc (_size * sizeof (datatype_t));

  if (!ptr)
    {
      ::fprintf (stderr, "CVEC::operator*(double) -- out of memory\n");
      ::abort ();
    }

  int cnt = _size;
  datatype_t *src = _data, *dst = ptr;

  while (--cnt >= 0)
    {
      *dst++ = *src++ * arg;
    }

  return CVEC (ptr, _size, 1);   // temporary
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC CVEC::operator/ (const double arg) const
{
  // static const char * const fxnName = "CVEC::operator/(double) -- ";

  datatype_t *ptr = (datatype_t *)::malloc (_size * sizeof (datatype_t));

  if (!ptr)
    {
      ::fprintf (stderr, "CVEC::operator/(double) -- out of memory\n");
      ::abort ();
    }

  int cnt = _size;
  datatype_t *src = _data, *dst = ptr;

  while (--cnt >= 0)
    {
      *dst++ = *src++ / arg;
    }

  return CVEC (ptr, _size, 1);   // temporary
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC &CVEC::operator+= (const CVEC &right)
{
  // static const char * const fxnName = "CVEC::operator+=(CVEC&) -- ";

  if (_size != right._size)
    {
      ::fprintf (stderr, "CVEC::operator+() -- sizes don't match\n");
      ::abort ();
    }

  int cnt = _size;
  datatype_t *src1 = _data, *src2 = right._data;

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

CVEC &CVEC::operator-= (const CVEC &right)
{
  // static const char * const fxnName = "CVEC::operator-=(CVEC&) -- ";

  if (_size != right._size)
    {
      ::fprintf (stderr, "CVEC::operator-() -- sizes don't match\n");
      ::abort ();
    }

  int cnt = _size;
  datatype_t *src1 = _data, *src2 = right._data;

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

CVEC &CVEC::operator*= (const double arg)
{
  // static const char * const fxnName = "CVEC::operator*=(double) -- ";

  int cnt = _size;
  datatype_t *src = _data;

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

CVEC &CVEC::operator/= (const double arg)
{
  // static const char * const fxnName = "CVEC::operator/=(double) -- ";

  int cnt = _size;
  datatype_t *src = _data;

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

static inline double sqr (const double arg)
{
  return arg * arg;
}


double CVEC::Norm (void) const
{
  double ret = 0.0;

  int cnt = _size;
  datatype_t *src = _data;

  while (--cnt >= 0)
    {
      ret += ::sqr (*src++);
    }

  return ::sqrt (ret);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC CVEC::Normalize (void) const
{
  datatype_t *ptr = (datatype_t *)::malloc (_size * sizeof (datatype_t));

  if (!ptr)
    {
      ::fprintf (stderr, "CVEC::Normalize() -- out of memory\n");
      ::abort ();
    }

  double norm = 0.0;

  int cnt = _size;
  datatype_t *src = _data;

  while (--cnt >= 0)
    {
      norm += ::sqr (*src++);
    }

  norm = ::sqrt (norm);

  cnt = _size;
  src = _data;
  datatype_t *dst = ptr;

  while (--cnt >= 0)
    {
      *dst++ = *src++ / norm;
    }

  return CVEC (ptr, _size, 1);   // temporary
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CVEC& CVEC::NormalizeSelf (void)
{
  double norm = 0.0;

  int cnt = _size;
  datatype_t *src = _data;

  while (--cnt >= 0)
    {
      norm += ::sqr (*src++);
    }

  norm = ::sqrt (norm);

  cnt = _size;
  src = _data;

  while (--cnt >= 0)
    {
      *src++ /= norm;
    }

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CVEC::Read (FILE *fp)
{
  static const char * const fxnName = "CVEC::Read(FILE*) -- ";

  if (_flags & STEALME)   // CTMPVEC
    {
      ::fprintf (stderr, "%sreading of temporary\n", fxnName);
      ::abort ();
    }

  int size, total = ::fscanf (fp, "# vector\n");
  total += ::fscanf (fp, "%i\n", &size);

  if (_flags & FREEME)   // CVEC
    {
      if (_data)
	{
	  ::free (_data);
	}

      if (!(_data = (datatype_t *)::malloc ((_size = size) * sizeof (datatype_t))))
	{
	  ::fprintf (stderr, "%sout of memory\n", fxnName);
	  ::abort ();
	}
    }
  else   // CMATVEC
    {
      if (size != _size)
	{
	  ::fprintf (stderr, "%ssizes don'ta match\n", fxnName);
	  ::abort ();
	}
    }

  datatype_t *ptr = _data;

  for (int i=0; i<_size; i++)
    {
      total += ::fscanf (fp, "%f\n", ptr++);
    }

  return total;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CVEC::Write (FILE *fp) const
{
  int total = ::fprintf (fp, "# vector\n");
  total += ::fprintf (fp, "%i\n", _size);

  datatype_t *ptr = _data;

  for (int i=0; i<_size; i++)
    {
      total += ::fprintf (fp, "%e\n", *ptr++);
    }

  ::fflush (fp);

  return total;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CVEC::Print (FILE *fp) const
{
  datatype_t *ptr = _data;

  int total = fprintf (fp, "CVEC::Print() -- %i\n", _size);

  for (int r=0; r<_size; r++)
    {
      total += fprintf (fp, "%+.4e ", *ptr++);
    }

  total += fprintf (fp, "\n\n");

  fflush (fp);

  return total;
}


///////////////////////////////////////////////////////////////////////////////
//
// three different kind of projections: on a line (1d), on a plane (2d) and in
// a space (3d). each of them use the DLS technique to both find the nearest
// point that actually is in the sub-space and yet be numerically stable in
// case of degenerate situations.
//
// if you only project points that you're sure are in the subspace spanned by
// the lines, then there's no problem. If by means of rounding errors you're
// not 100% sure, then there's also no problem: DLS will probably do what you
// meant anyway. Even the point is way out of that sub-space, taking the
// nearest one is resonable behaviour.
//
// degenerate situations include: 1) the length of an axis comes close to zero
// and 2) two axes become almost parallel. the caller must be aware that in
// these situations the result will not be what would be mathematically
// correct. that's due to rounding effects and can't be avoided. however, the
// results returned by the DLS are meaningful: If an axis in fact becomes a
// point, then a projection on that "axis" can only be exactly that point.
//
// in case it has not become clear so far: nobody said the lines must be
// orthogonal or something like that. you may use ANY lines that span up a
// sub-space...
//
///////////////////////////////////////////////////////////////////////////////

CVEC CVEC::ProjectOntoLine (const CVEC &l) const
{
  // Gleichungssystem mit 1 Unbekannten und N>1 Gleichungen -> LS
  // \vec{p} = k * \vec{l}

#if 0

  fprintf (stderr, "CVEC::ProjectOntoLine() untested!!!\n");
  abort ();

#endif

  // L * k = P
  // k = !L * P
  // k ~ !(LT L) * LT * P

  // this one needs no matrix computations

  datatype_t *f = (datatype_t *)malloc (sizeof (datatype_t));

  if (!f)
    {
      abort ();
    }

  *f = 1 / (l * l + 1e-30) * l * *this;

  return CVEC (1, f);
}


CVEC CVEC::ProjectIntoPlane (const CVEC &p1,
			     const CVEC &p2) const
{
  GTL ("CVEC::ProjectIntoPlane()");

  // Gleichungssystem mit 2 Unbekannten und N>2 Gleichungen
  // \vec{p} = k1 * \vec{p1} + k2 * \vec{p2}

  GTLFATAL (("UNTESTED\n"));

  if (_size < 2)
    {
      GTLFATAL (("size too small\n"));
      abort ();
    }

  if ((_size != p1._size) || (_size != p2._size))
    {
      GTLFATAL (("sizes don't match\n"));
      abort ();
    }

  CMAT m (_size, 2);
  m[0] = p1;
  m[1] = p2;

#if 1
  CMAT mTrans = m;
  m.TransSelf ();
#else
  CMAT LTrans = L.Trans ();
#endif

  CMAT mUnit (2, 2);
  mUnit.SetIdentity ();

  return ((mTrans * m + 1e-20 * mUnit).Inv () * mTrans) * *this;  // small pinv
}


CVEC CVEC::ProjectIntoSpace (const CVEC &s1,
			     const CVEC &s2,
			     const CVEC &s3) const
{
  // Gleichungssystem mit 3 Unbekannten und N>3 Gleichungen
  // \vec{p} = k1 * \vec{l1} + k2 * \vec{l2} + k3 * \vec{l3}

fprintf (stderr, "CVEC::ProjectIntoSpace() untested!!!\n");
abort ();

  CMAT L (s1.GetSize (), 3);
  L[0] = s1;
  L[1] = s2;
  L[2] = s3;

  CMAT LTrans (L.Trans());
  CMAT unit (3, 3);
  unit.SetIdentity ();

  return ((LTrans * L + 1e-30 * unit).Inv() * LTrans) * *this;
}
