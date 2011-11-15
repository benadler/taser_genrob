//
//
//

#ifndef _MAT_H_
#define _MAT_H_


//

#include "vec.h"


//

class CMAT
{
private:

protected:

#ifndef _WIN32
  static const int STEALME = 0x01;   // CTMPVEC
#else
  // _WIN32 has this already as global define
#endif

  int _flags;
  mutable int _rows, _cols;
  mutable int _size;
  mutable float *_data;

  CMAT (float *data, const int rows, const int cols)
  {
    _flags = STEALME;
    _size = (_rows = rows) * (_cols = cols);
    _data = data;
  }

public:

  CMAT (void)
  {
    //static const char * const fxnName = "CMAT::CMAT() -- ";

    _flags = 0;
    _size = _rows = _cols = 0;
    _data = 0x0;
  }

  CMAT (const CMAT &right);

  CMAT (const int rows, const int cols, const float *data = 0x0);

  void Create (const int rows, const int cols);
  void CreateCopy (const int rows, const int cols, float *data);
  void CreateSteal (const int rows, const int cols, float *data);

  ~CMAT (void)
  {
    if (_data)
      {
	free (_data);
	_data = 0x0;
      }
  }

  CMAT &operator= (const CMAT &right);

  //

  int GetRows (void) const
  {
    return _rows;
  }

  int GetCols (void) const
  {
    return _cols;
  }

  int GetSize (void) const
  {
    return _size;
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  // matSize.cpp
  //
  /////////////////////////////////////////////////////////////////////////////

  CMAT AddCols (const int num) const;
  CMAT AddRows (const int num) const;
  CMAT AddColsRows (const int num) const;

  CMAT &AddColsSelf (const int num);
  CMAT &AddRowsSelf (const int num);
  CMAT &AddColsRowsSelf (const int num);

  /////////////////////////////////////////////////////////////////////////////
  //
  // matManip.cpp
  //
  /////////////////////////////////////////////////////////////////////////////

  CMAT Clear (void) const
  {
    return CMAT (_rows, _cols);
  }
  CMAT SetDiag (const double arg) const;
  CMAT ClearDiag (void) const
  {
    return SetDiag (0.0);
  }
  CMAT UnitDiag (void) const
  {
    return SetDiag (1.0);
  }

  //

  CMAT &ClearSelf (void);
  CMAT &SetDiagSelf (const double arg);
  CMAT &ClearDiagSelf (void)
  {
    return SetDiagSelf (0.0);
  }
  CMAT &UnitDiagSelf (void)
  {
    return SetDiagSelf (1.0);
  }
  CMAT SetIdentitySelf (void)
  {
    return ClearSelf ().UnitDiagSelf ();
  }

  //

  CMAT SetIdentity (void) const
  {
    return Clear ().UnitDiagSelf ();
  }

  //
  // operators
  //

  CVEC operator[] (const int idx)
  {
    if ((idx < 0) | (idx >= _rows))
      {
	fprintf (stderr,
		 "CTMPMAT::operator[]() -- index %i out of range 0..%i\n",
		 idx, _rows-1);
	abort ();
      }

    return CVEC (_data + idx * _cols, _cols);   // needs no copy constructor
  }

  const CVEC operator[] (const int idx) const
  {
    if ((idx < 0) | (idx >= _rows))
      {
	fprintf (stderr,
		 "CTMPMAT::operator[]() -- index %i out of range 0..%i\n",
		 idx, _rows-1);
	abort ();
      }

    return CVEC (_data + idx * _cols, _cols);   // needs no copy constructor
  }

  //

  CMAT operator+ (const CMAT &right) const;
  CMAT operator- (const CMAT &right) const;
  CMAT operator* (const CMAT &right) const;
  CVEC operator* (const CVEC &right) const;
  CMAT operator* (const double arg) const;
  friend CMAT operator* (const double arg, const CMAT mat)
  {
    return mat * arg;
  }
  CMAT operator/ (const double arg) const;

  CMAT &operator+= (const CMAT &right);
  CMAT &operator-= (const CMAT &right);
  CMAT &operator*= (const CMAT &right);
  CMAT &operator*= (const double arg);
  CMAT &operator/= (const double arg);

  int Read (FILE *fp = stdin);
  int Write (FILE *fp = stdout) const;
  int Print (FILE *fp = stdout) const;   // "pretty" printed for screen display

  CMAT Trans (void) const;
  CMAT &TransSelf (void);

  //

  CVEC Solve (const CVEC &v) const;   // solves M*x=v

  CMAT Inv (void) const;

  CMAT PinvBig (void) const
    {
      CMAT t = Trans ();
      return t * (*this * t).Inv ();
    }

  CMAT PinvSmall (void) const
    {
      CMAT t = Trans ();
      return (t * *this).Inv () * t;
    }

  /////////////////////////////////////////////////////////////////////////////
  //
  // LU decomposition and associates
  //
  /////////////////////////////////////////////////////////////////////////////

  // DecompLU() - decomposes a matrix into a lower and an upper triangle matrix
  // and returns the determinant.

  void DecompLU (CMAT &l, CMAT &u, double *det = 0x0) const;
  CMAT DecompLU (double *det = 0x0) const;
  CMAT &DecompLUSelf (double *det = 0x0);

  CMAT &CombineLUSelf (const CMAT &l, const CMAT &u);

  CVEC SolveLU (const CVEC &b) const;

  CMAT InvLU (void) const;
  CMAT &InvLUSelf (void);
};


#endif
