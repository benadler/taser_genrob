//
//
//


#ifndef _CCHUNK_H
#define _CCHUNK_H


class CCHUNK
{
private:

  // not even implemented

  CCHUNK (const CCHUNK &);
  CCHUNK &operator= (const CCHUNK &);

protected:

  int _chunkSize;
  int _size;
  int _written;
  int _read;
#if 1
  char *_data;
#else
  void *_data;
#endif

public:

  //

  CCHUNK (int chunkSize = 4096);
  CCHUNK (const void *data, const int size);

  virtual ~CCHUNK (void);

  //
  // what's really implemented
  //

  int GetSize (void) const {return _written;}
  void *GetData (void) const {return _data;}

  // implement the (probably) most used functions directly...

  void PackInt (const int i);
  int UnpackInt (void);

  // ...all other are mapped to these by inline functions

  void Pack8BitArray  (const void *ptr, int size);
  void Pack16BitArray (const void *ptr, int size);
  void Pack32BitArray (const void *ptr, int size);
  void Pack64BitArray (const void *ptr, int size);

  void Unpack8BitArray  (void *ptr, int size);
  void Unpack16BitArray (void *ptr, int size);
  void Unpack32BitArray (void *ptr, int size);
  void Unpack64BitArray (void *ptr, int size);

  //
  // (not very optimized) inline functions for single elements
  //

  void PackChar (const char c)
    {
      Pack8BitArray (&c, 1);
    }
  void PackUnsignedChar (const unsigned char c)
    {
      Pack8BitArray (&c, 1);
    }
  void PackShort (const short s)
    {
      Pack16BitArray (&s, 1);
    }
  void PackUnsignedShort (const unsigned short s)
    {
      Pack16BitArray (&s, 1);
    }
  void PackUnsignedInt (const unsigned int i)
    {
      Pack32BitArray (&i, 1);
    }
  void PackLong (const long l)
    {
      Pack32BitArray (&l, 1);
    }
  void PackUnsignedLong (const unsigned long l)
    {
      Pack32BitArray (&l, 1);
    }
  void PackFloat (const float f)
    {
      Pack32BitArray (&f, 1);
    }
  void PackDouble (const double d)
    {
      Pack64BitArray (&d, 1);
    }

  //

  char UnpackChar (void)
    {
      char c;
      Unpack8BitArray (&c, 1);
      return c;
    }
  unsigned char UnpackUnsignedChar (void)
    {
      unsigned char c;
      Unpack8BitArray (&c, 1);
      return c;
    }
  short UnpackShort (void)
    {
      short s;
      Unpack16BitArray (&s, 1);
      return s;
    }
  unsigned short UnpackUnsignedShort (void)
    {
      unsigned short s;
      Unpack16BitArray (&s, 1);
      return s;
    }
  unsigned int UnpackUnsignedInt (void)
    {
      unsigned int i;
      Unpack32BitArray (&i, 1);
      return i;
    }
  long UnpackLong (void)
    {
      long l;
      Unpack32BitArray (&l, 1);
      return l;
    }
  unsigned long UnpackUnsignedLong (void)
    {
      unsigned long l;
      Unpack32BitArray (&l, 1);
      return l;
    }
  float UnpackFloat (void)
    {
      float f;
      Unpack32BitArray (&f, 1);
      return f;
    }
  double UnpackDouble (void)
    {
      double d;
      Unpack64BitArray (&d, 1);
      return d;
    }

  //
  // real array inline functions
  //

  void PackCharArray (const char *c, int size)
    {
      Pack8BitArray (c, size);
    }
  void PackUnsignedCharArray (const unsigned char *c, int size)
    {
      Pack8BitArray (c, size);
    }
  void PackShortArray (const short *s, int size)
    {
      Pack16BitArray (s, size);
    }
  void PackUnsignedShortArray (const unsigned short *s, int size)
    {
      Pack16BitArray (s, size);
    }
  void PackIntArray (const int *i, int size)
    {
      Pack32BitArray (i, size);
    }
  void PackUnsignedIntArray (const unsigned int *i, int size)
    {
      Pack32BitArray (i, size);
    }
  void PackLongArray (const long *l, int size)
    {
      Pack32BitArray (l, size);
    }
  void PackUnsignedLongArray (const unsigned long *l, int size)
    {
      Pack32BitArray (l, size);
    }
  void PackFloatArray (const float *f, int size)
    {
      Pack32BitArray (f, size);
    }
  void PackDoubleArray (const double *d, int size)
    {
      Pack64BitArray (d, size);
    }

  //

  void UnpackCharArray (char *c, int size)
    {
      Unpack8BitArray (c, size);
    }
  void UnpackUnsignedCharArray (unsigned char *c, int size)
    {
      Unpack8BitArray (c, size);
    }
  void UnpackShortArray (short *s, int size)
    {
      Unpack16BitArray (s, size);
    }
  void UnpackUnsignedShortArray (unsigned short *s, int size)
    {
      Unpack16BitArray (s, size);
    }
  void UnpackIntArray (int *i, int size)
    {
      Unpack32BitArray (i, size);
    }
  void UnpackUnsignedIntArray (unsigned int *i, int size)
    {
      Unpack32BitArray (i, size);
    }
  void UnpackLongArray (long *l, int size)
    {
      Unpack32BitArray (l, size);
    }
  void UnpackUnsignedLongArray (unsigned long *l, int size)
    {
      Unpack32BitArray (l, size);
    }
  void UnpackFloatArray (float *f, int size)
    {
      Unpack32BitArray (f, size);
    }
  void UnpackDoubleArray (double *d, int size)
    {
      Unpack64BitArray (d, size);
    }

  //

  int FixSize (void)
    {
      int save = _written;
      _written = 0;
      PackInt (save - 8);
      PackInt (0);
      return _written = save;
    }
};


#endif // _CCHUNK_H
