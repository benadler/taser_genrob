//
//
//

#include <string.h>

#include "math/mat.h"
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

CMAT CMAT::SetDiag (const double arg) const
{
  GTL ("CMAT::SetDiag()");

  if ((_cols < 1) ||
      (_rows < 1))
    {
      GTLFATAL (("matrix is empty\n"));
      abort ();
    }

  //

  float *newData = (float *)malloc (_size * sizeof (float));

  if (!newData)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //

  memcpy (newData, _data, _size * sizeof (float));

  int offset = 0;
  int cnt = _cols < _rows ? _cols : _rows;

  while (--cnt >= 0)
    {
      newData[offset++] = arg;
      offset += _cols;
    }

  //

  return CMAT (newData, _rows, _cols);
}


//
//
//

CMAT &CMAT::ClearSelf (void)
{
  memset (_data, 0, _size * sizeof (float));

  return *this;
}


//
//
//

CMAT &CMAT::SetDiagSelf (const double arg)
{
  int offset = 0;
  int cnt = _cols < _rows ? _cols : _rows;

  while (--cnt >= 0)
    {
      _data[offset++] = arg;
      offset += _cols;
    }

  return *this;
}
