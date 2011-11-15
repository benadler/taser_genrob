//
// args.cpp
//
// this isn't MT safe! -- but QNX (4.25) doesn't have no MUTEXes
//

#include <stdio.h>
#include <stdlib.h>

#include "util/strfxns.h"

#ifdef _WIN32
  #include <malloc.h>
  #define alloca _alloca
#endif

#ifdef __WATCOMC__
  #include <unix.h>   // snprintf()
#endif

#include "util/args.h"

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if 0

//
//
//

static int strtok_idx (char **ptr, const char *delim)
{
  *ptr += strspn (*ptr, delim);

  return strcspn (*ptr, delim);
}


static int strtok_idx (const char **ptr, const char *delim)
{
  *ptr += strspn (*ptr, delim);

  return strcspn (*ptr, delim);
}


//
//
//

static char *strndup (const char *s, const int n)
{
  char *ret = (char *)malloc (n + 1);

  if (!ret)
    {
      return 0x0;
    }

  memcpy (ret, s, n);

  ret[n] = 0;

  return ret;
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CARGS::Create (const char *s, const char *delim)
{
  GTL ("CARGS::Create()");
  GTLPRINT (("s = \"%s\"\n", s));

#if 0

  const int delimLen = strlen (delim);

  for (int i=0; i<delimLen; i++)
    {
      GTLPRINT (("delimiter = 0x%02x\n", delim[i]));
    }

#endif

  _maxArgs = 0;
  _numArgs = 0;
  _offset = 0;

  _s = 0x0;
  _arg = 0x0;

  _individuallyMalloced = 0;

  if (!s)
    {
      return;   // nothing to be done
    }

  // make a copy of the string

  if (!(_s = strdup (s)))
    {
      GTLFATAL (("out of memory\n"));
      throw -42;
    }

  // parse (copy of) string, which will "destroy" it because of `strtok()'

  char *tok, *save, *tmp = _s;

  do
    {
      tok = strtok_r (tmp, delim, &save);   // 0x0 if no more tokens found

      tmp = 0x0;   // see `man strtok'

      const unsigned int idx = _numArgs++;

      if (_numArgs > _maxArgs)
	{
	  _maxArgs += 16;

	  if (!(_arg = (char **)realloc (_arg, _maxArgs * sizeof (char *))))
	    {
	      GTLFATAL (("out of memory\n"));
	      throw -42;
	    }
	}

      _arg[idx] = tok;
    }
  while (tok);

  //

  _numArgs--;   // off-by-one (the trailing 0x0)
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CARGS::CARGS (void)
{
  GTL ("CARGS::CARGS()");

  _maxArgs = _numArgs = _offset = 0;

  _s = 0x0;
  _arg = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
// parse and split arguments into separate strings
//
///////////////////////////////////////////////////////////////////////////////

CARGS::CARGS (const char *s)
{
  GTL ("CARGS::CARGS(char*)");

  Create (s, "\r\n\t ");
}


///////////////////////////////////////////////////////////////////////////////
//
// parse and split arguments into separate strings
//
///////////////////////////////////////////////////////////////////////////////

CARGS::CARGS (const char *s, const char *delim)
{
  GTL ("CARGS::CARGS(char*,char*)");

  Create (s, delim);
}


///////////////////////////////////////////////////////////////////////////////
//
// parse and split arguments into separate strings, but do variable
// substitution
//
///////////////////////////////////////////////////////////////////////////////

CARGS::CARGS (const char *s, const CARGS &outerArgs)
{
  GTL ("CARGS::CARGS(char*,CARGS&)");

  Create (s, "\r\n\t ");

  for (unsigned int i=0; i<_numArgs; i++)
    {
      if (*_arg[i] == '$')
	{
	  // may have to convert to individually malloc()ed parts

	  if (!_individuallyMalloced)
	    {
	      _individuallyMalloced = 1;

	      for (unsigned int i=0; i<_numArgs; i++)
		{
		  if (!(_arg[i] = strdup (_arg[i])))
		    {
		      GTLFATAL (("out of memory\n"));
		      throw -42;
		    }
		}

	      free (_s);

	      _s = 0x0;
	    }

	  //

	  unsigned int idx = atoi (_arg[i] + 1);

	  free (_arg[i]);

	  if (!(_arg[i] = strdup (outerArgs[idx])))
	    {
	      GTLFATAL (("out of memory\n"));
	      throw -42;
	    }
	}
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CARGS::Clear (void)
{
  GTL ("CARGS::Clear()");

  if (_arg)
    {
      if (_individuallyMalloced)
	{
	  for (unsigned int i=0; i<_numArgs; i++)
	    {
	      free (_arg[i]);
	    }
	}

      free (_arg);
      _arg = 0x0;   // paranoia
    }

  if (_s)
    {
      free (_s);
      _s = 0x0;   // paranoia
    }

  _numArgs = _maxArgs = _offset = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CARGS::~CARGS (void)
{
  GTL ("CARGS::~CARGS()");

  Clear ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if 0

CARGS &CARGS::operator= (const char *s)
{
  GTL ("CARGS::operator=(char*)");

  Clear ();

  //

  Create (s, "\r\n\t ");

  //

  return *this;
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CARGS &CARGS::operator= (const CARGS &right)
{
  GTL ("CARGS::operator=(CARGS&)");

  Clear ();

  //

  _individuallyMalloced = 1;

  _numArgs = right._numArgs - right._offset;
  _maxArgs = _numArgs + 1;

  if (!(_arg = (char **)malloc (_maxArgs * sizeof (char *))))
    {
      GTLFATAL (("out of memory\n"));
      abort ();
    }

  for (int unsigned i=0; i<_numArgs; i++)
    {
      if (!(_arg[i] = strdup (right._arg[right._offset + i])))
	{
	  GTLFATAL (("out of memory\n"));
	  abort ();
	}
    }

  _arg[_numArgs] = 0x0;   // terminating NULL

  //

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

char *CARGS::Dump (void) const
{
  GTL ("CARGS::Dump()");

#ifdef __GNUC__
#warning "this isn't MT safe! -- but QNX (4.25) doesn't have no MUTEXes"
#endif

  static char buf[512];

  char *ptr = buf;
  int remaining = sizeof (buf) - 6;

  *ptr = 0;   // ++itschere20021115: killer! :-(

  for (int unsigned i=_offset; i<_numArgs; i++)
    {
      int size;

      if (i > _offset)
	{
	  size = snprintf (ptr, remaining, " %s", _arg[i]);
	}
      else
	{
	  size = snprintf (ptr, remaining, "%s", _arg[i]);
	}

      ptr += size;
      remaining -= size;

      if (remaining < 1)
	{
	  break;
	}
    }

  if (remaining < 1)
    {
      snprintf (ptr, 6, " (...)");
    }

  return buf;
}


///////////////////////////////////////////////////////////////////////////////
//
// operator[]() is supposed to be used in conjunction with GetNumArgs() so the
// user is supposed to know how many arguments are available. exceeding the
// number of arguments therefore results in an exception.
//
// see how operator*()/operator++() work different in this aspect.
//
///////////////////////////////////////////////////////////////////////////////

char *CARGS::operator[] (const unsigned int idx) const
{
  GTL ("CARGS::operator[]()");

  if (idx >= _numArgs - _offset)
    {
      GTLFATAL (("index %i out of range 0..%i\n",
		 idx,
		 _numArgs-_offset-1));
      throw -42;
    }

  return _arg[idx+_offset];
}


///////////////////////////////////////////////////////////////////////////////
//
// operator*()/operator++() are supposed to be used together in cases where it
// is not a priori known how many arguments exist. they should therefore not
// throw an exception if the list is exceeded, but report that condition by
// other means (return NULL).
//
///////////////////////////////////////////////////////////////////////////////

char *CARGS::operator* (void) const
{
  GTL ("CARGS::operator*()");

  if (_offset >= _numArgs)   // ideally `==' should be enough
    {
      return 0x0;
    }

  return _arg[_offset];
}


CARGS &CARGS::operator++ (void)
{
  GTL ("CARGS::operator++()");

  if (_offset < _numArgs)
    {
      _offset++;
    }

  return *this;
}
