package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.motion.Brakes;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.AccessException;
import genRob.genControl.modules.Slot;

class BrakesImpl
    implements Brakes
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

  /** Fach des Roblets&reg;. */
  private final Slot slot;

  /** Implementierung des Modules. */
  private ModuleImpl module;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Default-Konstruktor
  private BrakesImpl ()
  {
    this.use = null;
    this.out = null;
    this.module = null;
    this.slot = null;
    this.commands = null;
  }

  /** Konstruktor. */
  BrakesImpl (ModuleLog out,
              Use use,
              ModuleImpl module,
              Slot slot,
              Commands commands)
  {
    this.out = out;
    this.use = use;
    this.module = module;
    this.slot = slot;
    this.commands = commands;
  }

  // Brakes
  public void applyBrakes ()
      throws AccessException,
             SocketCommunicationException,
             ProtocolException,
             UnknownCommandException
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "applyBrakes()");

      if (module.getSlotOfControllingRoblet() == this.slot)
      {
        if (!commands.isCompleted ())
        {
          commands.stop ();
        }

        commands.applyBrakes ();
      }
      else
      {
        throw new AccessException
            ("Zugriff auf die Robotersteuerung verweigert.");
      }
    }
    finally
    {
      use.lower ();
    }
  }

  // Brakes
  public void releaseBrakes ()
      throws AccessException,
             SocketCommunicationException,
             ProtocolException,
             UnknownCommandException
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "releaseBrakes()");

      if (module.getSlotOfControllingRoblet() == this.slot)
      {
        commands.releaseBrakes ();
      }
      else
      {
        throw new AccessException
            ("Zugriff auf die Robotersteuerung verweigert.");
      }
    }
    finally
    {
      use.lower ();
    }
  }

  // Brakes
  public boolean areBrakesReleased ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "areBrakesReleased()");

      return commands.areBrakesReleased ();
    }
    finally
    {
      use.lower ();
    }
  }

}
