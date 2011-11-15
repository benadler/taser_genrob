package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.localization.Calibration;
import genRob.genControl.modules.Slot;
import uhh.fbi.tams.mobilerobot.unit.AccessException;

class CalibrationImpl
    implements Calibration
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

  /** Fach des Roblets&reg;. */
  private final Slot slot;

  /** Implementierung des Modules. */
  private ModuleImpl module;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Konstruktor
  private CalibrationImpl ()
  {
    this.use = null;
    this.out = null;
    this.commands = null;
    this.slot = null;
    this.module = null;
  }

  /** Konstruktor */
  CalibrationImpl (ModuleLog out,
                   Use use,
                   ModuleImpl module,
                   Slot slot,
                   Commands commands)
  {
    this.use = use;
    this.commands = commands;
    this.out = out;
    this.module = module;
    this.slot = slot;
  }


  // Calibration
  public void setPose (Pose pose)
      throws AccessException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "setPose()");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      if (module.getSlotOfControllingRoblet() == this.slot)
      {
        commands.setPosition (pose);
      }
      else
      {
        throw new AccessException
            ("Zugriff auf die Robotersteuerung verweigert.");
      }
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }

  }
}
