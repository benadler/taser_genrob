package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.motion.Destination;

class DestinationImpl
    implements Destination
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

  /** Module. */
  private final ModuleImpl moduleImpl;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Default-Konstruktor
  private DestinationImpl ()
  {
    this.out = null;
    this.use = null;
    this.commands = null;
    this.moduleImpl = null;
  }

  /** Konstruktor. */
  DestinationImpl (Use use,
                   Commands commands,
                   ModuleImpl moduleImpl,
                   ModuleLog out)
  {
    this.use = use;
    this.commands = commands;
    this.moduleImpl = moduleImpl;
    this.out = out;
  }

  // Destination
  public Pose getDestination ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getDestination()");
    }

    use.raise ();

    try
    {
      if (commands.isCompleted ())
      {
        return null;
      }
      else
      {
        return moduleImpl.getDestination ();
      }
    }
    finally
    {
      use.lower ();
    }
  }
}
