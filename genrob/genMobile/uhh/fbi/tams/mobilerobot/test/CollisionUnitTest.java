package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;

import genRob.genControl.client.Client;
import uhh.fbi.tams.mobilerobot.unit.motion.Collision;
import org.roblet.Roblet;
import org.roblet.Robot;

/**
 * Testet die Einheit {@link uhh.fbi.tams.mobilerobot.unit.motion.Collision}
 * auf dem Roboter des AB TAMS. Der Roblet&reg;-Server muss auf
 * <b>Port 6002</b> gestart sein.
 */
class CollisionUnitTest
    implements Roblet, Serializable
{
  /** Roblet&reg;-Server. */
  private static final String server = "tams59:6002";

  /** Startet die Anwendung. */
  public static void main (String[] args)
      throws Exception
  {
    Client client = new Client ();

    System.out.println
        ("Sende Roblet an tams59.informatik.uni-hamburg.de:6002.");

    try
    {
     Roblet roblet = new CollisionUnitTest ();

     Boolean stalled = (Boolean) client.run (server, roblet);

     if (stalled.booleanValue() == true)
     {
       System.out.println ("Roboter blockiert!");
     }
     else
     {
       System.out.println ("Roboter nicht blockiert!");
     }
    }
    finally
    {
      client.close ();
    }

    System.out.println ("Fertig.");
  }

  /** Fragt auf dem Roboter ab, ob dieser blockiert oder frei ist. */
  // Roblet
  public Object execute (Robot robot)
      throws Exception
  {
    Collision collision = (Collision) robot.getUnit (Collision.class);

    if (collision == null)
    {
      throw new Exception ("Einheit <Collision> nicht gefunden.");
    }

    return new Boolean (collision.isStalled());
  }
}
