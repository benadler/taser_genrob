#include "tsmouse.H"
#include <stdio.h>

class mymouse : public TSMouse
{
public:
  mymouse (char *Name);
  virtual void ErrorState(void);
};

mymouse::mymouse (char *Name)
  :TSMouse(Name)
{
  return;
}

void mymouse::ErrorState(void)
{
  cout << "We are in the demo Error function\n";
  abort();
}


int main(void)
{
  TMousePos     MPos;
  mymouse       smouse("/dev/ttyS1");
  int ds;
  unsigned char buf[32];

  if (smouse.IsError())
    {
      cout << "Error in construction Code " << smouse.IsError() << endl;
      perror(" Starting Mouse ");
      exit(0);
    }
  
  smouse.SetNullRadius(15);
  smouse.SetDataRate(13,4);
  smouse.SetSensitivity(8,8);
  smouse.BeepOnKey(TRUE);
  //  smouse.SetRotationMatrix(ROTX(1.570796327));
  smouse.SetRotationMatrix(ROTX_90);
  smouse.RotateOn(TRUE);
   do
    {
      ds = smouse.ReadCommand(buf);
      if (ds > 0)
        {
          if (buf[0] == 'd')// ** Ein Datensatz mit Mauspos. wurde empfangen **
            {
              if (smouse.DecodeData(&MPos,buf) == 0)
                printf("Pos : x=%4li, y=%4li, z=%4li, rx=%4li, ry=%4li, rz=%4li\n",
                       MPos.tx,MPos.ty,MPos.tz,MPos.rx,MPos.ry,MPos.rz);
              else
                printf("Recive error");
            }
          else if (buf[0] == 'k')  
            printf("Keymask : 0x%lx\n",smouse.DecodeKey(buf));
          else if (buf[0] == 'b')  
            printf("Beep ");
          else  
            smouse.Show(buf,ds);
        }
      else
        smouse.ErrorState();
    }while ((buf[0] != 'k') || ((smouse.DecodeKey(buf) & KEY8) != KEY8));
  
}
