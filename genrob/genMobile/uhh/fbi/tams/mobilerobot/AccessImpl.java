package uhh.fbi.tams.mobilerobot;

import uhh.fbi.tams.mobilerobot.unit.Access;
import genRob.genControl.modules.Use;
import genRob.genControl.modules.Slot;

class AccessImpl
    implements Access
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Logging. */
  private final ModuleLog out;

  /** Nutzungszähler. */
  private final Use use;

  /** Implementierung der Module-Schnittstelle. */
  private final ModuleImpl module;

  /** Slot des Robblets&reg;, das die Einheit erhalten hat. */
  private final Slot slot;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Default-Konstruktor
  private AccessImpl ()
  {
    this.use = null;
    this.out = null;
    this.module = null;
    this.slot = null;
  }

  /** Konstruktor. */
  AccessImpl (ModuleLog out, Use use, ModuleImpl module, Slot slot)
  {
    this.out = out;
    this.use = use;
    this.module = module;
    this.slot = slot;
  }

  // Access
  public boolean getAccess ()
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "getAccess()");

      // wenn das Roblet schon Zugriff hat...
      if (module.getSlotOfControllingRoblet() == this.slot)
      {
        return true;
      }
      // ...oder kein Roblet den Zugriff hat
      else if (module.getSlotOfControllingRoblet() == null)
      {
        module.setSlotOfControllingRoblet (this.slot);

        return true;
      }
      else
      {
        return false;
      }
    }
    finally
    {
      use.lower ();
    }
  }

  // Access
  public void releaseAccess ()
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "releaseAccess()");

      // Nur, wenn dieses Roblet auch den Zugriff hat !!!
      if (module.getSlotOfControllingRoblet() == this.slot)
      {
        module.setSlotOfControllingRoblet (null);
      }
    }
    finally
    {
      use.lower ();
    }

  }

  // Access
  public void forceAccess ()
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "forceAccess()");

      module.setSlotOfControllingRoblet (this.slot);
    }
    finally
    {
      use.lower ();
    }
  }
}
