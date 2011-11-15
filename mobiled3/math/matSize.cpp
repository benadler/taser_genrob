//
//
//

#include <math.h>
#include <stdlib.h>
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

CMAT CMAT::AddCols (const int num) const
{
  GTL ("CMAT::AddCols()");

  if ((_cols < 1) ||
      (_rows < 1))
    {
      GTLFATAL (("matrix empty\n"));
      abort();
    }

  const int newCols = _cols + num;

  if (newCols < _cols)
    {
      GTLFATAL (("too many columns\n"));
      abort();
    }

  //

  float *newData = (float *)malloc (newCols * _rows * sizeof (float));

  if (!newData)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //

  const float *src = _data;
  float *dst = newData;

  for (int y=0; y<_rows; y++)
    {
      int x;

      for (x=0; x<_cols; x++)
	{
	  *dst++ = *src++;
	}

      for (; x<newCols; x++)
	{
	  *dst++ = 0.0;
	}
    }

  //

  return CMAT (newData, _rows, newCols);
}


//
//
//

CMAT &CMAT::AddColsSelf (const int num)
{
  GTL ("CMAT::AddColsSelf()");

  if ((_cols < 1) ||
      (_rows < 1))
    {
      GTLFATAL (("matrix empty\n"));
      abort();
    }

  const int newCols = _cols + num;

  if (newCols < _cols)
    {
      GTLFATAL (("too many columns\n"));
      abort();
    }

  //

  float *newData = (float *)malloc (newCols * _rows * sizeof (float));

  if (!newData)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //

  const float *src = _data;
  float *dst = newData;

  for (int y=0; y<_rows; y++)
    {
      int x;

      for (x=0; x<_cols; x++)
	{
	  *dst++ = *src++;
	}

      for (; x<newCols; x++)
	{
	  *dst++ = 0.0;
	}
    }

  //

  CreateSteal (_rows, newCols, newData);

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

CMAT CMAT::AddRows (const int num) const
{
  GTL ("CMAT::AddRows()");

  if ((_cols < 1) ||
      (_rows < 1))
    {
      GTLFATAL (("matrix empty\n"));
      abort();
    }

  const int newRows = _rows + num;

  if (newRows < _rows)
    {
      GTLFATAL (("too many rows\n"));
      abort();
    }

  //

  float *newData = (float *)malloc (_cols * newRows * sizeof (float));

  if (!newData)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //

  memcpy (newData, _data, _size * sizeof (float));
#if 1
  memset (newData + _size, 0, num * _cols * sizeof (float));
#else
  memset (newData + _size * sizeof (float), 0, num * _cols * sizeof (float));
#endif

  //

  return CMAT (newData, newRows, _cols);
}


//
//
//

CMAT &CMAT::AddRowsSelf (const int num)
{
  GTL ("CMAT::AddRowsSelf()");

  if ((_cols < 1) ||
      (_rows < 1))
    {
      GTLFATAL (("matrix empty\n"));
      abort();
    }

  const int newRows = _rows + num;

  if (newRows < _rows)
    {
      GTLFATAL (("too many rows\n"));
      abort();
    }

  //

  _data = (float *)realloc (_data, newRows * _cols * sizeof (float));

  if (!_data)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //

#if 1
  memset (_data + _size, 0, num * _cols * sizeof (float));
#else
  memset (_data + _size * sizeof (float), 0, num * _cols * sizeof (float));
#endif

  //

  _rows = newRows;
  _size = _rows * _cols;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

CMAT CMAT::AddColsRows (const int num) const
{
  GTL ("CMAT::AddColsRows()");

  const int newCols = _cols + num;
  const int newRows = _rows + num;

  if ((newCols < _cols) ||
      (newRows < _rows))
    {
      GTLFATAL (("too many columns or rows\n"));
      abort();
    }

  //

  float *newData = (float *)malloc (newCols * newRows * sizeof (float));

  if (!newData)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //

  const float *src = _data;
  float *dst = newData;
  int y;

  for (y=0; y<_rows; y++)   // if _rows is 0 this will never be touched
    {
      int x;

      for (x=0; x<_cols; x++)
	{
	  *dst++ = *src++;   // if _cols is 0 this will never be touched
	}

      for (; x<newCols; x++)
	{
	  *dst++ = 0.0;
	}
    }

  for (; y<newRows; y++)
    {
      int x;

      for (x=0; x<_cols; x++)
	{
	  *dst++ = *src++;   // if _cols is 0 this will never be touched
	}

      for (; x<newCols; x++)
	{
	  *dst++ = 0.0;
	}
    }

  //

  return CMAT (newData, newRows, newCols);
}


//
//
//

CMAT &CMAT::AddColsRowsSelf (const int num)
{
  GTL ("CMAT::AddColsRowsSelf()");

  const int newCols = _cols + num;
  const int newRows = _rows + num;

  if ((newCols < _cols) ||
      (newRows < _rows))
    {
      GTLFATAL (("too many columns or rows\n"));
      abort();
    }

  //

  float *newData = (float *)malloc (newCols * newRows * sizeof (float));

  if (!newData)
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  //

  const float *src = _data;
  float *dst = newData;
  int y;

  for (y=0; y<_rows; y++)   // if _rows is 0 this will never be touched
    {
      int x;

      for (x=0; x<_cols; x++)
	{
	  *dst++ = *src++;   // if _cols is 0 this will never be touched
	}

      for (; x<newCols; x++)
	{
	  *dst++ = 0.0;
	}
    }

  for (; y<newRows; y++)
    {
      int x;

      for (x=0; x<_cols; x++)
	{
	  *dst++ = *src++;   // if _cols is 0 this will never be touched
	}

      for (; x<newCols; x++)
	{
	  *dst++ = 0.0;
	}
    }

  //

  CreateSteal (newRows, newCols, newData);

  return *this;
}
