//
//
//


#include <iostream.h>


class TEST
{
protected:

  TEST *checker (void)
    {
      delete this;
      return 0;
    }

public:

  TEST (void)
    {
    }
  ~TEST (void)
    {
    }

  TEST *creator (void)
    {
      TEST *test = new TEST;
      return test->checker ();
    }

};


int main (void)
{
  TEST test, *newTest;

  while (42)
    {
      newTest = test.creator ();
    }
}
