#ifndef TRANSTYPE_H
#define TRANSTYPE_H

#define REQUEST_CONECTION   'r'
#define ACCEPT_CONECTION    'a'
#define REFUSE_CONECTION    'p'
#define KEYMASK             'k'
#define MOUSEPOS            'd'
#define CLOSE_CONECTION     'c'
#define STOP_SERVER         's'
#define SERVER_ERROR        'e'

#define RED     'r'
#define BLUE    'b'
#define VIRTUAL 'v'


#ifndef TRUE
#define TRUE	(1==1)
#endif

#ifndef FALSE
#define FALSE	(1==0)
#endif

#define SERVERPORT (IPPORT_USERRESERVED + 815)
#define CLIENTPORT (IPPORT_RESERVED     + 815)

// =======================================================================

struct TMousePos 
{
  long tx; 
  long ty; 
  long tz; 
  long rx;
  long ry; 
  long rz; 
}; 
typedef TMousePos *PMousePos;

// =======================================================================

struct TTranstype
{
  unsigned char Code;       // ** Befehlscodes           **
  TMousePos     Pos;        // ** Mouse Position         **
  unsigned long KeyMask;    // ** Maustaten als Bitmaske **
  unsigned long mn;         // ** Magic Number           **
  unsigned char Robot;      // ** Roboter Name           **
};
typedef TTranstype *PTranstype;

// =======================================================================

#define NOKEY 0x0
#define KEY1  0x1
#define KEY2  0x2
#define KEY3  0x4
#define KEY4  0x8
#define KEY5  0x10
#define KEY6  0x20
#define KEY7  0x40
#define KEY8  0x80
#define KEYX  0x100


#endif
