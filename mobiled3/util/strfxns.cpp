//
//
//

#include "util/strfxns.h"


#if !defined (linux)

char *strtok_r (char *s, const char *delim, char **save)
{
  char *sEnd;

  if (!s)
    {
      s = *save;
    }

  s += strspn (s, delim);

  // `s' now points to the next token or \000 */

  if (!*s)
    {
      return 0x0;
    }

  sEnd = s + strcspn (s, delim);

  /* `tokEnd' now points to the first char after the token, maybe \000 */

  if (*sEnd)
    {
      *sEnd++ = 0;   /* if there is a tail, cut it off */
    }

  *save = sEnd;   /* save start for next call */

  return s;
}

#endif


#ifdef __WATCOMC__

// watcom-c under qnx lacks strsep(). this version of strsep() was taken from
// glibc-2.1.1.

char *strsep (char **stringp, const char *delim)
{
  char *begin, *end;

  begin = *stringp;

  if (begin == NULL)
    {    
      return NULL;
    }

  /* A frequent case is when the delimiter string contains only one character.
     Here we don't need to call the expensive `strpbrk' function and instead
     work using `strchr'.  */

  if ((delim[0] == '\0') || (delim[1] == '\0'))
    {
      char ch = delim[0];

      if (ch == '\0')
	{
	  end = NULL;
	}
      else
	{
	  if (*begin == ch)
	    {
	      end = begin;
	    }
	  else
	    {
	      end = strchr (begin + 1, ch);
	    }
	}
    }
  else
    {
      /* Find the end of the token.  */
      end = strpbrk (begin, delim);
    }

  if (end)
    {
      /* Terminate the token and set *STRINGP past NUL character.  */
      *end++ = '\0';
      *stringp = end;
    }
  else
    {
      /* No more delimiters; this is the last token.  */
      *stringp = NULL;
    }

  return begin;
}


#endif
