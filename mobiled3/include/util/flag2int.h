//
//
//

#ifndef _FLAG2INT_H_
#define _FLAG2INT_H_

typedef struct flag
{
  char *name;
  int value;
};


extern int flag2int (const struct flag *flags, char *s);
extern int flag2int (const struct flag *flags, const char *s);


#endif
