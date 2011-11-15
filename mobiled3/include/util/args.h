//
//
//

#ifndef _ARGS_H_
#define _ARGS_H_


class CARGS
{
private:

  // cannot be allocated on heap
  void *operator new (size_t);

  // not even implemented
  CARGS (const CARGS &);

protected:

  unsigned int _maxArgs;
  unsigned int _numArgs;
  char *_s;   // this is the real string (but with \0s in it)
  char **_arg;   // these are just pointers into `_s'...

  int _individuallyMalloced;   // ...if this is false

  unsigned int _offset;

  void Clear (void);

public:

  CARGS (void);
  CARGS (const char *s);
  CARGS (const char *s, const char *delim);
  CARGS (const char *s, const CARGS &outerArgs);
  ~CARGS (void);

  void Create (const char *s, const char *delim);

#if 0
  CARGS &operator= (const char *s);
#endif
  CARGS &operator= (const CARGS &right);

  char *Dump (void) const;

  const int GetNumArgs (void) const
  {
    return _numArgs - _offset;
  }

  char *operator* (void) const;
  char *operator[] (const unsigned int idx) const;

  CARGS &operator++ (void);   // prefix

#if 0

  const CARGS operator++ (const int)   // postfix
    {
      CARGS tmp = *this;
      this->operator++ ();
      return tmp;
    }

#endif
};


#endif
