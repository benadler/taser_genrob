//
//
//


#include <iostream.h>


///////////////////////////////////////////////////////////////////////////////

class CSIMPLELISTITEM
{
private:

  CSIMPLELISTITEM (const CSIMPLELISTITEM &);
  CSIMPLELISTITEM &operator= (const CSIMPLELISTITEM &);

protected:

  CSIMPLELISTITEM *_prev, *_next;

  CSIMPLELISTITEM (void)
    {
      cout << __PRETTY_FUNCTION__ << endl;
      _prev = _next = 0x0;
    }

public:

  virtual ~CSIMPLELISTITEM (void)
    {
      cout << __PRETTY_FUNCTION__ << endl;
    }
};


class CSIMPLELIST
{
private:
protected:
public:
  void Append (void)
    {
      cout << __PRETTY_FUNCTION__ << endl;
    }
  void Remove (void)
    {
      cout << __PRETTY_FUNCTION__ << endl;
    }
};


//
// und wie sieht das ganze auf serverseite aus? jede CCONNECTION hat eine liste
// von offenen directories und offenen files?
//

///////////////////////////////////////////////////////////////////////////////

class CIBDIR;
class CIBFILE;


class CIB
{
  friend class CIBDIR;
  friend class CIBFILE;

private:
protected:
  CSIMPLELIST _dirs;
  CSIMPLELIST _files;
public:

  CIB (void)
    {
      cout << __PRETTY_FUNCTION__ << endl;
    }

  virtual ~CIB (void)
    {
      cout << __PRETTY_FUNCTION__ << endl;
    }

  CIBDIR *Opendir (const char *name);
  CIBFILE *Open (const char *name);
};


///////////////////////////////////////////////////////////////////////////////

class CIBDIR : public CSIMPLELISTITEM
{
  friend class CIB;

private:

  CIBDIR (void);
  CIBDIR (const CIBDIR &);
  CIBDIR &operator= (const CIBDIR &);

protected:

  CIB *_ib;

  CIBDIR (CIB *ib)
    {
      cout << __PRETTY_FUNCTION__ << endl;
      _ib = ib;
      _ib->_dirs.Append ();
    }

public:

  virtual ~CIBDIR (void)
    {
      cout << __PRETTY_FUNCTION__ << endl;
      _ib->_dirs.Remove ();
    }

  char *Readdir (void)
    {
      return 0x0;
    }
};


CIBDIR *CIB::Opendir (const char *name)
{
  cout << __PRETTY_FUNCTION__ << endl;
  return new CIBDIR (this);
}


///////////////////////////////////////////////////////////////////////////////

class CIBFILE : public CSIMPLELISTITEM
{
  friend class CIB;

private:

protected:

  CIB *_ib;

  CIBFILE(CIB *ib)
    {
      cout << __PRETTY_FUNCTION__ << endl;
      _ib = ib;
      _ib->_files.Append ();
    }

public:
};


CIBFILE *CIB::Open (const char *name)
{
  cout << __PRETTY_FUNCTION__ << endl;
  return new CIBFILE (this);
}


///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  CIB ib;

  CIBDIR *dir = ib.Opendir ("/");

  char *fName = dir->Readdir ();

  CIBFILE *file = ib.Open (fName);

  delete file;

  delete dir;
}
