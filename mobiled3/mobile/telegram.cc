//
//
//

#include <stdlib.h>
#include <string.h>

#include "mobile/telegram.h"

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#define MAGIC 0x4F4C4548


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CTELEGRAM::CTELEGRAM (void)
{
  GTL ("CTELEGRAM::CTELEGRAM");

  _bufSize = 8192;

  if (!(_buf = (unsigned char *)malloc (_bufSize)))
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  _inBuf = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CTELEGRAM::CTELEGRAM (const int bufSize, const unsigned char *buf)
{
  GTL ("CTELEGRAM::CTELEGRAM(int,uchar*)");

  _bufSize = bufSize;

  if (!(_buf = (unsigned char *)malloc (_bufSize)))
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  memcpy (_buf, buf, bufSize);

  _inBuf = _bufSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CTELEGRAM::~CTELEGRAM (void)
{
  GTL ("CTELEGRAM::~CTELEGRAM");

  free (_buf);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

unsigned int CTELEGRAM::ComputeCrc (unsigned int len) const
{
  GTL ("CTELEGRAM::ComputeCrc");

  if (len < 4)
    {
      GTLFATAL (("too short\n"));
      abort ();
    }

  //

  static unsigned int crctab[256];

  static bool initialized = false;

  if (!initialized)
    {
      for (int i = 0; i < 256; i++)
	{
	  unsigned int crc = i << 24;

	  for (int j = 0; j < 8; j++)
	    {
	      if (crc & 0x80000000)
		{
		  crc = (crc << 1) ^ 0x04c11db7;
		}
	      else
		{
		  crc = crc << 1;
		}
	    }

	  crctab[i] = crc;
	}
    }

  //

  unsigned char *data = _buf;

  unsigned int result;

  result = *data++ << 24;
  result |= *data++ << 16;
  result |= *data++ << 8;
  result |= *data++;

  result = ~ result;

  len -= 4;

  while (len-- > 0)
    {
      result = (result << 8 | *data++) ^ crctab[result >> 24];
    }

  return ~result;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CTELEGRAM::PeekS32 (int offset)
{
  if (offset > _inBuf - 4)
    {
      fprintf (stderr, "too short\n");
      abort ();
    }

  return *(int *)(_buf + offset);
}


unsigned int CTELEGRAM::PeekU32 (int offset)
{
  if (offset > _inBuf - 4)
    {
      fprintf (stderr, "too short\n");
      abort ();
    }

  return *(unsigned int *)(_buf + offset);
}


float CTELEGRAM::PeekF32 (int offset)
{
  if (offset > _inBuf - 4)
    {
      fprintf (stderr, "too short\n");
      abort ();
    }

  return *(float *)(_buf + offset);
}


//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRCVBUFFER::~CRCVBUFFER (void)
{
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CRCVBUFFER::ReceiveData (CSTREAMSOCK *sock, const bool block)
{
  GTL ("CRCVBUFFER::ReceiveData(CSTREAMSOCK)");

  int done, todo = _bufSize - _inBuf;

  if (block)
    {
      done = sock->Read (todo, _buf + _inBuf);
    }
  else
    {
      done = sock->ReadSome (todo, _buf + _inBuf);
    }

  GTLPRINT (("got %i bytes\n", done));

  if (done < 1)   // happens only when remote aborts the connection
    {
      return -1;
    }

  _inBuf += done;

  GTLPRINT (("got %i bytes total\n", _inBuf));

#if 0

  for (int offset=0; offset<_inBuf; offset+=4)
    {
      GTLPRINT (("0x%08x\n", PeekU32 (offset)));
    }

#endif

  return done;
}


///////////////////////////////////////////////////////////////////////////////
//
// CRCVBUFFER::PopTelegram()
//
// pops one telegram off the data available so far (if possible). in case of
// any sanity errors it throws an exception to be caught by CCLIENT::Fxn(),
// which then closes this communication channel.
//
///////////////////////////////////////////////////////////////////////////////

CRCVTELEGRAM *CRCVBUFFER::PopTelegram (void)
{
  GTL ("CRCVBUFFER::PopTelegram()");

  // check magic

  if (_inBuf < 4)
    {
      GTLPRINT (("not enough data #1\n"));
      return 0x0;
    }

  const int magic = PeekU32 (0);

#if 0
  if (magic == MAGIC)   // deliberate error! (used for testing)
#else
  if (magic != MAGIC)
#endif
    {
      GTLFATAL (("magic failed\n"));
      throw -1;
    }

  // check rest of header

  if (_inBuf < 8)
    {
      GTLPRINT (("not enough data #2\n"));
      return 0x0;
    }

  const int size = PeekU32 (4);

  if (size > _bufSize)
    {
      GTLFATAL (("too large\n"));
      throw -1;
    }

  if (_inBuf < size)
    {
      GTLPRINT (("not enough data #3\n"));
      return 0x0;
    }

  //

  const unsigned int crc = ComputeCrc (size - 4);

  if (PeekU32 (size - 4) != crc)
    {
      GTLFATAL (("crc failed (0x%08x in telegram vs. 0x%08x self)\n",
		 PeekU32 (size - 4), crc));
      throw -1;
    }

  //

  CRCVTELEGRAM *ret = new CRCVTELEGRAM (size, _buf);

  //

  if ((_inBuf -= size) > 0)
    {
      memmove (_buf, _buf + size, _inBuf);
    }

  //

#if 0

  for (int offset=0; offset<size; offset+=4)
    {
      GTLFATAL (("0x%08x\n", ret->PeekU32 (offset)));
    }

#endif

  return ret;
}


//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRCVTELEGRAM::CRCVTELEGRAM (const int bufSize, const unsigned char *buf)
  : CTELEGRAM (bufSize, buf)
{
  _offset = 8;
  _top = bufSize - 4;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRCVTELEGRAM::~CRCVTELEGRAM (void)
{
  GTL ("CRCVTELEGRAM::~CRCVTELEGRAM");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CRCVTELEGRAM::PopS32 (void)
{
  GTL (("CRCVTELEGRAM::PopS32()"));

  if (_offset >= _top)
    {
      GTLFATAL (("telegram data underrun\n"));
      throw -1;
    }

  const int ret = PeekS32 (_offset);

  _offset += 4;

  return ret;
}


unsigned int CRCVTELEGRAM::PopU32 (void)
{
  GTL (("CRCVTELEGRAM::PopU32()"));

  if (_offset >= _top)
    {
      GTLFATAL (("telegram data underrun\n"));
      throw -1;
    }

  const unsigned int ret = PeekU32 (_offset);

  _offset += 4;

  return ret;
}


float CRCVTELEGRAM::PopF32 (void)
{
  GTL (("CRCVTELEGRAM::PopF32()"));

  if (_offset >= _top)
    {
      GTLFATAL (("telegram data underrun\n"));
      throw -1;
    }

  const float ret = PeekF32 (_offset);

  _offset += 4;

  return ret;
}


//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSNDTELEGRAM::CSNDTELEGRAM (unsigned int cmd)
{
  _ptr = _buf;

  PushU32 (MAGIC);        // <MAGIC> ("HELO")
  PushU32 (0x00000000);   // dummy <SIZE>

  PushU32 (cmd);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CSNDTELEGRAM::~CSNDTELEGRAM (void)
{
  GTL ("CSNDTELEGRAM::~CSNDTELEGRAM");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CSNDTELEGRAM::Send (CSTREAMSOCK *sock)
{
  GTL ("CSNDTELEGRAM::Send()");

  PushU32 (0x00000000);   // dummy <CRC>

  // fix buffer

  PutU32 (_inBuf, 4);

  unsigned int crc = ComputeCrc (_inBuf - 4);

  PutU32 (crc, _inBuf - 4);

  //

#if 0

  for (int offset=0; offset<_inBuf; offset+=4)
    {
      GTLFATAL (("0x%08x\n", PeekU32 (offset)));
    }

#endif

  // send buffer

  if (sock->Write (_inBuf, _buf) != _inBuf)
    {
      GTLFATAL (("socket write failed\n"));
      return -1;
    }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CSNDTELEGRAM::PushS32 (const int i)
{
  if ((_inBuf += 4) > _bufSize)
    {
      fprintf (stderr, "buffer overflow\n");
      abort ();
    }

  *((int *)_ptr)++ = i;
}


void CSNDTELEGRAM::PushU32 (const unsigned int i)
{
  if ((_inBuf += 4) > _bufSize)
    {
      fprintf (stderr, "buffer overflow\n");
      abort ();
    }

  *((unsigned int *)_ptr)++ = i;
}


void CSNDTELEGRAM::PushF32 (const float f)
{
  if ((_inBuf += 4) > _bufSize)
    {
      fprintf (stderr, "buffer overflow\n");
      abort ();
    }

  *((float *)_ptr)++ = f;
}


void CSNDTELEGRAM::PutS32 (const int i, const int offset)
{
  if ((offset > _inBuf - 4))
    {
      fprintf (stderr, "buffer overflow\n");
      abort ();
    }

  *(int *)(_buf + offset) = i;
}


void CSNDTELEGRAM::PutU32 (const unsigned int i, const int offset)
{
  if ((offset > _inBuf - 4))
    {
      fprintf (stderr, "buffer overflow\n");
      abort ();
    }

  *(unsigned int *)(_buf + offset) = i;
}
