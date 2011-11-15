//
//
//

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "math/mat.h"

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
// DecompLU()
//
// decomposes a matrix into a lower and an upper triangle matrix and returns
// the determinant.
//
///////////////////////////////////////////////////////////////////////////////

void CMAT::DecompLU (CMAT &l, CMAT &u, double *det) const
{
  GTL ("CMAT::DecompLU(CMAT&,CMAT&,double*)");

#if 1

  static bool first = true;

  if (first)
    {
      first = false;
      fprintf (stderr,
	       "# CMAT::DecompLU() does neither do pivotizing nor compute determinant (yet)\n");
    }

#endif

  const int size = _cols;   // abbreviation

  if (size != _rows)
    {
      GTLFATAL (("matrix not square\n"));
      abort ();
    }

  //

  try
    {
      // create target matrices and initialize to zero

      l.Create (size, size);
      u.Create (size, size);
    }

  catch (...)
    {
      GTLFATAL (("failed to create target matrices\n"));
      throw;
    }

  float *lData = l._data;
  float *uData = u._data;

  //
  // choose l[i][i] = 1
  //

  {
    float *lPtr = lData;

    for (int i=0; i<size; i++)
      {
	*lPtr++ = 1.0;
	lPtr += size;
      }
  }

  //
  // crout's algorithm (w/o pivoting)
  //
  // the code in the next paragraph is the principal algorithm, shown just for
  // a better understanding.
  //

#if 0

  for (int j=0; j<size; j++)
    {
      // step 1

      u[0][j] = m[0][j];

      // step 2

      for (int i=1; i<=j; i++)
	{
	  double sum = 0.0;

	  for (int k=0; k<i; k++)
	    {
	      sum += l[i][k] * u[k][j];
	    }

	  u[i][j] = m[i][j] - sum;
	}

      // step 3

      for (int i=j+1; i<size; i++)
	{
	  double sum = 0.0;

	  for (int k=0; k<j; k++)
	    {
	      sum += l[i][k] * u[k][j];
	    }

	  l[i][j] = (m[i][j] - sum) / u[j][j];
	}
    }

#endif

  //
  // and now the real code
  //


  for (int j=0; j<size; j++)
    {
      // step 1

      uData[j] = _data[j];

      // step 2

      for (int i=1; i<=j; i++)
	{
	  double sum = 0.0;

	  float *lPtr = lData + i * size;
	  float *uPtr = uData + j;

	  int k = i;

	  while (--k >= 0)
	    {
	      sum += *lPtr++ * *uPtr;
	      uPtr += size;
	    }

	  uData[i*size+j] = _data[i*size+j] - sum;
	}

      // step 3

      for (int i=j+1; i<size; i++)
	{
	  double sum = 0.0;

	  float *lPtr = lData + i * size;
	  float *uPtr = uData + j;

	  int k = j;

	  while (--k >= 0)
	    {
	      sum += *lPtr++ * *uPtr;
	      uPtr += size;
	    }

	  lData[i*size+j] = (_data[i*size+j] - sum) / uData[j*size+j];
	}
    }

  //

  if (det)
    {
      *det = 0.0;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// DecompLU()
//
// a more optimized (in that it returns a combined LU matrix) of the above.
//
///////////////////////////////////////////////////////////////////////////////

CMAT CMAT::DecompLU (double *det) const
{
  GTL ("CMAT::DecompLU(double*)");

#if 1

  static bool first = true;

  if (first)
    {
      first = false;
      fprintf (stderr,
	       "# CMAT::DecompLU() does neither do pivotizing nor compute determinant (yet)\n");
    }

#endif

  //

  const int size = _cols;   // abbreviation

  if (size != _rows)
    {
      GTLFATAL (("matrix not square\n"));
      abort ();
    }

  //

  float *lu = (float *)malloc (size * size * sizeof (float));

  if (!lu)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //
  // crout's algorithm (w/o pivoting)
  //

  for (int j=0; j<size; j++)
    {
      // step 1

      lu[j] = _data[j];

      // step 2

      for (int i=1; i<=j; i++)
	{
	  double sum = 0.0;

	  float *lPtr = lu + i * size;
	  float *uPtr = lu + j;

	  int k = i;

	  while (--k >= 0)
	    {
	      sum += *lPtr++ * *uPtr;
	      uPtr += size;
	    }

	  lu[i*size+j] = _data[i*size+j] - sum;
	}

      // step 3

      for (int i=j+1; i<size; i++)
	{
	  double sum = 0.0;

	  float *lPtr = lu + i * size;
	  float *uPtr = lu + j;

	  int k = j;

	  while (--k >= 0)
	    {
	      sum += *lPtr++ * *uPtr;
	      uPtr += size;
	    }

	  lu[i*size+j] = (_data[i*size+j] - sum) / lu[j*size+j];
	}
    }

  //

  if (det)
    {
      *det = 0.0;
    }

  return CMAT (lu, size, size);
}


///////////////////////////////////////////////////////////////////////////////
//
// DecompLU()
//
// same as above, except that the original matrix is filled with the result.
//
///////////////////////////////////////////////////////////////////////////////

CMAT &CMAT::DecompLUSelf (double *det)
{
  GTL ("CMAT::DecompLUSelf(double*)");

#if 1

  static bool first = true;

  if (first)
    {
      first = false;
      fprintf (stderr,
	       "# CMAT::DecompLUSelf() does neither do pivotizing nor compute determinant (yet)\n");
    }

#endif

  //

  const int size = _cols;   // abbreviation

  if (size != _rows)
    {
      GTLFATAL (("matrix not square\n"));
      abort ();
    }

  //

  float *lu = (float *)malloc (size * size * sizeof (float));

  if (!lu)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //
  // crout's algorithm (w/o pivoting)
  //

  for (int j=0; j<size; j++)
    {
      // step 1

      lu[j] = _data[j];

      // step 2

      for (int i=1; i<=j; i++)
	{
	  double sum = 0.0;

	  float *lPtr = lu + i * size;
	  float *uPtr = lu + j;

	  int k = i;

	  while (--k >= 0)
	    {
	      sum += *lPtr++ * *uPtr;
	      uPtr += size;
	    }

	  lu[i*size+j] = _data[i*size+j] - sum;
	}

      // step 3

      for (int i=j+1; i<size; i++)
	{
	  double sum = 0.0;

	  float *lPtr = lu + i * size;
	  float *uPtr = lu + j;

	  int k = j;

	  while (--k >= 0)
	    {
	      sum += *lPtr++ * *uPtr;
	      uPtr += size;
	    }

	  lu[i*size+j] = (_data[i*size+j] - sum) / lu[j*size+j];
	}
    }

  //

  if (det)
    {
      *det = 0.0;
    }

  free (_data);

  _data = lu;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT &CMAT::CombineLUSelf (const CMAT &l, const CMAT &u)
{
  GTL ("CMAT::CombineLUSelf()");

  const int size = l._cols;

  if ((size != l._rows) ||
      (size != u._cols) ||
      (size != u._rows))
    {
      GTLFATAL (("sizes don't match\n"));
      abort ();
    }

  //

  try
    {
      Create (size, size);
    }

  catch (...)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //

  int offset = 0;
  float *dst = _data;

  for (int y=0; y<size; y++)
    {

      for (int x=0; x<size; x++)
	{
	  if (y > x)
	    {
	      *dst++ = l._data[offset];
	    }
	  else
	    {
	      *dst++ = u._data[offset];
	    }

	  offset++;
	}
    }

  //

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
// SolveLU()
//
// takes a combined LU matrix and solves (LU)*x=b in the steps L*y=b and
// U*x=y. returns x.
//
///////////////////////////////////////////////////////////////////////////////

CVEC CMAT::SolveLU (const CVEC &b) const
{
  GTL ("CMAT::SolveLU()");

  const int size = _cols;

  if ((size != _rows) ||
      (size != b._size))
    {
      GTLFATAL (("sizes don't match\n"));
      abort ();
    }

  //

  CVEC y = b;

  const float *lData = _data;
  const float *uData = _data;

  //
  // the code in the next paragraph is the principal algorithm, shown just for
  // a better understanding.
  //

#if 0

  // first solve L*y=b

  for (int i=0; i<size; i++)
    {
      y[i] = tmp[i] / l[i][i];

      for (int j=i+1; j<size; j++)
	{
	  tmp[j] -= y[i] * l[j][i];
	}
    }

  // second solve U*x=y

  x.Create (size);

  for (int i=size-1; i>=0; i--)
    {
      x[i] = y[i] / u[i][i];

      for (int j=i-1; j>=0; j--)
	{
	  y[j] -= u[j][i] * x[i];
	}
    }

#endif

  //
  // the real code
  //

  // first solve L*y=b (but caution: 'y' and 'b' share the same data fields!)

  float *yPtr = y._data;

  for (int i=0; i<size; i++)
    {
      double r = *yPtr++;   // l[i][i] == 1 by definition

      float *yPtrInner = yPtr;
      const float *lPtr = lData + (i+1) * size + i;

      for (int j=i+1; j<size; j++)
	{
	  *yPtrInner++ -= r * *lPtr;
	  lPtr += size;
	}
    }

  // second solve U*x=y

  float *x = (float *)malloc (size * sizeof (float));

  if (!x)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  float *xPtr = x + (size-1);
  yPtr = y._data + (size-1);

  for (int i=size-1; i>=0; i--)
    {
      double r = *xPtr-- = *yPtr-- / uData[i*size+i];

      float *yPtrInner = yPtr;
      const float *uPtr = uData + (i-1) * size + i;

      for (int j=i-1; j>=0; j--)
	{
	  *yPtrInner-- -= *uPtr * r;
	  uPtr -= size;
	}
    }

  //

  return CVEC (x, size, 42);   // needs no copy constructor
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT CMAT::InvLU (void) const
{
  GTL ("CMAT::InvLU()");

  const int size = _cols;

  if (size != _rows)
    {
      GTLFATAL (("matrix not square\n"));
      abort ();
    }

  //

  CMAT lu = DecompLU ();

  //

  float *mi = (float *)malloc (size * size * sizeof (float));

  if (!mi)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  for (int i=0; i<size; i++)
    {
      CVEC v (size);

      v[i] = 1.0;

      CVEC r = lu.SolveLU (v);

      float *miPtr = mi + i;

      for (int j=0; j<size; j++)
	{
	  *miPtr = r[j];
	  miPtr += size;
	}
    }

  //

  return CMAT (mi, size, size);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAT &CMAT::InvLUSelf (void)
{
  GTL ("CMAT::InvLUSelf()");

  const int size = _cols;

  if (size != _rows)
    {
      GTLFATAL (("matrix not square\n"));
      abort ();
    }

  //

  DecompLUSelf ();

  //

  float *mi = (float *)malloc (size * size * sizeof (float));

  if (!mi)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  for (int i=0; i<size; i++)
    {
      CVEC v (size);

      v[i] = 1.0;

      CVEC r = SolveLU (v);

      float *miPtr = mi + i;

      for (int j=0; j<size; j++)
	{
	  *miPtr = r[j];
	  miPtr += size;
	}
    }

  //

  CreateSteal (size, size, mi);

  return *this;
}
