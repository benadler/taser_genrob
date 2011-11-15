package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.Landmark;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.localization.Landmarks;

class LandmarksImpl
    implements Landmarks
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

  // default-Konstruktor
  private LandmarksImpl ()
  {
    this.use = null;
    this.out = null;
    this.commands = null;
  }

  /** Konstruktor. */
  LandmarksImpl (Use use,
                 Commands commands,
                 ModuleLog out)
  {
    this.use = use;
    this.commands = commands;
    this.out = out;
  }

  // Landmarks
  public Landmark[] getAllMarks ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getAllMarks()");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return commands.getAllMarks ();
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Landmarks
  public Landmark[] getAllMarksInitial ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getAllMarksInitial()");
    }
    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return commands.getAllMarksInitial ();
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }
}
