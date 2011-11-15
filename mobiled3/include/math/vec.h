//
//
//


#ifndef _VEC_H_
#define _VEC_H_


//

#include <stdio.h>
#include <stdlib.h>

#ifdef __WATCOMC__
  #define mutable
#endif


//

class CVEC
{
  //

  friend class CMAT;

  //

public:

  typedef float datatype_t;

  //

private:

protected:

#if defined (__WATCOMC__) || defined (_WIN32)
  #define STEALME 0x01
  #define FREEME 0x02
  #define KEEPME 0x04
  #define MAXLOCALDATA 6
#else
  static const int STEALME = 0x01;   // CTMPVEC
  static const int FREEME = 0x02;   // CVEC(size>MAXLOCALSIZE), CTMPVEC
  static const int KEEPME = 0x04;   // CMATVEC
  static const int MAXLOCALDATA = 6;
#endif

  datatype_t _localData[MAXLOCALDATA];

  mutable int _flags;
  mutable int _size;
  mutable datatype_t *_data;

  CVEC (datatype_t *data, const int size)   // temporary CMAT (row) vecs
  {
    _flags = KEEPME;
    _size = size;
    _data = data;
  }

  CVEC (datatype_t *data, const int size, const int)   // other temporary vecs
  {
    _flags = STEALME | FREEME;
    _size = size;
    _data = data;
  }

public:

  CVEC (void)
  {
    _flags = 0;
    _size = 0;
    _data = 0x0;
  }

  CVEC (const CVEC &right);

  CVEC (const int size);
  CVEC (const int size, const datatype_t *data);

  CVEC (const double arg1);
  CVEC (const double arg1, const double arg2);
  CVEC (const double arg1, const double arg2, const double arg3);

  void Create (const int size);
#if 0
  void Create (const int size, const datatype_t *data);
#endif

  void Create (const double arg1);
  void Create (const double arg1, const double arg2);
  void Create (const double arg1, const double arg2, const double arg3);

  ~CVEC (void)
  {
    if (_flags & FREEME)
      {
	free (_data);
	_flags = 0;   // paranoia
	_data = 0x0;   // paranoia
      }
  }

  CVEC &operator= (const CVEC &right);

  int GetSize (void) const
  {
    return _size;
  }

  datatype_t *operator& (void)
  {
    return _data;
  }

  const datatype_t *operator& (void) const
  {
    return _data;
  }

  datatype_t &operator[] (const int idx)
  {
    if ((idx < 0) || (idx >= _size))
      {
	fprintf (stderr,
		 "CVEC::operator[]() -- index %i out of range 0..%i\n",
		 idx,
		 _size-1);
	abort ();
      }

    return _data[idx];
  }

  const datatype_t &operator[] (const int idx) const
  {
    if ((idx < 0) || (idx >= _size))
      {
	fprintf (stderr,
		 "CVEC::operator[]() -- index %i out of range 0..%i\n",
		 idx,
		 _size-1);
	abort ();
      }

    return _data[idx];
  }

  //

  CVEC operator+ (const CVEC &right) const;
  CVEC operator- (const CVEC &right) const;
  double operator* (const CVEC &right) const;
  CVEC operator* (const double arg) const;
  friend CVEC operator* (const double arg, const CVEC &vec)
  {
    return vec * arg;
  }
  CVEC operator/ (const double arg) const;

  CVEC &operator+= (const CVEC &right);
  CVEC &operator-= (const CVEC &right);
  CVEC &operator*= (const double arg);
  CVEC &operator/= (const double arg);

  double Norm (void) const;
  CVEC Normalize (void) const;
  CVEC& NormalizeSelf (void);

  int Read (FILE *fp = stdin);
  int Write (FILE *fp = stdout) const;
  int Print (FILE *fp = stdout) const;   // pretty printed for screen display

  // ++itschere20020225: ported from old diploma code for CFILT

  CVEC ProjectOntoLine (const CVEC &l) const;
  CVEC ProjectIntoPlane (const CVEC &p1, const CVEC &p2) const;
  CVEC ProjectIntoSpace (const CVEC &s1, const CVEC &s2, const CVEC &s3) const;
};


#endif
