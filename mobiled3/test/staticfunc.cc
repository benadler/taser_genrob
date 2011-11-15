//
//
//


#include <iostream.h>


class KLASSE
{
public:

  static void Dump (void)
    {
      cerr << "statisch" << endl;
    }

  void Dump (void)   // BOOM!
    {
      cerr << "dynamisch" << endl;
    }

};


int main (void)
{
  KLASSE::Dump ();

  KLASSE klasse;
  klasse.Dump ();
}
