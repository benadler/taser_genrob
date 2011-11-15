//
// infobase.h - part of infobase
// (C) 08/1999-11/2001 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//


#ifndef _INFOBASE_H
#define _INFOBASE_H


#include <stdlib.h>


#include "util/status.h"
#include "sock/streamsock.h"


#include "infobase/infobasedefines.h"


///////////////////////////////////////////////////////////////////////////////

class CINFOBASE
{
  friend class CINFOBASEDIR;

  //
  // a word on entry names: every string that does NOT contain a `/´ or `#´ is
  // considered a legal name. the `/´ is used as a seperator between path
  // components and the `#´ (plus an appended number) is used as a suffix to
  // distinguish files with an otherwise equal filename.
  //
  // this allows a directory to have several entries with the same name, even
  // files and subdirectories with the same name. each call to `mkdir´ or
  // `creat´ returns the real name generated for accessing the entry. if you
  // choose to ignore the real name you must be aware that further references
  // to a file "foo" (which may have been created as "foo#42") may in fact
  // access a file "foo#13" instead. you´ve got to be aware that this may cause
  // confusion if this feature hits you unprepared.
  //

private:

  //
  // not even implemented...
  //
  // hint: always declare a default and a copy constructor, as well as an
  // assignment operator, even if they´re not needed. if you don´t, the
  // compiler will generate default ones on its own, which are unlikely to do
  // what you want. if you do, you´ll get clean errors when linking.
  //

  CINFOBASE &operator= (const CINFOBASE &);

  //
  //
  //

  void Init (void);

  //
  //
  //

  CSTATUS New (const char *fxnName,
	       const char *entryName,
	       const int flags);

protected:

  unsigned int _addr;
  CSTREAMSOCK *_sock;
  //char *_polledEntry;

#if 0

#define INFOBASE_MODE_VERBOSE 0x1
#define INFOBASE_MODE_FATAL 0x2

  int _mode;

#endif

  //
  // readdir()
  //
  // successively reads the contents of the directory opened with one of
  // `opendirTopDown´ or `opendirBottomUp´.
  //
  // possible return values:
  //
  // - a negative error code if the directory has no more entries
  // - the (positive) length of the entry returned in `buf´
  //

  CSTATUS Readdir (unsigned int handle,
		   int *isDirPtr = 0,
		   int *sizePtr = 0,
		   double *timePtr = 0) const;

  CSTATUS Closedir (unsigned int handle) const;

public:

  /////////////////////////////////////////////////////////////////////////////
  //
  // constructors
  //
  /////////////////////////////////////////////////////////////////////////////

  //
  // (default) constructor, takes the `hostname´ of the infobase server (aka
  // daemon) as argument. if no hostname is given the server is searched via
  // broadcast.
  //

  CINFOBASE (const char *hostname = 0x0);

  //
  // copy constructor
  //
  // used to create a new instance of an infobase client from an existing
  // one. this new instance shares the server (this is the reason why it
  // exists, so that you don´t have to pass it along again) but has a
  // different communication channel and can therefore search a different
  // directory than the original one without disturbing it.
  //

  CINFOBASE (const CINFOBASE &orig);

  /////////////////////////////////////////////////////////////////////////////
  //
  // destructor
  //
  // hint: always declare the destructor virtual, even if virtuality is not
  // really needed.
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual ~CINFOBASE (void);

  /////////////////////////////////////////////////////////////////////////////
  //
  // file access functions
  //
  /////////////////////////////////////////////////////////////////////////////

#if 0

  int GetMode (void) const
    {
      return _mode;
    }

  int SetMode (const int mode)
    {
      int ret = _mode;
      _mode = mode;
      return ret;
    }

#endif

  /////////////////////////////////////////////////////////////////////////////
  //
  // file access functions
  //
  /////////////////////////////////////////////////////////////////////////////

  //
  // chdir()
  //
  // let `pathName' be the new directory all paths not starting with a `/' are
  // relative to (default "/").
  //

  CSTATUS Chdir (const char *pathName);

  //
  // mkdir()
  //
  // creates a new directory. note that only the last part of the name is
  // supposed to be the directory to be created, the leading ones are supposed
  // to be already existing directories. if you want to create a deeper
  // directory use several successive calls to `mkdir´ with proper arguments
  // like mkdir("/foo"), mkdir("/foo/bar") and mkdir("/foo/bar/tmp").  the real
  // name (full path) as used to distinguish equally named entries is returned
  // in the buffer `fullPathName´ of maximum length
  // `fullPathNameMaxLength´. note that you´ve got to ensure that this buffer
  // is large enough.
  //
  // possible return values:
  //
  // - a negative error code if the directory could not be created or parts of
  //   its path components do not exist
  // - the (positive) length of the real name as returned in `buf´ otherwise
  //

  CSTATUS Mkdir (const char *pathName,
		 const int flags = 0)
    {
      return New ("CINFOBASE::Mkdir()",
		  pathName,
		  flags | INFOBASE_FLAG_DIR);
    }

  //
  // creat()
  //
  // creates a new file. note that only the last part of the name is supposed
  // to be the file to be created, the leading ones are supposed to be already
  // existing directories.  the real name (full path) as used to distinguish
  // equally named entries is returned in the buffer `fullFileName´ of maximum
  // length `fullFileNameMaxLength´. note that you´ve got to ensure that this
  // buffer is large enough.
  //
  // possible return values:
  //
  // - a negative error code if the file could not be created or parts of its
  //   path components do not exist
  // - the (positive) length of the real name as returned in `buf´ otherwise
  //

  CSTATUS Creat (const char *fileName,
		 const int flags = 0)
    {
      return New ("CINFOBASE::Creat()",
		  fileName,
		  flags);
    }

  //
  // read()
  //
  // read an entry. note that under no circumstances more bytes than the buffer
  // size as given in `bufMaxLength´ are read, so if the buffer is too small
  // for the object you´ve got a problem. use stat() to check for the file
  // length.
  //
  // possible return values:
  //
  // - a negative error code if the entry or some of its path components do not
  //   exist
  // - the (positive) number of bytes read otherwise
  //

  CSTATUS Read (const char *fileName,
		void *buf,
		int bufMaxLength) const;

  //
  // write()
  //
  // write/update/change an entry.
  //
  // possible return values:
  //
  // - a negative error code if the entry or some of its path components do not
  //   exist
  // - the (positive) number of bytes written otherwise
  //

  CSTATUS Write (const char *fileName,
		 const void *buf,
		 int bufLength) const;

  //
  // stat()
  //
  // gives you status information on an entry. status information consist of a
  // `int´ flag whether the entry is a directory, an `int´ size argument and a
  // `double´ timestamp from the last write() access.
  //
  // if `entryName´ is a file, then `size´ is the number of bytes stored in
  // that file. if `entryName´ is a directory, then `size´ is the number of
  // entries stored in that directory.
  //
  // possible return values
  //
  // - a negative error code if the entry or some of its path components do not
  //   exist
  // - something positive otherwise
  //

  CSTATUS Stat (const char *entryName,
		int *isDirPtr = 0,
		int *sizePtr = 0,
		double *timePtr = 0) const;

  //
  // unlink()
  //
  // unlinks (deletes) an entry. if the entry is a directory then all its
  // contents is recursively unlinked too (note that this may have an impact on
  // currently ongoing directory lookups).
  //
  // possible return values:
  //
  // - a negative error code if the entry of parts of its path component do not
  //   exist
  // - something non-negative otherwise
  //

  CSTATUS Unlink (const char *entryName);

  /////////////////////////////////////////////////////////////////////////////
  //
  // directory access functions
  //
  /////////////////////////////////////////////////////////////////////////////

  //
  // opendirTopDown(), opendirBottomUp()
  //
  // open a directory for reading its entries in one iof two modes:
  // `opendirTopDown´ will start with the first entry and then give you the
  // following ones (useful for dumping the tree) and `opendirBottomUp´ will
  // start with the last entry and then give you the preceeding ones (useful
  // for printing entries in reverse order of creation).
  //
  // possible return values:
  //
  // - a negative error code if the directory or parts of its path components
  //   do not exist, or `pathName´ exists but isn´t a directory
  // - something positive otherwise
  //

  CINFOBASEDIR *Opendir (const char *pathName,
			 const int flags = 0) const;

  //
  //
  //

  //
  // may take an optional `entry´ name to enter a special polling mode: in this
  // mode the server actively sends the client the contents of the entry once
  // upon startup and each time when it gets changed by anyone. the only call
  // that the client may legally make is read(). this mode is used by the
  // client if it mustn´t miss a single change of the entry.
  //

#if 0

  int Poll (const char *entryName = 0);
  int Unpoll (void);

#endif

  /////////////////////////////////////////////////////////////////////////////
  //
  // extended functions
  //
  /////////////////////////////////////////////////////////////////////////////


  //
  // you can either:
  //
  // - read a string into a supplied buffer, or
  // - let the infobase allocate the memory to hold the string.
  //
  // in any case the length of the string is returned.
  //

  CSTATUS ReadString (const char *eName, char    *value, int maxSize) const;
  CSTATUS ReadString (const char *eName, char   **value             ) const;

  //
  // you can either:
  //
  // - read a single element (int, float or double) into a variable given by a
  //   pointer (maxSize == 1), or
  // - read a set of ints into a sized array (maxSize != 1), or
  // - let the infobase allocate the memory to hold the array.
  //
  // in any case the number of elements read is returned.
  //

  CSTATUS ReadInt    (const char *eName, int     *value, int maxSize = 1) const;
  // CSTATUS ReadInt    (const char *eName, int    **value                 );
  CSTATUS ReadFloat  (const char *eName, float   *value, int maxSize = 1) const;
  // CSTATUS ReadFloat  (const char *eName, float  **value                 );
  CSTATUS ReadDouble (const char *eName, double  *value, int maxSize = 1) const;
  // CSTATUS ReadDouble (const char *eName, double **value                 );

  //
  // you can only write a single string
  //

  CSTATUS WriteString (const char *eName, const char *value) const;

  //
  // you can either:
  //
  // - write a single element (int, float or double), or
  // - write a sized array of elements.
  //
  // in any case the number of elements written is returned.
  //

  CSTATUS WriteInt    (const char *eName, const int     value              ) const;
  CSTATUS WriteInt    (const char *eName, const int    *value, int size = 1) const;
  CSTATUS WriteFloat  (const char *eName, const float   value              ) const;
  CSTATUS WriteFloat  (const char *eName, const float  *value, int size = 1) const;
  CSTATUS WriteDouble (const char *eName, const double  value              ) const;
  CSTATUS WriteDouble (const char *eName, const double *value, int size = 1) const;
};


//////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CINFOBASEDIR
{
private:

  // not even implemented

  CINFOBASEDIR (void);
  CINFOBASEDIR (const CINFOBASEDIR &);
  CINFOBASEDIR &operator= (const CINFOBASEDIR &);

protected:

  const CINFOBASE *_ib;
  unsigned int _handle;

public:

  CINFOBASEDIR (const CINFOBASE *ib, unsigned int handle)
    {
      _ib = ib;
      _handle = handle;
    }

  virtual ~CINFOBASEDIR (void);

  CSTATUS Readdir (int *isDirPtr = 0,
		   int *sizePtr = 0,
		   double *timePtr = 0)
    {
      return _ib->Readdir (_handle, isDirPtr, sizePtr, timePtr);
    }
};


#endif // _INFOBASE_H
