//
//
//

#define _REENTRANT

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#include <streamsock.h>


class CTHREAD
{
protected:

  pthread_t id;

public:

  CTHREAD (void * (*func) (void *), void *arg)
    {
      pthread_attr_t attr;
      pthread_attr_init (&attr);
      pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
      pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);

      pthread_create (&id, &attr, func, arg);
    }

  virtual ~CTHREAD (void)
    {
    }
};


#define COUNT 10000
#define MSGSIZE 64

void *sender (void *arg)
{
  struct timeval tv = {1, 0};
  ::select (0, 0x0, 0x0, 0x0, &tv);

  CSTREAMSOCK sock;
  sock.Connect (0x7f000001, 2222);

  int total = 0;

  for (int i=0; i<COUNT; i++)
    {
      // send message
      int size = MSGSIZE - 4;
      char buf[MSGSIZE];
      *(int *)buf = size;
      sock.WriteAll (MSGSIZE, buf); total += MSGSIZE;

      // read reply
      sock.ReadAll (4, &size); total += 4;
      sock.ReadAll (size, buf); total += size;
    }

  printf ("total: %i\n", total);

  return 0x0;
}


static void mark (const char *s)
{
  struct timeval tv;
  ::gettimeofday (&tv, 0x0);
  // printf ("%6li %s\n", tv.tv_usec, s);
}


void *receiver (void *arg)
{
  CSTREAMSOCK sk;
  sk.Bind (2222);
  sk.Listen (1);
  CSTREAMSOCK *sock = sk.Accept ();

  while (42)
    {
      // read message

      int size;
mark ("reading(4)...");
      if (sock->ReadAll (4, &size) < 1)
	{
	  abort ();
	}
mark ("done, reading(MSGSIZE)...");
      char buf[MSGSIZE];
      sock->ReadAll (size, buf);

      // send reply

      size = MSGSIZE - 4;
      *(int *)buf = size;
mark ("done, writing(4+MSGSIZE)...");
 sock->WriteAll (MSGSIZE, buf);
mark ("done\n");
    }
}


int main (void)
{
  CTHREAD recvThread (receiver, 0x0);
  CTHREAD sendThread (sender, 0x0);

  while (42)
    {
      ::select (0, 0x0, 0x0, 0x0, 0x0);
    }
}
