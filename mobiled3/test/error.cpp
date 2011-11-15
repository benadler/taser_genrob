//
//
//


#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "error.h"


CERROR::CERROR (const char *inFxn,
		const char *calledFxn = 0x0,
		const int errNo = 0,
		const char *errStr = 0x0)
{
  _inFxn = ::strdup (inFxn);

  if (calledFxn)
    {
      _calledFxn = ::strdup (calledFxn);
    }
  else
    {
      _calledFxn = 0x0;
    }

  _errNo = errNo;

  if (errStr)
    {
      _errStr = ::strdup (errStr);
    }
  else
    {
      _errStr = 0x0;
    }
}


CERROR::CERROR (const CERROR &right)
{
  _inFxn = ::strdup (right._inFxn);

  if (right._calledFxn)
    {
      _calledFxn = ::strdup (right._calledFxn);
    }
  else
    {
      _calledFxn = 0x0;
    }

  _errNo = right._errNo;

  if (right._errStr)
    {
      _errStr = ::strdup (right._errStr);
    }
  else
    {
      _errStr = 0x0;
    }
}


CERROR::~CERROR (void)
{
  if (_errStr)
    {
      ::free (_errStr);
    }

  if (_calledFxn)
    {
      ::free (_calledFxn);
    }

  ::free (_inFxn);
}


void CERROR::Dump (void) const
{
  cout << "ERROR CAUGHT from `" << _inFxn << "'" << endl;

  if (_calledFxn)
    {
      cout << "\tcalling `" << _calledFxn << "' failed" << endl;
    }

  if (_errNo)
    {
      cout << "\terror code: " << _errNo << endl;
    }

  if (_errStr)
    {
      cout << "\terror desc: \"" << _errStr << "\"" << endl;
    }
}


///////////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <stdio.h>


static void deep (void)
{
  static const char * const fxnName = "deep()";

  printf ("deep()\n");

  if (!malloc (12.34567890))
    {
      throw CERROR (fxnName, "malloc()", errno, sys_errlist[errno]);
    }

  if (open ("/foo/bar/baff", 0) < 0)
    {
      throw CERROR (fxnName, "open()", 0, "failed to open");
    }
}


static void flat (void)
{
  static const char * const fxnName = "flat()";

  // neee, das ist doch doof das so zu machen. wenn zwangsweise alles eine
  // exception schmeisst, dann kann ich ja gar nicht mehr die fehlercodes
  // ignorieren. wenn ich mehrere potentiell fehlerausloesende funktionen
  // aufrufen will muss ich es entweder so machen:

  try {deep ();} catch (...) {}
  try {deep ();} catch (...) {}
  try {deep ();} catch (...) {}

  // oder ich habe ein problem. weil, so

  try
    {
      deep ();
      deep ();
      deep ();
    }

  catch (...)
    {
    }

  // geht es nicht. der erste call loest eine exception aus die auch gefangen
  // wird, aber danach wird nicht in den zweiten call gesprungen. statt dessen
  // fliegt er komplett raus aus dem block. war ja auch zu erwarten. ist nur
  // leider nicht was ich will...

  try
    {
      deep ();
    }

  catch (CERROR &e)
    {
      e.Dump ();

      throw CERROR (fxnName, "deep()");
    }
}


int main (void)
{
  static const char * const fxnName = "main()";

  try
    {
      flat ();
    }

  catch (CERROR &e)
    {
      e.Dump ();

      throw CERROR (fxnName);
    }

  return 0;
}
