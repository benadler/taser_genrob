package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.sensor.OdometryLog;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;

class OdometryLogImpl
    implements OdometryLog
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Logging. */
  private final ModuleLog out;

  /** Nutzungszähler. */
  private final Use use;

  /** Protokoll-Kommandos. */
  private final Commands commands;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Default-Konstruktor
  private OdometryLogImpl ()
  {
    this.out = null;
    this.use = null;
    this.commands = null;
  }

  /** Konstruktor. */
  OdometryLogImpl (ModuleLog out, Use use, Commands commands)
  {
    this.out = out;
    this.use = use;
    this.commands = commands;
  }

  // OdometryLog
  public void startLogging ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "startLogging()");
    }

    use.raise ();

    try
    {
      commands.startOdoLogging ();
    }
    finally
    {
      use.lower ();
    }
  }

  // OdometryLog
  public void stopLogging ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "stopLogging()");
    }

    use.raise ();

    try
    {
      commands.stopOdoLogging ();
    }
    finally
    {
      use.lower ();
    }
  }
}
