package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.motion.Velocity;
import uhh.fbi.tams.mobilerobot.unit.AccessException;
import genRob.genControl.modules.Slot;

class VelocityImpl
    implements Velocity
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Nutzungszähler */
  private Use use;

  /** verfügbare Protokollbefehle */
  private Commands commands;

  /** Logging. */
  private ModuleLog out;

  /** Modul. */
  private final ModuleImpl module;

  /** Fach des Roblets&reg;. */
  private final Slot slot;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Default-Konstruktor
  private VelocityImpl ()
  {
    this.out = null;
    this.use = null;;
    this.commands = null;
    this.module = null;
    this.slot = null;
  }

  /** Konstruktor. */
  VelocityImpl (ModuleLog out,
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

  /** Testet, ob Zugriff auf die Steuerung m&ouml;glich ist. */
  private void assertAccess ()
      throws AccessException
  {
    if (module.getSlotOfControllingRoblet() != this.slot)
    {
      throw new AccessException
          ("Zugriff auf die Robotersteuerung verweigert.");
    }
  }

  // Velocity
  public float getTranslationScale ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getTranslationscale()");
    }
    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      // Skalierung der Geschwindigkeiten holen
      float[] scales = commands.getScale ();

      // Skalierung der Translationsgeschwindigkeit zurückliefern
      return scales[0];
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }


  // Velocity
  public float getRotationScale ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this,"getRotationScale()");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      // Skalierung der Geschwindigkeiten holen
      float[] scales = commands.getScale ();

      // Skalierung der Rotationsgeschwindigkeit zurückliefern
      return scales[1];
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }


  // Velocity
  public void setTranslationScale (float scale)
      throws AccessException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "setTranslationScale(" + scale + ")");
    }
    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess ();

      // Skalierung der Geschwindigkeiten holen
      float[] scales = commands.getScale ();

      // neue Skalierung für Translation setzen
      commands.setScale (scale, scales[1]);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }


  // Velocity
  public void setRotationScale (float scale)
      throws AccessException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "setRotationScale (" + scale + ")");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess ();

      // Skalierung der Geschwindigkeiten holen
      float[] scales = commands.getScale ();

      // neue Skalierung für Translation setzen
      commands.setScale (scales[0], scale);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }


  // Velocity
  public void modifyTranslationScale (float changeScale)
      throws AccessException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "modifyTranslationScale(" + changeScale + ")");
    }
    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess ();

      // modifizieren der Skalierung für die Translationsgeschwindigkeit
      commands.modifyScale (changeScale, 1.0f);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }


  // Velocity
  public void modifyRotationScale (float changeScale)
      throws AccessException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "modifyRotationscale(" + changeScale + ")");
    }
    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess ();

      // modifizieren der Skalierung für die Rotationsgeschwindigkeit
      commands.modifyScale (1.0f, changeScale);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

}
