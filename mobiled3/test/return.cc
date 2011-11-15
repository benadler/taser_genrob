//
//
//

#include <iostream.h>

class KLASSE
{
public:
  KLASSE (void) {cerr << __PRETTY_FUNCTION__ << endl;}
  ~KLASSE (void) {cerr << __PRETTY_FUNCTION__ << endl;}
  KLASSE (const KLASSE &) {cerr << __PRETTY_FUNCTION__ << endl;}
  KLASSE variante1 (void);
  KLASSE variante2 (void);
  KLASSE variante3 (void);
};

KLASSE KLASSE::variante1 (void)
{
  cerr << __PRETTY_FUNCTION__ << endl;
  return KLASSE ();
}

KLASSE KLASSE::variante2 (void)
{
  cerr << __PRETTY_FUNCTION__ << endl;
  KLASSE k;
  return k;
}

KLASSE KLASSE::variante3 (void) return k;
{
  cerr << __PRETTY_FUNCTION__ << endl;
  return;
}

int main (void)
{
  KLASSE k;

  cerr << "-------------" << endl;
  k.variante1 ();
  cerr << "-------------" << endl;
  k.variante2 ();
  cerr << "-------------" << endl;
  k.variante3 ();
  cerr << "-------------" << endl;
}
