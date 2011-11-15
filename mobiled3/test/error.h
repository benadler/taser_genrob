//
//
//


#ifndef _ERROR_H
#define _ERROR_H


class CERROR
{
private:

  // not even implemented
  CERROR (void);
  CERROR &operator= (const CERROR &);

protected:

  char *_inFxn;
  char *_calledFxn;
  int _errNo;
  char *_errStr;

public:

  CERROR (const CERROR &);   // needs a copy constructor for exceptions

  CERROR (const char *inFxn,
	  const char *calledFxn,
	  const int errNo,
	  const char *errStr);
  virtual ~CERROR (void);

  void Dump (void) const;
};


#endif
