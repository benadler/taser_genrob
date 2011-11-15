//
//
//

#ifndef _TELEGRAM_H_
#define _TELEGRAM_H_

#include "sock/streamsock.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CTELEGRAM
{
private:

  // not even implemented
  CTELEGRAM (const CTELEGRAM &);
  CTELEGRAM &operator= (const CTELEGRAM &);
  bool operator== (const CTELEGRAM &);

protected:

  int _inBuf;
  int _bufSize;
  unsigned char *_buf;

  unsigned int ComputeCrc (unsigned int len) const;

  CTELEGRAM (void);
  CTELEGRAM (const int bufSize, const unsigned char *buf);

  int          PeekS32 (int offset);   // inline?
  unsigned int PeekU32 (int offset);   // inline?
  float        PeekF32 (int offset);   // inline?

public:

  virtual ~CTELEGRAM (void);
};


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CRCVTELEGRAM : protected CTELEGRAM
{
  friend class CRCVBUFFER;

private:

  // not even implemented

  CRCVTELEGRAM (void);
  CRCVTELEGRAM (const CRCVTELEGRAM &);
  CRCVTELEGRAM &operator= (const CRCVTELEGRAM &);
  bool operator== (const CRCVTELEGRAM &);

protected:

  CRCVTELEGRAM (const int bufSize, const unsigned char *buf);

  int _offset;
  int _top;

public:

  virtual ~CRCVTELEGRAM (void);

  int          PopS32 (void);   // inline?
  unsigned int PopU32 (void);   // inline?
  float        PopF32 (void);   // inline?
};


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CRCVBUFFER : protected CTELEGRAM
{
private:

  // not even implemented

  CRCVBUFFER (const CRCVBUFFER &);
  CRCVBUFFER &operator= (const CRCVBUFFER &);
  bool operator== (const CRCVBUFFER &);

protected:

public:

  CRCVBUFFER (void)
  {
  }

  virtual ~CRCVBUFFER (void);

  int ReceiveData (CSTREAMSOCK *sock, const bool block = false);

  CRCVTELEGRAM *PopTelegram (void);
};


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CSNDTELEGRAM : protected CTELEGRAM
{
private:

  // not even implemented
  CSNDTELEGRAM (void);
  CSNDTELEGRAM (const CSNDTELEGRAM &);
  CSNDTELEGRAM &operator= (const CSNDTELEGRAM &);
  bool operator== (const CSNDTELEGRAM &);

protected:

  unsigned char *_ptr;

public:

  CSNDTELEGRAM (unsigned int cmd);

  virtual ~CSNDTELEGRAM (void);

  //

  int Send (CSTREAMSOCK *sock);

  // may be made inline

  void PushS32 (const int i);
  void PushU32 (const unsigned int i);
  void PushF32 (const float f);

  void PutS32 (const int i, const int offset);
  void PutU32 (const unsigned int i, const int offset);
};


#endif
