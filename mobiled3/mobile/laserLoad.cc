//
// laserLoad.cc
//

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>


static int globCnt = 0;


static void sigalrm (int sig)
{
  printf ("%i\n", globCnt);
  fflush (stdout);

  globCnt = 0;
}


int main (void)
{
  //

  nice (20);

  //

  struct sigaction sa;

  sa.sa_flags = 0;
  sa.sa_handler = sigalrm;

  if (sigaction (SIGALRM, &sa, 0x0) < 0)
    {
      perror ("sigaction()");
      return -1;
    }

  struct itimerval it;

  it.it_value.tv_sec = 1;
  it.it_value.tv_usec = 1;
  it.it_interval.tv_sec = 1;
  it.it_interval.tv_usec = 1;

  if (setitimer (ITIMER_REAL, &it, 0x0) < 0)
    {
      perror ("setitimer()");
      return -1;
    }

  while (42)
    {
      globCnt++;
    }
}
