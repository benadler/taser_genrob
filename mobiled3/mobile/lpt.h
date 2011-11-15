//
//
//

#ifndef _LPT_H_
#define _LPT_H_


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CLPT
{
private:

  // not even implemented
  CLPT (void);
  CLPT (const CLPT &);
  CLPT &operator= (const CLPT &);
  bool operator== (const CLPT &);

protected:

  int _fd;

public:

  CLPT (const char *devName);
  virtual ~CLPT (void);

  void Out (unsigned char val);
};

#endif
