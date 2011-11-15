package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.sensor.Battery;

class BatteryImpl
    implements Battery
{
  private static final double LOW_VOLTAGE = 47.0d;
  private static final double MIN_VOLTAGE = 46.0d;

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
  private BatteryImpl ()
  {
    this.use = null;
    this.out = null;
    this.commands = null;
  }

  /** Konstruktor. */
  BatteryImpl (ModuleLog out, Use use, Commands commands)
  {
    this.out = out;
    this.use = use;
    this.commands = commands;
  }

  // Battery
  public double getVoltage ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "getVoltage ()");

      return commands.getBatteryVoltage ();
    }
    finally
    {
      use.lower ();
    }
  }

  // Battery
  public double getLowVoltage ()
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "getLowVoltage ()");

      return LOW_VOLTAGE;
    }
    finally
    {
      use.lower ();
    }
  }

  // Battery
  public double getMinVoltage ()
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "getMinVoltage ()");

      return MIN_VOLTAGE;
    }
    finally
    {
      use.lower ();
    }
  }
}
