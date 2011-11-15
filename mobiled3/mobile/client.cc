//
// client.cc
// (C) 12/2002 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#include <unistd.h>

#ifdef DEBUG
#undef DEBUG
#endif
#include "thread/tracelog.h"

#include "client.h"
#include "protocol.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CCLIENT::CCLIENT (CSTREAMSOCK *sock,
		    CGENBASE *genBase)
  : CCONNECTION (sock)
{
  GTL ("CCLIENT::CCLIENT()");

  _genBase = genBase;

  Start ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

CCLIENT::~CCLIENT (void)
{
  GTL ("CCLIENT::~CCLIENT()");
}


///////////////////////////////////////////////////////////////////////////////
//
// CCLIENT::DataAvailable()
//
// called from spawner (daemon) thread -- must not block!
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::DataAvailable (CDAEMON *daemon)
{
  GTL ("CCLIENT::DataAvailable()");

  _cond.Lock ();

  //

  if (_buffer.ReceiveData ((CSTREAMSOCK *)_sock) < 0)
    {
      GTLPRINT (("connection closed by foreign host\n"));

      daemon->RemoveConnection (this);   // no more events please

      Stop ();   // will self-terminate
    }

  //

  _cond.Signal ();

  _cond.Unlock ();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::Fxn (void)
{
  GTL ("CCLIENT::Fxn()");

  GTLPRINT (("pid=%i\n", getpid ()));

  //

  _cond.Lock ();

  while (42)
    {
      // as long as there is something to be done, do it

      CRCVTELEGRAM *telegram = 0x0;

      try
	{
	  telegram = _buffer.PopTelegram ();
	}

      catch (...)
	{
	  GTLFATAL (("telegram framing failed (protocol error?)\n"));

	  // need to abort the connection to this client. this is best done by
	  // closing the socket here, which leads to that the daemon and the
	  // DataAvailable() function will think that the remote part has
	  // closed the communication and will terminate this thread.

	  ((CSTREAMSOCK *)_sock)->Shutdown ();
	}

      //

      if (telegram)
	{
	  _cond.Unlock ();

	  // at this point we have a legal telegram as far as the basic layout
	  // (framing, crc) is concerned. we're still not sure if it contains
	  // the data necessary for the command it carries or not, so we need
	  // more checks. likewise execution may produce other errors as "out
	  // of memory", ...

	  try
	    {
	      Execute (telegram);
	    }

	  catch (...)
	    {
	      GTLFATAL (("telegram execution failed (protocol error?)\n"));

	      ((CSTREAMSOCK *)_sock)->Shutdown ();
	    }

	  delete telegram;

	  _cond.Lock ();

	  continue;   // try to do more telegrams
	}

      // if nothing more is to be done, check if we have to terminate

      if (_stopRequested)
	{
	  GTLPRINT (("termination requested\n"));
	  break;
	}

      // if not, wait for more data. this will also wake if the remote side
      // closes the connection, so that we don't wait for ever.

      _cond.Wait ();
    }

  //

  _cond.Unlock ();

  // thread will delete() itself upon exit from this function
}


///////////////////////////////////////////////////////////////////////////////
//
// CCLIENT::Execute() -- main telegram execution arbiter
//
// called from CCLIENT::Fxn() in a try/catch environment, so we may throw
// whatever exception we like to indicate violations of the protocol or other
// severe errors.
//
///////////////////////////////////////////////////////////////////////////////

void CCLIENT::Execute (CRCVTELEGRAM *telegram)
{
  GTL ("CCLIENT::Execute()");

  const unsigned int cmd = telegram->PopU32 ();

  switch (cmd)
    {
      // 0x0000 prefix

    case CMD_PING:
#if 0
      telegram->PopU32 ();   // deliberate error! (used for testing)
#endif
      Ping ();
      break;

    case CMD_CHECKPROTOCOL:
      CheckProtocol ();
      break;

    case CMD_DEBUGGING:
      Debugging (telegram);
      break;

      // 0x0001 prefix

    case CMD_GETMODE:
      GetMode ();
      break;

    case CMD_SETMODE:
      SetMode (telegram);
      break;

      // 0x0002 prefix

    case CMD_GETPOSITION:
      GetPosition ();
      break;

    case CMD_GETPOSITIONODO:
      GetPositionOdo ();
      break;

    case CMD_SETPOSITION:
      SetPosition (telegram);
      break;

    case CMD_GETPOSITIONANDVARIANCE:
      GetPositionAndVariance ();
      break;

    case CMD_GETALLMARKS:
      GetAllMarks ();
      break;

    case CMD_GETALLMARKSINITIAL:
      GetAllMarksInitial ();
      break;

    case CMD_GETALLMARKSWITHVARIANCES:
      GetAllMarksWithVariances ();
      break;

    case CMD_GETALLLINES:
      GetAllLines ();
      break;

      // 0x0004 prefix

    case CMD_GETNUMSCANNERS:
      GetNumScanners ();
      break;

    case CMD_GETSCANNERPOSITION:
      GetScannerPosition (telegram);
      break;

    // ++westhoff20050808
    case CMD_GETSCANSCANNER:
      GetScanScanner (telegram);
      break;

    // ++westhoff20050715
    case CMD_GETSCANRADIALSCANNER:
      GetScanRadialScanner (telegram);
      break;

    case CMD_GETSCANPLATFORM:
      GetScanPlatform (telegram);
      break;

    case CMD_GETSCANWORLD:
      GetScanWorld (telegram);
      break;


    // 0x0003 prefix

    case CMD_GETSCALE:
      GetScale ();
      break;

    case CMD_SETSCALE:
      SetScale (telegram);
      break;

    case CMD_MODIFYSCALE:
      ModifyScale (telegram);
      break;

    case CMD_STOP:
      StopMotion ();
      break;

    case CMD_TRANSLATE:
      Translate (telegram);
      break;

    case CMD_FORWARD:
      Forward (telegram);
      break;

    case CMD_ROTATEANGLE:
      RotateAngle (telegram);
      break;

      // --westhoff20060905
      //    case CMD_ROTATEPOINT:
      //      RotatePoint (telegram);
      //      break;

    // ++westhoff20060421
    case CMD_TURNANGLE:
      TurnAngle (telegram);
      break;

    case CMD_MOVE:
      Move (telegram);
      break;

      // --westhoff20060905
      //    case CMD_MOVEPOINT:
      //      MovePoint (telegram);
      //      break;

    case CMD_ISCOMPLETED:
      IsCompleted ();
      break;

    case CMD_WAITFORCOMPLETED:
      WaitForCompleted ();
      break;

    // ++westhoff20050506
    case CMD_STALLED:
      Stalled ();
      break;

    // ++westhoff20050803
    case CMD_APPLYBRAKES:
      ApplyBrakes ();
      break;

    // ++westhoff20050803
    case CMD_RELEASEBRAKES:
      ReleaseBrakes ();
      break;

    // ++westhoff20050803
    case CMD_AREBRAKESRELEASED:
      AreBrakesReleased ();
      break;

    // ++westhoff20050202: Start-/StopOdoLogging
    // 0x0006 prefix

    case CMD_STARTODOLOGGING:
      StartOdoLogging ();
      break;

    case CMD_STOPODOLOGGING:
      StopOdoLogging ();
      break;

    // ++westhoff20050330: Batterie-Spannung, Antriebstemperaturen
    // 0x0007 prefix

    case CMD_GETBATTERYVOLTAGE:
      GetBatteryVoltage ();
      break;

    case CMD_GETDRIVETEMPERATURES:
      GetDriveTemperatures ();
      break;


    default:
      GTLFATAL (("unknown command 0x%08x\n", cmd));
      throw -1;   // be unforgiving
    }
}
