package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.Version;

class VersionImpl
  implements Version
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Logging. */
  private final ModuleLog out;

  /** Nutzungszähler. */
  private final Use use;

  /** Versionsnummer. */
  private final int major, minor, patch;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Default-Konstruktor
  private VersionImpl ()
  {
    this.use = null;
    this.out = null;
    this.major = 0;
    this.minor = 0;
    this.patch = 0;
  }

  /** Konstruktor. */
  VersionImpl (ModuleLog out, Use use, int major, int minor, int patch)
  {
    this.out = out;
    this.use = use;
    this.major = major;
    this.minor = minor;
    this.patch = patch;
  }

  // Version
  public int getMajor ()
  {
    use.raise ();

    try
    {
      return major;
    }
    finally
    {
      use.lower ();
    }
  }

  // Version
  public int getMinor ()
  {
    use.raise ();

    try
    {
      return minor;
    }
    finally
    {
      use.lower ();
    }
  }

  // Version
  public int getPatch ()
  {
    use.raise ();

    try
    {
      return patch;
    }
    finally
    {
      use.lower ();
    }
  }
}
