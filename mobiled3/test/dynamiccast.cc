//
//
//


#include <iostream.h>

class CBASE
{
protected:
  int isBase;
public:
  CBASE (void) {isBase = 1;}
  void *my_dynamic_cast (const char *name)
    {
      if (isBase) return 0x0;
      return this;
    }
};

class CDERIVED : public CBASE
{
public:
  CDERIVED (void) {isBase = 0;}
};

#define DYNAMIC_CAST(type,expr) (type *)((expr)->my_dynamic_cast(#type))

int main (void)
{
  cout << (void *) DYNAMIC_CAST(CDERIVED,new CBASE) << endl;
  cout << (void *) DYNAMIC_CAST(CDERIVED,new CDERIVED) << endl;
}
