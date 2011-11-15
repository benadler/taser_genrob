//
//
//

#include <unistd.h>

#include "labdev/labdev.h"
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CMAPCLIENT : public CLABDEV
{
public:

  CMAPCLIENT (void);
  virtual ~CMAPCLIENT (void);

  int GetMapImage (void **image);
};


//
//
//

CMAPCLIENT::CMAPCLIENT (void) : CLABDEV ("mapd")
{
  GTL ("CMAPCLIENT::CMAPCLIENT()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAPCLIENT::~CMAPCLIENT (void)
{
  GTL ("CMAPCLIENT::~CMAPCLIENT()");
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CMAPCLIENT::GetMapImage (void **image)
{
  GTL ("CMAPCLIENT::GetMapImage()");

  CLABMSG msg;

  msg._cmd = CMD_GET_MAP_IMAGE;
  msg._intData[0] = 648;
  msg._intData[1] = 280;

  if (RequestService (msg) < 0)
    {
      GTLPRINT (("error communicating with remote service\n"));
      return -1;
    }

  if (!(*image = malloc (msg._auxDataSize)))
    {
      GTLFATAL (("out of memory\n"));
      return -1;
    }

  memcpy (*image, msg._auxData, msg._auxDataSize);

  return msg._auxDataSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{
  if (argc == 2)
    {
      int ms = atoi (argv[1]);

      if (ms < 0)
	{
	  ms = 0;
	}

      if (ms)
	{
	  usleep (1000 * ms);
	}
    }

  //

  CMAPCLIENT *mapClient;

  try
    {
      mapClient = new CMAPCLIENT;
    }

  catch (...)
    {
      return -1;
    }

  void *data;
  int size;

  if ((size = mapClient->GetMapImage (&data)) < 0)
    {
      return -1;
    }

  write (fileno (stdout), data, size);

  free (data);

  return 0;
}
