#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include "tsmouse.H"

unsigned char TSMouse::NibbleCodes[] = "0AB3D56GH9:K<MN?";
unsigned char TSMouse::Table6Bits[64] = 
{
  0x80, /*   0 [] */ 
  0x41, /*   1 [A] */ 
  0x42, /*   2 [B] */
  0x83, /*   3 [] */
  0x44, /*   4 [D] */
  0x85, /*   5 [] */
  0x86, /*   6 [] */
  0x47, /*   7 [G] */
  0x48, /*   8 [H] */
  0x89, /*   9 [	] */
  0x8A, /*  10 [ ] */
  0x4B, /*  11 [K] */
  0x8C, /*  12 [] */
  0x4D, /*  13 [M] */
  0x4E, /*  14 [N] */
  0x8F, /*  15 [] */
  0x50, /*  16 [P] */
  0x91, /*  17 [] */
  0x92, /*  18 [] */
  0x53, /*  19 [S] */
  0x94, /*  20 [] */
  0x55, /*  21 [U] */
  0x56, /*  22 [V] */
  0x97, /*  23 [] */
  0x98, /*  24 [] */
  0x59, /*  25 [Y] */
  0x5A, /*  26 [Z] */
  0x9B, /*  27 [] */
  0xDC, /*  28 [\] [\]*/
  0x9D, /*  29 [] */
  0x9E, /*  30 [] */
  0x5F, /*  31 [_] */
  0xE0, /*  32 [`] [`] */
  0xA1, /*  33 [!] */
  0xA2, /*  34 ["] */
  0x63, /*  35 [c] */
  0xA4, /*  36 [$] */
  0x65, /*  37 [e] */
  0x66, /*  38 [f] */
  0xA7, /*  39 ['] */
  0xA8, /*  40 [(] */
  0x69, /*  41 [i] */
  0x6A, /*  42 [j] */
  0xAB, /*  43 [+] */
  0x6C, /*  44 [l] */
  0xAD, /*  45 [-] */
  0xAE, /*  46 [.] */
  0x6F, /*  47 [o] */
  0xB0, /*  48 [0] */
  0x71, /*  49 [q] */
  0x72, /*  50 [r] */
  0xB3, /*  51 [3] */
  0x74, /*  52 [t] */
  0xB5, /*  53 [5] */
  0xB6, /*  54 [6] */
  0x77, /*  55 [w] */
  0x78, /*  56 [x] */
  0xB9, /*  57 [9] */
  0xBA, /*  58 [:] */
  0x7B, /*  59 [{] */
  0xBC, /*  60 [<] */
  0x7D, /*  61 [}] */
  0x3E, /*  62 [>] [~] */
  0xBF, /*  63 [?] */
};

TRMatrix ROTX_90( 1, 0, 0,  0, 0,-1,   0, 1, 0);
TRMatrix ROTY_90( 0, 0, 1,  0, 1, 0,  -1, 0, 0);
TRMatrix ROTZ_90( 0,-1, 0,  1, 0, 0,   0, 0, 1);

TRMatrix ROTX_m90( 1, 0, 0,  0, 0, 1,   0,-1, 0);
TRMatrix ROTY_m90( 0, 0,-1,  0, 1, 0,   1, 0, 0);
TRMatrix ROTZ_m90( 0, 1, 0,  1, 0, 0,   0, 0, 1);


/* ======================================================================= */
/* ===                                                                 === */
/* ===                         T R M a t r i x                         === */
/* ===                                                                 === */
/* ======================================================================= */

TRMatrix::TRMatrix(long v11,long v12,long v13,
                   long v21,long v22,long v23,
                   long v31,long v32,long v33)
{
  rm[0][0] = v11;
  rm[0][1] = v12;
  rm[0][2] = v13;

  rm[1][0] = v21;
  rm[1][1] = v22;
  rm[1][2] = v23;

  rm[2][0] = v31;
  rm[2][1] = v32;
  rm[2][2] = v33;
}

// =======================================================================
// ===                                                                 ===
// ===                         T S M o u s e                           ===
// ===                                                                 ===
// =======================================================================

// ------------------------------------
// -          Constructor
// ------------------------------------

TSMouse::TSMouse (const char *dev)
  : CTTYS (dev), rm (1,0,0, 0,1,0, 0,0,1)
{
  SetRaw ();
  SetSpeed (19200);
  SetParity (NONE);
  SetSize (8);
  SetStopBits (2);
  SetCtsRts (1);

  unsigned char buf[128];

  BeepOn = FALSE;
  Rotate = FALSE;

  cout << "Initializing ...."; 
  cout.flush();

  // ** Send an undocumented startup string **
  WriteCommand(mglStartup);
  usleep(1);

  // ** Wait a litle bit **
  cout << "."; cout.flush();

  // ** Set the Mode to 3D Mouse with all axes avaiable **
  WriteCommand(mglModeOn);

  // ** Make some noise to say  we are Initialized **
  WriteCommand(mglBeeper);

  // ** find Syncronisation into the Data stream **
  do
    {
      cout << "."; cout.flush();
      if (Read(buf,1) == -1)
        {
          if (errno == EINTR) continue;
          perror("InitMouse");
          exit(0);
        }
    } while(*buf != '\r');

  // ** Now we are synconised and can use the "ReadCommand" function. **
  cout << "."; cout.flush();

  //
  // reading the first comand. It can be the answer to the mode or 
  // beep command
  //
  ReadCommand(buf);  
  if (buf[0] == 'm') ReadCommand(buf);

  cout << "... done\n";

}


// ------------------------------------
// -           Destructor
// ------------------------------------
TSMouse::~TSMouse()
{

  WriteCommand(mglModeOff); 
  WriteCommand(mglBeeper);

  usleep(100);
}

  
// ---------------------------------------------------------------
// -  unsigned int  TSMouse::Low4(unsigned char Value, int *Error)
// ---------------------------------------------------------------
unsigned int  TSMouse::Low4(unsigned char Value, int *Err)
{
  unsigned char val; 
  
  val = Value & 0x000F; 
  if (NibbleCodes[val] != Value) 
    *Err = TRUE;
  
  return val; 
}

// --------------------------------------------------------------
// -  unsigned int  TSMouse::Low6(unsigned int ch, int *Error )
// --------------------------------------------------------------
long  TSMouse::Low6(long ch, int *Err )
{
  unsigned char val;
  
  if (! *Err)
    {
      val = ch & 0x003F;
      if ( ch != Table6Bits[val] )
        *Err = TRUE;
      return long(val);
    }
  return 0;
}


// ---------------------------------------------------
// -  void TSMouse::Show(unsigned char *buf, int size)
// ---------------------------------------------------
void TSMouse::Show (unsigned char *buf, int size)
{
  if (size > 0)
    {
      cout << "----------- recived:  "<< size <<" byte -----------\n";
      for (int j = 0; j < size; j++)
        {
          if (buf[j] == 13)
            printf("Data: �%i�/%i : �CR/0x%x�\n",(int)buf[j],(int)buf[j]&0xF,
                   buf[j]);
          else
            printf("Data: �%i�/%i : �%c/0x%x�\n",(int)buf[j],(int)buf[j]&0xF,
                   buf[j],buf[j]);
        }
    }
  else
    printf("PANIC how did we get here, buf[0] = %i/0x%x\n", buf[0],buf[0]);
  
}

// --------------------------------------------------------------
// -  int TSMouse::DecodeData(PMousePos Pos, unsigned char *buf)
// --------------------------------------------------------------
long TSMouse::DecodeData(PMousePos Pos, unsigned char *buf)
{
  int Err = FALSE;
  long ChkSum = buf[1] + buf[2] + buf[3] + buf[4]  + buf[5]  + buf[6] + 
                buf[7] + buf[8] + buf[9] + buf[10] + buf[11] + buf[12];
  
  if (ChkSum != (Low6(buf[13],&Err)*64 + Low6(buf[14],&Err)) || Err)
    {
      Pos->tx = 0; Pos->ty = 0; Pos->tz = 0; 
      Pos->rx = 0; Pos->ry = 0; Pos->rz = 0;    
      return -1;
    }


  Pos->tx = Low6(buf[ 1],&Err)*64 + Low6(buf[ 2],&Err) - 2048;
  Pos->ty = Low6(buf[ 3],&Err)*64 + Low6(buf[ 4],&Err) - 2048;
  Pos->tz = Low6(buf[ 5],&Err)*64 + Low6(buf[ 6],&Err) - 2048;
  Pos->rx = Low6(buf[ 7],&Err)*64 + Low6(buf[ 8],&Err) - 2048;
  Pos->ry = Low6(buf[ 9],&Err)*64 + Low6(buf[10],&Err) - 2048;
  Pos->rz = Low6(buf[11],&Err)*64 + Low6(buf[12],&Err) - 2048;
  
 /*   if ( dataxyzabc->x > 100 )
      if ( dataxyzabc->z == 0 )
      {
      dataxyzabc->x = mglLow6(Buffer[ 1])*64 + mglLow6(Buffer[ 2]) - 2048;
      dataxyzabc->y = mglLow6(Buffer[ 3])*64 + mglLow6(Buffer[ 4]) - 2048;
      dataxyzabc->z = mglLow6(Buffer[ 5])*64 + mglLow6(Buffer[ 6]) - 2048;
      dataxyzabc->a = mglLow6(Buffer[ 7])*64 + mglLow6(Buffer[ 8]) - 2048;
      dataxyzabc->b = mglLow6(Buffer[ 9])*64 + mglLow6(Buffer[10]) - 2048;
      dataxyzabc->c = mglLow6(Buffer[11])*64 + mglLow6(Buffer[12]) - 2048;
      };
      */

  if (Err)
    {
      Pos->tx = 0; Pos->ty = 0; Pos->tz = 0; 
      Pos->rx = 0; Pos->ry = 0; Pos->rz = 0;    
      return -2;
    };

  if(Rotate)
    {
      rm.Calc(Pos->tx,Pos->ty,Pos->tz,&(Pos->tx),&(Pos->ty),&(Pos->tz));
      rm.Calc(Pos->rx,Pos->ry,Pos->rz,&(Pos->rx),&(Pos->ry),&(Pos->rz));
    }

  return 0;

}

// -------------------------------------------------
// -  int TSMouse::DecodeKey(unsigned char *buf)
// -------------------------------------------------
unsigned long TSMouse::DecodeKey (unsigned char *buf)
{
  unsigned long rv = 0;
  int Error = 0;

  if (buf[0] != 'k') return 0;

  rv  = (Low4(buf[3],&Error) << 4 | Low4(buf[2],&Error)) << 4 | 
        Low4(buf[1],&Error);
  
  if (Error == TRUE) return 0;
  
  return rv;
}

// -------------------------------------------------
// -  int TSMouse::ReadCommand (unsigned char *buf)
// -------------------------------------------------
int TSMouse::ReadCommand (unsigned char *buf)
{
  int rs;

  do
    {
      errno = 0;
      if ((rs = Read(buf,1)) < 0)
        {
          if (errno == EINTR) 
            {
              cout << "hmmm\n";
              continue;
            }
          perror("ReadCommand");
          return -1;
        }
      else if(rs == 1 && buf[0] == 0x0)
        {
          cout << "Oh ohhh, conection closed by device \n";
          return -666;
        }
    } while (errno == EINTR);

  switch (*buf)
    {
    case 'k': if (Read(buf+1, 4) < 0) return -2;
              if(buf[4] != '\r') return -11;
              if (BeepOn && DecodeKey(buf) != 0x0) WriteCommand(mglBeeper);
              return 5;
    case 'b': if (Read(buf+1, 1) < 0) return -3;
              if(buf[1] != '\r') return -11;
              return 2;
    case 'm': if (Read(buf+1, 2) < 0) return -4;
              if(buf[2] != '\r') return -11;
              return 3;
    case 'd': if (Read(buf+1,15) < 0) return -5;
              if(buf[15] != '\r') return -11;
              return 16;
    case 'p': if (Read(buf+1, 3) < 0) return -6;
              if(buf[3] != '\r') return -11;
              return 4;
    case 'n': if (Read(buf+1, 2) < 0) return -7;
              if(buf[2] != '\r') return -11;
              return 3;
    case 'q': if (Read(buf+1, 3) < 0) return -8;
              if(buf[3] != '\r') return -11;
              return 4;
    case 'z': if (Read(buf+1, 1) < 0) return -9;
              if(buf[1] != '\r') return -11;
              return 2;
    case 'e': if (Read(buf+1, 4) < 0) return -10;
              if(buf[4] != '\r') return -11;
              return 5;
    default: return -12;
    }
}

// ----------------------------------------------------
// -  int TSMouse::SetNullRadius(unsigned char radius)
// ----------------------------------------------------
int TSMouse::SetNullRadius(unsigned char radius)
{
  unsigned char CM[4] = mglNullRadius;
  unsigned char buf[32];

  if (radius > 15)
    return -1;

  CM[1] = mkNibble(radius);

  if (WriteCommand(CM) == -1)    return -2;

  do
    {
      if (ReadCommand(buf) < 0)  return -3;
    } while (buf[0] != 'n');

  if (buf[1] != CM[1]) return -4;
    
  return 0;
}

// ---------------------------------------------------------------------------
// -  int TSMouse::SetDataRate (unsigned char min, unsigned char max)
// ---------------------------------------------------------------------------
int TSMouse::SetDataRate  (unsigned char min, unsigned char max)
{
  unsigned char CM[5] = mglDataRateFast;
  unsigned char buf[32];

  if (min > 15)   return -1;
  if (max > 15)   return -2;

  CM[1] = mkNibble(max);
  CM[2] = mkNibble(min);

  if (WriteCommand(CM) == -1)    return -3;

  do
    {
      if (ReadCommand(buf) < 0)  return -4;
    } while (buf[0] != 'p');

  if (buf[1] != CM[1]) return -5;
  if (buf[2] != CM[2]) return -6;
    
  return 0;

}

// -----------------------------------------------------------------------
// -  int TSMouse::SetSensitivity(unsigned char trans, unsigned char rot)
// -----------------------------------------------------------------------
int TSMouse::SetSensitivity(unsigned char trans, unsigned char rot)
{
  unsigned char CM[5] = mglQuality;
  unsigned char buf[32];
  int rd;

  if (trans > 15)   return -1;
  if (rot > 15)     return -2;

  CM[1] = mkNibble(trans);
  CM[2] = mkNibble(rot);

  if (WriteCommand(CM) == -1)    return -3;

  do
    {
      if ((rd=ReadCommand(buf)) < 0)  return -4;
    } while (buf[0] != 'q');

  if (buf[1] != CM[1]) return -5;
  if (buf[2] != CM[2]) return -6;
    
  return 0;

}
