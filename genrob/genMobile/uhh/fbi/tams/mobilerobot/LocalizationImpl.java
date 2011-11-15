package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.localization.Localization;

class LocalizationImpl
    implements Localization
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Nutzungszähler */
  private final Use use;

  /** verfügbare Protokollbefehle */
  private final Commands commands;

  /** Logging. */
  private final ModuleLog out;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Default-Konstruktor
  private LocalizationImpl ()
  {
    this.use = null;
    this.out = null;
    this.commands = null;
  }

  /** Konstruktor. */
  LocalizationImpl (Use use, Commands commands, ModuleLog out)
  {
    this.use = use;
    this.commands = commands;
    this.out = out;
  }


  // Localization
  public Pose getPose ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getPose()");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return commands.getPosition ();
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

}
