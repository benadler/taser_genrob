package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.Laserscan;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.sensor.Laserscanner;
import uhh.fbi.tams.mobilerobot.unit.*;

class LaserscannerImpl
    implements Laserscanner
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
  private LaserscannerImpl ()
  {
    this.use = null;
    this.out = null;
    this.commands = null;
  }

  /** Konstruktor. */
  LaserscannerImpl (Use use,
                    Commands commands,
                    ModuleLog out)
  {
    this.use = use;
    this.commands = commands;
    this.out = out;
  }

  // Laserscanner
  public int getNumScanners ()
      throws UnknownCommandException,
      SocketCommunicationException,
      ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getNumScanners()");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return commands.getNumScanners ();
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Laserscanner
  public Laserscan getScanPlatform (int scanner)
      throws UnknownCommandException,
      SocketCommunicationException,
      ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getScanPlatform(" + scanner + ")");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return commands.getScanPlatform (scanner);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Laserscanner
  public Laserscan getScanWorld (int scanner)
      throws UnknownCommandException,
      SocketCommunicationException,
      ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getScanWorld(" + scanner + ")");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return commands.getScanWorld (scanner);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Laserscanner
  public Frame2D getScannerPosition (int scanner)
      throws UnknownCommandException,
      SocketCommunicationException,
      ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getScannerPosition(" + scanner + ")");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return commands.getScannerPosition (scanner);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Laserscanner
  public int[] getScanRadial (int scanner)
      throws UnknownCommandException,
      SocketCommunicationException,
      ProtocolException
  {
    if (out.module)
    {
      out.module (this, "getscanRadial (" + scanner + ")");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return commands.getScanRadial (scanner);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Laserscanner
  public Laserscan getScanScanner (int scanner)
      throws SocketCommunicationException, ProtocolException,
      UnknownCommandException
  {
    if (out.module)
    {
      out.module (this, "getscanRadial (" + scanner + ")");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return commands.getScanScanner (scanner);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }
}
