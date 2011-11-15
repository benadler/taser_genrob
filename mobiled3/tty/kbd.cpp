//
//
//

#include <stdio.h>
#include <signal.h>

#if defined (__WATCOMC__)
  #include <sys/select.h>
#endif

#include "tty/kbd.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CKBD::CKBD (void) : CTTY (fileno (stdin))
{
  SaveFlags ();

  SetRaw ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CKBD::~CKBD (void)
{
  RestoreFlags ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int CKBD::WaitIn (const milliseconds_t ms) const
{
  fd_set rfd;

  FD_ZERO (&rfd);
  FD_SET (_fd, &rfd);

  struct timeval tv = {ms / 1000, (ms % 1000) * 1000}, *tvp;

  if (ms < 0)
    {
      tvp = 0x0;
    }
  else
    {
      tvp = &tv;
    }

  // this either returns a negative error, 0 if no data is available or +1 if
  // data is available

  return select (_fd + 1, &rfd, 0x0, 0x0, tvp);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

unsigned long CKBD::GetKey (const milliseconds_t timeout) const
{
  unsigned char c[] = {0, 0, 0, 0, 0, 0, 0, 0};

  if (WaitIn (timeout) < 1)
    {
      return 0;
    }

  int ret = Read (c, sizeof (c));

  if ((ret == 1) && (c[0] == 3))
    {
      kill (0, SIGINT);
    }

  return *(unsigned long *)c;
}
