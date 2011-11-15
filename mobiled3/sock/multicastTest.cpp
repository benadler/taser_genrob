//
// multicastsock.h -
// (C) 11/2001 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sock/multicastsock.h"
#include "thread/tracelog.h"


int main (void)
{
  //

  srandom (time (0));

  //

  CMULTICASTSOCK receiver;

  if (receiver.Bind (32767, 0xE6000001) < 0)
    {
      return -1;
    }

  //

  CMULTICASTSOCK sender;

  if (sender.Connect (0xE6000001, 32767))
    {
      return -1;
    }

  //

  while (42)
    {
      //

      while (receiver.WaitIn (1) > 0)
	{
	  char c;

	  if (receiver.Recv (1, &c) < 0)
	    {
	      return -1;
	    }

	  printf ("%c", c);
	  fflush (stdout);
	}

      //

#if defined (linux)
      char c = 'l';
#elif defined (__svr4__)
      char c = 's';
#else
      char c = 'x';
#endif

      if (sender.Send (1, &c) < 1)
	{
	  return -1;
	}
    }

  return 0;
}
