//
//
//

#include <stdio.h>

#include "tsmouse.H"

int main (int argc, char **argv)
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <devicename>\n", argv[0]);
      return -1;
    }

  // bernoulli: /dev/ttyS0 -> port #2
  // bernoulli: /dev/ttyS1 -> port #1

  TSMouse smouse (argv[1]);

  smouse.SetNullRadius (15);
  smouse.SetDataRate (13, 4);
  smouse.SetSensitivity (8, 8);
  smouse.BeepOnKey (TRUE);
  // smouse.SetRotationMatrix (ROTX (1.570796327));
  smouse.SetRotationMatrix (ROTX_90);
  smouse.RotateOn (TRUE);

  //

  while (42)
    {
      unsigned char buf[32];

      if (smouse.ReadCommand (buf) > 0)
	{
	  switch (buf[0])
	    {
	    case 'd':
	      {
		TMousePos MPos;

		if (smouse.DecodeData (&MPos, buf) == 0)
		  {
		    printf ("Pos: x=%4li, y=%4li, z=%4li, rx=%4li, ry=%4li, rz=%4li\n",
			    MPos.tx, MPos.ty, MPos.tz,
			    MPos.rx, MPos.ry, MPos.rz);
		  }
		else
		  {
		    printf("Receive error");
		  }
	      }
	      break;

	    case 'k':
	      {
		int key = smouse.DecodeKey (buf);

		printf ("key %i\n", key);
	      }
	    }
	}
    }

  return 0;
}
