package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.InternalPathplannerException;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.motion.Motion;
import uhh.fbi.tams.mobilerobot.unit.*;
import genRob.genControl.modules.Slot;

class MotionImpl
    implements Motion
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Nutzungszähler */
  private final Use use;

  /** Log */
  private final ModuleLog out;

  /** verfügbare Protokollbefehle */
  private final Commands commands;

  /** Modul. */
  private final ModuleImpl module;

  /** Fach des Roblets&reg;. */
  private final Slot slot;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Default-Konstruktor
  private MotionImpl ()
  {
    this.use = null;
    this.out = null;
    this.commands = null;
    this.module = null;
    this.slot = null;
  }

  /** Konstruktor. */
  MotionImpl (ModuleLog out,
              Use use,
              ModuleImpl module,
              Slot slot,
              Commands commands)
  {
    this.use = use;
    this.commands = commands;
    this.module = module;
    this.slot = slot;
    this.out = out;
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

  // Motion
  public void turn (final int orientation)
      throws AccessException,
             BrakesOnException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "turn(" + orientation + ")");
    }
    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess();

      commands.turn (orientation);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }

  }

  // Motion
  public void forward (final int distance)
      throws AccessException,
             BrakesOnException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "forward(" + distance + ")");
    }
    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess();

      commands.forward (distance);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Motion
  public void backward (final int distance)
      throws AccessException,
             BrakesOnException,
             ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    if (out.module)
    {
      out.module (this, "backward(" + distance + ")");
    }

    forward ((-1) * distance);
  }


  // Motion
  public boolean isCompleted ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "isCompleted()");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
     return commands.isCompleted ();
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Motion
  public void move (final Pose pose)
      throws AccessException,
             BrakesOnException,
             InternalPathplannerException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "move(" + pose.toString () + ")");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess ();
      commands.move (pose);
      module.setDestination (pose);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }


  // Motion
  public void stop ()
      throws AccessException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "stop()");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess ();
      commands.stop ();
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Motion
  public void rotate (final int orientation)
      throws AccessException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException,
             BrakesOnException
  {
    if (out.module)
    {
      out.module (this, "rotate(" + orientation + ")");
    }
    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess ();
      commands.rotate (orientation);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

  // Motion
  public void translate (final Pose pose)
      throws AccessException,
             UnknownCommandException,
             SocketCommunicationException,
             ProtocolException,
             BrakesOnException
  {
    if (out.module)
    {
      out.module (this, "translate(" + pose.toString () + ")");
    }
    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      assertAccess ();
      commands.translate (pose);
      module.setDestination (pose);
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }


  // Motion
  public void waitForCompleted ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
    if (out.module)
    {
      out.module (this, "waitForCompleted()");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
     commands.waitForCompleted ();
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

}
