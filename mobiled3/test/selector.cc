//
//
//


#include <iostream.h>


///////////////////////////////////////////////////////////////////////////////

class _CFD
{
  friend class CSELECTOR;

private:

protected:

  virtual void Action (void *arg) = 0;

public:
};


///////////////////////////////////////////////////////////////////////////////

class CSOCK : public _CFD
{
  friend class CSELECTOR;

private:

protected:

  virtual void Action (void *arg);

public:
};


void CSOCK::Action (void*arg)
{
  cout << __PRETTY_FUNCTION__ << endl;
}


///////////////////////////////////////////////////////////////////////////////

class CSELECTOR
{
private:

  CSOCK *_sock;
  void *_arg;

protected:

public:

  //CSELECTOR (void);
  //virtual ~CSELECTOR (void);

  void AddRead (CSOCK *sock,
		void *arg);
  void Run (void);
};


void CSELECTOR::AddRead (CSOCK *sock, void *arg)
{
  _sock = sock;
  _arg = arg;
}


void CSELECTOR::Run (void)
{
  cout << __PRETTY_FUNCTION__ << endl;
  _sock->Action (_arg);
}


///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  cout << __PRETTY_FUNCTION__ << endl;

  CSOCK sock;
  CSELECTOR sel;

  sel.AddRead (&sock, 0x0);

  sel.Run ();
}
