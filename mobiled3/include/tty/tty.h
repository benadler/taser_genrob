//
//
//

#ifndef _TTY_H_
#define _TTY_H_


#ifdef _WIN32
  #include <wtypes.h>
  #include <winbase.h>
#else
  #include <termios.h>
  #include <sys/time.h>
#endif


class CTTY
{
private:

  // not even implemented

  CTTY (const CTTY &);
  CTTY &operator= (const CTTY &);

  //

  int _closeMe;

protected:

  typedef int milliseconds_t;

#ifdef _WIN32
  HANDLE _fd;
#else
  int _fd;
#endif

  void Init (void);

  CTTY (void)   // used by CTTYS/CSM
  {
    _closeMe = 1;
  }

#if !defined (_WIN32)

  CTTY (const int fd);   // used by CKBD

#endif


#ifdef _WIN32
  DCB _tFlags;
  DCB _tFlagsSaved;
#else
  struct termios _tFlags;
  struct termios _tFlagsSaved;
#endif

public:

  //

  virtual ~CTTY (void);

  //

  void SaveFlags (void);
  void RestoreFlags (void);

  int SetRaw (void);

  //

  virtual int Read (void *ptr, const int size) const;
  virtual int Write (const void * const ptr, const int size) const;

#if defined (linux)

  int WaitIn (const milliseconds_t timeout) const;
  void FlushInput (void) const;

#endif

  int Ioctl (int cmd, void *ptr);
};

#endif
