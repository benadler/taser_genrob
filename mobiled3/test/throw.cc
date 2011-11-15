//
//
//


#include <stdlib.h>

#include <iostream.h>


class CCLASS
{
public:
  void *operator new (size_t size)
  {
    cout << __PRETTY_FUNCTION__ << endl;
    return ::malloc (size);
  }
  void operator delete (void *ptr)
  {
    cout << __PRETTY_FUNCTION__ << endl;
    ::free (ptr);
  }
  CCLASS (void)
  {
    cout << __PRETTY_FUNCTION__ << endl;
    throw "willnich";
  }
  ~CCLASS (void)
  {
    cout << __PRETTY_FUNCTION__ << endl;
  }
};


int main (void)
{
  CCLASS *c;

  try
    {
      c = new CCLASS;
    }

  catch (char *msg)
    {
      cout << msg << endl;
    }
}
