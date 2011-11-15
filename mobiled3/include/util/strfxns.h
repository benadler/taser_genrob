//
//
//

#ifndef _STRFXNS_H
#define _STRFXNS_H

#include <string.h>

#if !defined (linux)
extern char *strtok_r (char *s, const char *delim, char **save);
#endif

#if defined (__WATCOMC__)
extern char *strsep (char **stringp, const char *delim);
#define strcasecmp stricmp
#endif

#endif
