#define SIZE 65536

int main (void)
{
  void *ptr;
  int size = 0;

  while (42)
    {
      if (!(ptr = malloc (SIZE)))
	{
	  sleep (5);
	  continue;
	}

      memset (ptr, 0, SIZE);
      size += SIZE;

      printf ("size=%i, %ik, %im\n", size, size>>10, size>>20);
    }
}
