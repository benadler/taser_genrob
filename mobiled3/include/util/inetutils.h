//
//
//


#ifndef _INETUTILS_H
#define _INETUTILS_H


extern char * _gethostname (void);
extern unsigned int _gethostbyname (const char *hostName);
extern unsigned int _gethostaddr (void);
extern unsigned int _getbroadcastaddr (void);
extern char *_inet_n2a (const unsigned int addr);
extern char *_inet_n2a (const unsigned int addr, char *buf, const int bufSize);


#endif
