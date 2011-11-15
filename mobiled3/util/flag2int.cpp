//
//
//

#include <stdlib.h>

#include "util/flag2int.h"
#include "util/strfxns.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
//
//

int flag2int (const struct flag *flags, char *s)
{
  int ret = 0;

  // parse (copy of) string

  char *tok, *save;

  while ((tok = strtok_r (s, ",", &save)))
    {
      s = 0x0;

      const struct flag *flagPtr = flags;

      while (*flagPtr->name)
	{
#if defined (__WATCOMC__)
	  if (!stricmp (flagPtr->name, tok))
#else
	  if (!strcasecmp (flagPtr->name, tok))
#endif
	    {
	      ret |= flagPtr->value;
	      break;
	    }

	  flagPtr++;
	}
    }

  //

  return ret;
}


//
//
//

int flag2int (const struct flag *flags, const char *options)
{
  // make a temporary copy of the string

  const int len = (strlen (options) + 1 ) * sizeof (char);
  char *sCopy = (char *)alloca (len);
  memcpy (sCopy, options, len);

  int ret = 0;

  // parse (copy of) string

  char *tok, *save;

  while ((tok = strtok_r (sCopy, ",", &save)))
    {
      sCopy = 0x0;

      const struct flag *flagPtr = flags;

      while (*flagPtr->name)
	{
#if defined (__WATCOMC__)
	  if (!stricmp (flagPtr->name, tok))
#else
	  if (!strcasecmp (flagPtr->name, tok))
#endif
	    {
	      ret |= flagPtr->value;
	      break;
	    }

	  flagPtr++;
	}
    }

  //

  return ret;
}
