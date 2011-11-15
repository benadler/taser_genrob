#include <jni.h>
#include <iostream>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
// #include <linux/hiddev.h>
#include <linux/input.h>
#include <linux/ioctl.h>
#include "iowarrior.h"
#include "jiowarrior.h"

static short oval;
static int usb_handle;

//
//
//
void throwException (JNIEnv *env,
                     char* excCls,
                     char* msg)
{
  jclass exception = env->FindClass(excCls);

  if (exception == 0)
    {
      std::cerr << "[C++] libiowarrior.so: "
                << __FILE__
                << "["
                << __LINE__
                << "]: *** ERROR: Unable to find the exception class, give up."
                << std::endl
                << "[C++] libiowarrior.so: "
                << __FILE__
                << "["
                << __LINE__
                << "]: ***        class: "
                << excCls
                << std::endl;

      abort ();
    }

  env->ThrowNew(exception, msg);
}

//
//
//
void write_usb (JNIEnv *env)
{
  if( ioctl( usb_handle, IOW_WRITE, &oval ))
  {
    throwException (env,
                    "uhh/fbi/tams/iowarrior/unit/IOwarriorException",
                    "libiowarrior.so - write_usb(): ioctl-Fehler.");
  }
};

//
// close usb
//
JNIEXPORT void JNICALL Java_uhh_fbi_tams_iowarrior_IOwarriorUSB_close
  (JNIEnv *env, jclass rClass)
{
  //std::cout << "close()" << std::endl;

  oval= -1;

  if( ioctl( usb_handle, IOW_WRITE, &oval ))
  {
    throwException (env,
                    "uhh/fbi/tams/iowarrior/unit/IOwarriorException",
                    "libiowarrior.so - close(): ioctl-Fehler.");

    return;
  };

  close( usb_handle );
}


//
// open usb
//
JNIEXPORT void JNICALL Java_uhh_fbi_tams_iowarrior_IOwarriorUSB_init
  (JNIEnv *env, jclass rClass)
{
  //std::cout << "init ()..." << std::endl;

  usb_handle = -1;

  if (( usb_handle = open( "/dev/iowarrior0", O_RDWR)) < 0 )
  {
    throwException (env,
                    "uhh/fbi/tams/iowarrior/unit/IOwarriorException",
                    "libiowarrior.so - open(): Can't open /dev/iowarrior0.");

    return;
  };

  oval= -1;

  if( ioctl( usb_handle, IOW_WRITE, &oval ))
  {
    throwException (env,
                    "uhh/fbi/tams/iowarrior/unit/IOwarriorException",
                    "libiowarrior.so - open(): ioctl-Fehler.");

    return;
  };
}

//
// set port
//
JNIEXPORT void JNICALL Java_uhh_fbi_tams_iowarrior_IOwarriorUSB_set
  (JNIEnv *env, jclass rClass, jint port, jboolean enabled)
{
  //std::cout << "set()" << std::endl;

  switch (port)
    {
    case 1:
      if (enabled)
        oval = oval & 0xfffe;
      else
        oval = oval | 1;
      break;
   case 2:
     if (enabled)
         oval = oval & 0xfffd;
      else
        oval = oval | 2;
      break;
    case 3:
      if (enabled)
        oval = oval & 0xfffb;
      else
        oval = oval | 4;
      break;
    case 4:
      if (enabled)
        oval = oval & 0xffef;
      else
        oval = oval | 0x10;
      break;
    case 5:
      if (enabled)
        oval = oval & 0xffdf;
      else
        oval = oval | 0x20;
      break;
    case 6:
      if (enabled)
         oval= oval & 0xffbf;
      else
        oval= oval | 0x40;
      break;
    case 7:
      if (enabled)
         oval= oval & 0xff7f;
      else
        oval= oval | 0x80;
      break;
    }

  write_usb(env);
}



