//
//
//

#include <math.h>
#include <stdlib.h>

#include "thread/tracelog.h"
#include "util/inetutils.h"

#include "mobile/mobile.h"
#include "protocol.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMOBILE::CMOBILE (const unsigned int addr, const unsigned short port)
{
  GTL ("CMOBILE::CMOBILE(uint,ushort)");
  GTLPRINT (("addr=0x%08x:%i\n", addr, port));

  if (_sock.Connect (addr, port) < 0)
    {
      GTLFATAL (("failed to connect\n"));
      throw -42;
    }

  //

  CheckProtocol ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMOBILE::CMOBILE (const char *hostname, const unsigned short port)
{
  GTL ("CMOBILE::CMOBILE(char*,ushort)");
  GTLPRINT (("hostname=\"%s\"\n", hostname ? hostname : "<NULL>"));

  unsigned int addr;

  if (hostname)
    {
      if (!(addr = _gethostbyname (hostname)))
	{
	  GTLFATAL (("cannot resolve host name\n"));
	  abort ();
	}
    }
  else
    {
      addr = 0x7f000001;   // localhost
    }

  GTLPRINT (("addr=0x%08x:%i\n", addr, port));

  if (_sock.Connect (addr, port) < 0)
    {
      GTLFATAL (("failed to connect\n"));
      throw -42;
    }

  //

  CheckProtocol ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMOBILE::~CMOBILE (void)
{
  GTL ("CMOBILE::~CMOBILE()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CRCVTELEGRAM *CMOBILE::GetReply (unsigned int id)
{
  GTL ("CMOBILE::GetReply()");

  CRCVTELEGRAM *ret;

  while (!(ret = _buffer.PopTelegram ()))
    {
      if (_sock.WaitIn (-1) < 0)
	{
	  GTLPRINT (("WaitIn() failed\n"));
	}

      if (_buffer.ReceiveData (&_sock, false) < 0)
	{
	  GTLFATAL (("ReceiveData() failed\n"));
	  abort ();
	}
    }

  unsigned int cmd = ret->PopU32 ();

  if (cmd != id)
    {
      GTLFATAL (("reply has illegal id (0x%08x vs. 0x%08x expected)\n",
		 cmd,
		 id));
      throw -1;
    }

  return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::Ping (void)
{
  //GTL ("CMOBILE::Ping()");

  CSNDTELEGRAM telegram (CMD_PING);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      //GTLFATAL (("Send() failed\n"));
      return -1;
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_PING);

      status = reply->PopS32 ();
    }

  catch (...)
    {
      //GTLFATAL (("protocol error\n"));
      return -1;
    }

  delete reply;

  //

  return status;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CMOBILE::CheckProtocol (void)
{
  GTL ("CMOBILE::CheckProtocol()");

  CSNDTELEGRAM telegram (CMD_CHECKPROTOCOL);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;
  unsigned int version;
  unsigned int count;
  unsigned int *cmd;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_CHECKPROTOCOL);

      status = reply->PopS32 ();
      version = reply->PopU32 ();
      count = reply->PopU32 ();

      if (!(cmd = (unsigned int *)malloc (count * sizeof (unsigned int))))
	{
	  GTLFATAL (("out of memory\n"));
	  throw -1;
	}

      for (unsigned int i=0; i<count; i++)
	{
	  cmd[i] = reply->PopU32 ();
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

#if 1

  GTLPRINT (("supported commands:\n"));

  for (unsigned int i=0; i<count; i++)
    {
      GTLPRINT (("0x%08x\n", cmd[i]));
    }

#endif

  //

  free (cmd);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::Debugging (const int flag)
{
  GTL ("CMOBILE::Debugging()");

  CSNDTELEGRAM telegram (CMD_DEBUGGING);

  telegram.PushS32 (flag);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;
  int oldFlag;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_DEBUGGING);

      status = reply->PopS32 ();
      oldFlag = reply->PopS32 ();
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return oldFlag;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::GetPosition (meter_t &x, meter_t &y, degree_t &a)
{
  GTL ("CMOBILE::GetPosition()");

  //

  CSNDTELEGRAM telegram (CMD_GETPOSITION);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETPOSITION);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}

      x = reply->PopF32 ();
      y = reply->PopF32 ();
      a = reply->PopF32 () * 180.0 / M_PI;
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::GetPositionOdo (meter_t &x, meter_t &y, degree_t &a)
{
  GTL ("CMOBILE::GetPositionOdo()");

  //

  CSNDTELEGRAM telegram (CMD_GETPOSITIONODO);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETPOSITIONODO);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}

      x = reply->PopF32 ();
      y = reply->PopF32 ();
      a = reply->PopF32 () * 180.0 / M_PI;
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::GetPositionAndVariance (meter_t &x, meter_t &y, degree_t &a,
				     double &varXX,
				     double &varXY,
				     double &varYX,
				     double &varYY,
				     double &varAA)
{
  GTL ("CMOBILE::GetPositionAndVariance()");

  //

  CSNDTELEGRAM telegram (CMD_GETPOSITIONANDVARIANCE);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETPOSITIONANDVARIANCE);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}

      x = reply->PopF32 ();
      y = reply->PopF32 ();
      a = reply->PopF32 () * 180.0 / M_PI;

      varXX = reply->PopF32 ();
      varXY = reply->PopF32 ();
      varYX = reply->PopF32 ();
      varYY = reply->PopF32 ();
      varAA = reply->PopF32 ();
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::SetPosition (meter_t x, meter_t y, degree_t a)
{
  GTL ("CMOBILE::SetPosition()");

  //

  CSNDTELEGRAM telegram (CMD_SETPOSITION);

  telegram.PushF32 (x);
  telegram.PushF32 (y);
  telegram.PushF32 (a * M_PI / 180.0);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_SETPOSITION);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}

      x = reply->PopF32 ();   // not actually to be returned...
      y = reply->PopF32 ();
      a = reply->PopF32 ();
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::GetAllMarks (CVEC &marks)
{
  GTL ("CMOBILE::GetAllMarks()");

  //

  CSNDTELEGRAM telegram (CMD_GETALLMARKS);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;
  unsigned int count;
  unsigned int id;
  float x;
  float y;
  float angle;
  float range;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETALLMARKS);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}

      count = reply->PopU32 ();

      marks.Create (2 * (int)count);

      CVEC::datatype_t *ptr = &marks;

      for (unsigned int i=0; i<count; i++)
	{
	  id     = reply->PopU32 ();
	  x      = reply->PopF32 ();
	  y      = reply->PopF32 ();
	  angle  = reply->PopF32 ();
	  range  = reply->PopF32 ();

	  *ptr++ = x;
	  *ptr++ = y;
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return count;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::GetAllMarksInitial (CVEC &marks)
{
  GTL ("CMOBILE::GetAllMarksInitial()");

  //

  CSNDTELEGRAM telegram (CMD_GETALLMARKSINITIAL);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;
  unsigned int count;
  unsigned int id;
  float x;
  float y;
  float angle;
  float range;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETALLMARKSINITIAL);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}

      count = reply->PopU32 ();

      marks.Create (2 * (int)count);

      CVEC::datatype_t *ptr = &marks;

      for (unsigned int i=0; i<count; i++)
	{
	  id     = reply->PopU32 ();
	  x      = reply->PopF32 ();
	  y      = reply->PopF32 ();
	  angle  = reply->PopF32 ();
	  range  = reply->PopF32 ();

	  *ptr++ = x;
	  *ptr++ = y;
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return count;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::GetAllMarksWithVariances (CVEC &marks, CVEC &variances)
{
  GTL ("CMOBILE::GetAllMarksWithVariances()");

  //

  CSNDTELEGRAM telegram (CMD_GETALLMARKSWITHVARIANCES);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;
  unsigned int count;
  unsigned int id;
  float angle;
  float range;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETALLMARKSWITHVARIANCES);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}

      count = reply->PopU32 ();

      marks.Create (2 * (int)count);
      variances.Create (4 * (int)count);

      CVEC::datatype_t *ptr1 = &marks;
      CVEC::datatype_t *ptr2 = &variances;

      for (unsigned int i=0; i<count; i++)
	{
	  id      = reply->PopU32 ();
	  *ptr1++ = reply->PopF32 ();
	  *ptr1++ = reply->PopF32 ();
	  angle   = reply->PopF32 ();
	  range   = reply->PopF32 ();

	  *ptr2++ = reply->PopF32 ();
	  *ptr2++ = reply->PopF32 ();
	  *ptr2++ = reply->PopF32 ();
	  *ptr2++ = reply->PopF32 ();
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return count;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::GetAllLines (CVEC &lines)
{
  GTL ("CMOBILE::GetAllLines()");

  //

  CSNDTELEGRAM telegram (CMD_GETALLLINES);
  
  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply = 0x0;

  int status;
  unsigned int count;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETALLLINES);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}

      count = reply->PopU32 ();

      lines.Create (4 * (int)count);

      CVEC::datatype_t *ptr = &lines;

      for (unsigned int i=0; i<count; i++)
	{
	  reply->PopU32 ();   // id

	  *ptr++ = reply->PopF32 ();
	  *ptr++ = reply->PopF32 ();
	  *ptr++ = reply->PopF32 ();
	  *ptr++ = reply->PopF32 ();
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return count;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMOBILE::CMAP::~CMAP (void)
{
}


int CMOBILE::GetMap (CMAP &map)
{
  GTL ("CMOBILE::GetMap()");

  //

  try
    {
      map.numLines = GetAllLines (map.line);

      GTLPRINT (("%i lines\n", map.numLines));

      map.numMarks = GetAllMarksInitial (map.mark);

      GTLPRINT (("%i marks\n", map.numMarks));
    }

  catch (...)
    {
      GTLFATAL (("failed\n"));
      return -1;
    }

  //

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMOBILE::CSCAN::~CSCAN (void)
{
}


int CMOBILE::GetNumScanners (void)
{
  GTL ("CMOBILE::GetNumScanners()");

  //

  CSNDTELEGRAM telegram (CMD_GETNUMSCANNERS);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply = 0x0;
  unsigned int num = 0;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETNUMSCANNERS);

      reply->PopU32 ();   // status (always 0)

      num = reply->PopU32 ();
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  if (reply)
    {
      delete reply;
    }

  //

  return num;
}


int CMOBILE::GetScannerPosition (meter_t &x,
				 meter_t &y,
				 degree_t &a,
				 const unsigned int idx)
{
  GTL ("CMOBILE::GetScannerPosition()");

  //

  CSNDTELEGRAM telegram (CMD_GETSCANNERPOSITION);

  telegram.PushU32 (idx);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply = 0x0;
  int status = 0;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETSCANNERPOSITION);

      status = reply->PopS32 ();

      x = reply->PopF32 ();
      y = reply->PopF32 ();
      a = reply->PopF32 ();
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  if (reply)
    {
      delete reply;
    }

  //

  return status;
}


int CMOBILE::GetScanPlatform (CSCAN &scan, const unsigned int idx)
{
  GTL ("CMOBILE::GetScanPlatform()");

  scan.numScans = 0;
  scan.numMarks = 0;

  //

  CSNDTELEGRAM telegram (CMD_GETSCANPLATFORM);

  telegram.PushU32 (idx);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply = 0x0;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETSCANPLATFORM);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("remote failed\n"));
	}

      unsigned int numScans = reply->PopU32 ();

      for (unsigned int i=0; i<numScans; i++)
	{
	  scan.sx[i] = reply->PopF32 ();
	  scan.sy[i] = reply->PopF32 ();
	}

      scan.numScans = numScans;

      unsigned int numMarks = reply->PopU32 ();

      for (unsigned int i=0; i<numMarks; i++)
	{
	  scan.mx[i] = reply->PopF32 ();
	  scan.my[i] = reply->PopF32 ();
	  scan.mIdx[i] = reply->PopS32 ();
	}

      scan.numMarks = numMarks;
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  if (reply)
    {
      delete reply;
    }

  //

  return status;
}


int CMOBILE::GetScanWorld (CSCAN &scan, const unsigned int idx)
{
  GTL ("CMOBILE::GetScanWorld()");

  scan.numScans = 0;
  scan.numMarks = 0;

  //

  CSNDTELEGRAM telegram (CMD_GETSCANWORLD);

  telegram.PushU32 (idx);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply = 0x0;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETSCANWORLD);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("remote failed\n"));
	}

      unsigned int numScans = reply->PopU32 ();

      for (unsigned int i=0; i<numScans; i++)
	{
	  scan.sx[i] = reply->PopF32 ();
	  scan.sy[i] = reply->PopF32 ();
	}

      scan.numScans = numScans;

      unsigned int numMarks = reply->PopU32 ();

      for (unsigned int i=0; i<numMarks; i++)
	{
	  scan.mx[i] = reply->PopF32 ();
	  scan.my[i] = reply->PopF32 ();
	  scan.mIdx[i] = reply->PopS32 ();
	}

      scan.numMarks = numMarks;
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  if (reply)
    {
      delete reply;
    }

  //

  return status;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::Stop (void)
{
  GTL ("CMOBILE::Stop()");

  //

  CSNDTELEGRAM telegram (CMD_STOP);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_STOP);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::RotateAngle (const degree_t a)
{
  GTL ("CMOBILE::RotateAngle()");

  //

  CSNDTELEGRAM telegram (CMD_ROTATEANGLE);

  telegram.PushF32 (M_PI * a / 180.0);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_ROTATEANGLE);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed (unknown reason)\n"));
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return status;
}


int CMOBILE::RotatePoint (const meter_t xHead, const meter_t yHead)
{
  GTL ("CMOBILE::RotatePoint()");

  //

  CSNDTELEGRAM telegram (CMD_ROTATEPOINT);

  telegram.PushF32 (xHead);
  telegram.PushF32 (yHead);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_ROTATEPOINT);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed (unknown reason)\n"));
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return status;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::Forward (const meter_t d)
{
  GTL ("CMOBILE::Forward()");

  //

  CSNDTELEGRAM telegram (CMD_FORWARD);

  telegram.PushF32 (d);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_FORWARD);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed (unknown reason)\n"));
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return status;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::MovePoint (const meter_t x, const meter_t y)
{
  GTL ("CMOBILE::MovePoint()");

  //

  CSNDTELEGRAM telegram (CMD_MOVEPOINT);

  telegram.PushF32 (x);
  telegram.PushF32 (y);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_MOVEPOINT);

      if ((status = reply->PopS32 ()) < 0)
	{
	  switch (status)
	    {
	    case -1:
	      GTLFATAL (("failed (start in obstacle)\n"));
	      break;

	    case -2:
	      GTLFATAL (("failed (goal in obstacle)\n"));
	      break;

	    case -3:
	      GTLFATAL (("failed (no path)\n"));
	      break;

	    default:
	      GTLFATAL (("failed (unknown reason)\n"));
	    }

	  return status;
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return status;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::Move (const meter_t x, const meter_t y, const degree_t a)
{
  GTL ("CMOBILE::Move()");

  //

  CSNDTELEGRAM telegram (CMD_MOVE);

  telegram.PushF32 (x);
  telegram.PushF32 (y);
  telegram.PushF32 (a * M_PI / 180.0);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_MOVE);

      if ((status = reply->PopS32 ()) < 0)
	{
	  switch (status)
	    {
	    case -1:
	      GTLFATAL (("failed (start in obstacle)\n"));
	      break;

	    case -2:
	      GTLFATAL (("failed (goal in obstacle)\n"));
	      break;

	    case -3:
	      GTLFATAL (("failed (no path)\n"));
	      break;

	    default:
	      GTLFATAL (("failed (unknown reason)\n"));
	    }

	  return status;
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return status;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::IsCompleted (void)
{
  GTL ("CMOBILE::IsCompleted()");

  //

  CSNDTELEGRAM telegram (CMD_ISCOMPLETED);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;
  unsigned int flag;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_ISCOMPLETED);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}

      flag = reply->PopU32 ();
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return flag;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::WaitForCompleted (void)
{
  GTL ("CMOBILE::WaitForCompleted()");

  //

  CSNDTELEGRAM telegram (CMD_WAITFORCOMPLETED);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_WAITFORCOMPLETED);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed\n"));
	  return status;
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMOBILE::GetScale (double &tScale,
		       double &rScale)
{
  GTL ("CMOBILE::GetScale()");

  //

  CSNDTELEGRAM telegram (CMD_GETSCALE);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETSCALE);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed (unknown reason)\n"));
	}
      else
	{
	  tScale = reply->PopF32 ();
	  rScale = reply->PopF32 ();
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return status;
}


int CMOBILE::SetScale (const double tScale,
		       const double rScale,
		       double &tScaleRet,
		       double &rScaleRet)
{
  GTL ("CMOBILE::SetScale()");

  //

  CSNDTELEGRAM telegram (CMD_SETSCALE);

  telegram.PushF32 (tScale);
  telegram.PushF32 (rScale);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_SETSCALE);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed (unknown reason)\n"));
	}
      else
	{
	  tScaleRet = reply->PopF32 ();
	  rScaleRet = reply->PopF32 ();
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return status;
}


int CMOBILE::ModifyScale (const double tScaleFactor,
			  const double rScaleFactor,
			  double &tScaleOld,
			  double &rScaleOld)
{
  GTL ("CMOBILE::ModifyScale()");

  //

  CSNDTELEGRAM telegram (CMD_MODIFYSCALE);

  telegram.PushF32 (tScaleFactor);
  telegram.PushF32 (rScaleFactor);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_MODIFYSCALE);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("failed (unknown reason)\n"));
	}
      else
	{
	  tScaleOld = reply->PopF32 ();
	  rScaleOld = reply->PopF32 ();
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return status;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

MOBILEMODE CMOBILE::GetMode (void)
{
  GTL ("CMOBILE::GetMode()");

  CSNDTELEGRAM telegram (CMD_GETMODE);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;
  MOBILEMODE mode;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_GETMODE);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("status check failed\n"));
	  return ERROR;
	}

      mode = (MOBILEMODE)reply->PopS32 ();
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return mode;
}


MOBILEMODE CMOBILE::SetMode (const MOBILEMODE mode)
{
  GTL ("CMOBILE::SetMode()");

  CSNDTELEGRAM telegram (CMD_SETMODE);

  telegram.PushS32 ((int)mode);

  if (telegram.Send ((CSTREAMSOCK *)&_sock) < 0)
    {
      GTLFATAL (("Send() failed\n"));
      abort ();
    }

  //

  CRCVTELEGRAM *reply;

  int status;
  MOBILEMODE oldMode, newMode;

  try
    {
      reply = GetReply (CMD_REPLY | CMD_SETMODE);

      if ((status = reply->PopS32 ()) < 0)
	{
	  GTLFATAL (("status check failed\n"));
	  return ERROR;
	}

      oldMode = (MOBILEMODE)reply->PopS32 ();

      if ((newMode = (MOBILEMODE)reply->PopS32 ()) != mode)
	{
	  GTLFATAL (("weirdness: new mode != desired mode\n"));
	  return ERROR;
	}
    }

  catch (...)
    {
      GTLFATAL (("protocol error\n"));
      abort ();
    }

  delete reply;

  //

  return oldMode;
}
