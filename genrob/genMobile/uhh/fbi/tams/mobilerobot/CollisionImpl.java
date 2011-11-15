package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.motion.Collision;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;

class CollisionImpl
    implements Collision
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Logging. */
  private final ModuleLog out;

  /** Nutzungzähler. */
  private final Use use;

  /** Protokoll-Kommandos. */
  private final Commands commands;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Default-Konstruktor
  private CollisionImpl ()
  {
    this.out = null;
    this.use = null;
    this.commands = null;
  }

  /** Konstruktor. */
  CollisionImpl (ModuleLog out, Use use, Commands commands)
  {
    this.out = out;
    this.use = use;
    this.commands = commands;
  }

  // Collision
 public boolean isStalled ()
      throws UnknownCommandException,
             SocketCommunicationException,
             ProtocolException
  {
   use.raise ();

   try
   {
     if (out.module) out.module (this, "isStalled ()");

     return commands.isStalled ();
   }
   finally
   {
     use.lower ();
   }
 }

  // Collision
  public int getCollisionRadius ()
  {
    use.raise ();

    try
    {
      if (out.module) out.module (this, "getCollisionRadius ()");

      /** @todo Kollisionsradius ist fest kodiert. */
      return 650;
    }
    finally
    {
      use.lower ();
    }


  }

}
