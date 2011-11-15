package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.sensor.DriveTemperature;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;

class DriveTemperatureImpl
    implements DriveTemperature
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Nutzungszähler. */
  private final Use use;

  /** Logging. */
  private final ModuleLog out;

  /** Protokoll-Kommandos. */
  private final Commands commands;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Konstruktor
  private DriveTemperatureImpl ()
  {
    this.use = null;
    this.out = null;
    this.commands = null;
  }

  /** Konstruktor */
  DriveTemperatureImpl (ModuleLog out, Use use, Commands commands)
  {
    this.use = use;
    this.out = out;
    this.commands = commands;
  }

  // DriveTemperature
  public double getLeftTemperature ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getLeftTemperature ()");
    }

    use.raise ();

    try
    {
      return (commands.getDriveTemperatures())[0];
    }
    finally
    {
      use.lower ();
    }
  }

  // DriveTemperature
  public double getRightTemperature ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getRightTemperature ()");
    }

    use.raise ();

    try
    {
      return (commands.getDriveTemperatures())[1];
    }
    finally
    {
      use.lower ();
    }
  }

  // DriveTemperature
  public double[] getTemperatures ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getTemperatures ()");
    }

    use.raise ();

    try
    {

      return commands.getDriveTemperatures ();
    }
    finally
    {
      use.lower ();
    }
  }
}
