//
//
//

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>   // XSizeHints

int main (void)
{
  //
  //
  //

  Display *display = XOpenDisplay (NULL);

  if (!display)
    {
      fprintf (stderr, "can't connect to xserver\n");
      abort ();
    }

  //
  //
  //

  Window win = XCreateSimpleWindow (display,
				    RootWindow (display, DefaultScreen (display)),
				    0, 0,
				    800, 600,
				    0,
				    BlackPixel (display, DefaultScreen (display)),
				    WhitePixel (display, DefaultScreen (display)));

  XSizeHints hints;

  hints.flags = PPosition | PSize | PMinSize | PMaxSize;
  hints.x = 0;
  hints.y = 0;
  hints.width = hints.min_width = hints.max_width = 800;
  hints.height = hints.min_height = hints.max_height = 600;

  XSetNormalHints (display, win, &hints);

  XStoreName (display, win, "<no title>");

  XSelectInput (display, win,
		ButtonPressMask | KeyPressMask | ExposureMask);

  XGCValues xgcvalues;

  GC gc = XCreateGC (display, win, 0, &xgcvalues);

  XSetFont (display, gc, XLoadQueryFont (display, "9x15")->fid);
  XSetForeground (display, gc, BlackPixel (display, DefaultScreen (display)));
  XSetLineAttributes (display, gc, 0, LineSolid, CapRound, JoinRound);

  XSetFunction (display, gc, GXcopy);

#if 0

  // set backing store

  XSetWindowAttributes winattr;

  winattr.backing_store = Always;

  XChangeWindowAttributes (display, win, CWBackingStore, &winattr); 

#endif

  XEvent ev;

  XMapWindow (display, win);

  XNextEvent (display, &ev);   // pop the map event

  //
  //
  //

  int lines = 0;
  struct timeval start;
  gettimeofday (&start, 0x0);

  while (42)
    {
      struct timeval now;

      gettimeofday (&now, 0x0);

      int ms =
	(now.tv_sec - start.tv_sec) * 1000 +
	(now.tv_usec - start.tv_usec) / 1000;

      if (ms >= 1000)
	{
	  start = now;
	  printf ("************* %i lines/s\n", lines);
	  fflush (stdout);
	  lines = 0;
	}

      lines += 800;

      //
      //
      //

      XSetForeground (display,
		      gc,
		      WhitePixel (display, DefaultScreen (display)));

      // XSetFillStyle (display, gc, FillSolid);

      XSetFunction (display,
		    gc,
		    GXcopy);

      XFillRectangle (display, win, gc, 0, 0, 799, 599);

      //
      //
      //

      XSetForeground (display,
		      gc,
		      BlackPixel (display, DefaultScreen (display)));

      XSetFunction (display, gc, GXcopy);

      for (int x=0; x<=799; x++)
	{
	  XDrawLine (display, win, gc, x, 0, 799-x, 599);
	}

      XFlush (display);

      //
      //
      //

      //sleep (1);
    }

  //
  //
  //

  XUnmapWindow (display, win);   // raises no event?

  XDestroyWindow (display, win);

  XFlush (display);

  return 0;
}
