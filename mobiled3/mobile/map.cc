//
// map.cc
//

// Die folgenden drei Zeilen einkommentieren, wenn Debugging-
// Informationen ausgegeben werden sollen.
#ifdef DEBUG
#undef DEBUG
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thread/tracelog.h"
#include "util/args.h"

#include "./map.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#define LINESINCREMENT 16
#define MARKSINCREMENT 16


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CMAP::~CMAP (void)
{
  GTL ("CMAP::~CMAP()");
}


///////////////////////////////////////////////////////////////////////////////
//
// LoadMap()
//
// entries in the "map.map" file are floating point numbers in meter.
//
///////////////////////////////////////////////////////////////////////////////

void CMAP::LoadMap (const char *fName)
{
  GTL ("CMAP::LoadMap(char*)");
  GTLPRINT (("fName = %s\n", fName));

  //

  FILE *fp = fopen (fName, "r");

  char line[256], *linePtr;

  while ((linePtr = fgets (line, 256, fp)))
    {
      linePtr += strspn (linePtr, " \t");   // skip whitespace
      linePtr [strcspn (linePtr, "\r\n")] = 0;   // cut off linefeeds

      if (!linePtr[0] || (linePtr[0] == '#'))
	{
	  continue;   // skip empty or comment lines
	}

      // allocate more storage, if necessary

      const int idx = _numLines++;

      if (_numLines > _maxLines)
	{
	  _maxLines += LINESINCREMENT;

	  if (!(_line = (SLINE_t*)realloc (_line,
					   _maxLines * sizeof (SLINE_t))))
	    {
	      GTLFATAL (("out of memory\n"));
	      abort ();
	    }
	}

      // add new line

      CARGS args (linePtr);

      _line[idx].x0 = atof (args[0]);
      _line[idx].y0 = atof (args[1]);
      _line[idx].xe = atof (args[2]);
      _line[idx].ye = atof (args[3]);
    }

  fclose (fp);
}


///////////////////////////////////////////////////////////////////////////////
//
// LoadMarks()
//
// entries in the "map.marks" file are integer numbers in millimeter.
//
///////////////////////////////////////////////////////////////////////////////

void CMAP::LoadMarks (const char *fName)
{
  GTL ("CMAP::LoadMarks(char*)");
  GTLPRINT (("fName = %s\n", fName));

  //

  FILE *fp = fopen (fName, "r");

  char line[256], *linePtr;

  while ((linePtr = fgets (line, 256, fp)))
    {
      linePtr += strspn (linePtr, " \t");   // skip whitespace
      linePtr [strcspn (linePtr, "\r\n")] = 0;   // cut off linefeeds

      if (!linePtr[0] || (linePtr[0] == '#'))
	{
	  continue;   // skip empty or comment lines
	}

      // allocate more storage, if necessary

      const int idx = _numMarks++;

      if (_numMarks > _maxMarks)
	{
	  _maxMarks += MARKSINCREMENT;

	  if (!(_mark = (SMARK_t*)realloc (_mark,
					   _maxMarks * sizeof (SMARK_t))))
	    {
	      GTLFATAL (("out of memory\n"));
	      abort ();
	    }
	}

      // add new mark

      CARGS args (linePtr);

      _mark[idx].x = atoi (args[0]) / 1000.0;
      _mark[idx].y = atoi (args[1]) / 1000.0;
    }

  fclose (fp);
}
