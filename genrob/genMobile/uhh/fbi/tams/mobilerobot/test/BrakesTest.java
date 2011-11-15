package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;

import genRob.genControl.client.Client;

import uhh.fbi.tams.mobilerobot.unit.Access;
import uhh.fbi.tams.mobilerobot.unit.motion.Brakes;
import org.roblet.Roblet;
import org.roblet.Robot;


/**
 * Testet die Einheit {@link uhh.fbi.tams.mobilerobot.unit.motion.Brakes}
 * des Serviceroboters des AB TAMS aus.
 * Der Roblet&reg;-Server muss auf dem  Roboter auf <b>Port 6002</b> laufen.
 */
class BrakesTest
    implements Roblet, Serializable
{

  /** Startet die Anwendung. */
  public static void main (String[] args)
      throws Exception
  {
    Client client = new Client ();

    try
    {
      System.out.println
          ("Sende Roblet an tams59.informatik.uni-hamburg.de:6002.");

      client.run ("tams59:6002", new BrakesTest ());
    }
    finally
    {
      client.close ();
    }

    System.out.println ("Fertig!");
  }


  /** Liest auf dem Roboter die Batteriespannung aus. */
  public Object execute (Robot robot)
      throws Exception
  {
    // Benötigte Einheiten holen

    Access access = (Access) robot.getUnit (Access.class);

    if (access == null)
      throw new Exception ("Einheit <Access> nicht gefunden.");

    Brakes brakes = (Brakes) robot.getUnit (Brakes.class);

    if (brakes == null)
      throw new Exception ("Einheit <Brakes> nicht gefunden.");

    // Zugriff auf die Steuerung holen

    if (!access.getAccess ())
      throw new Exception
          ("Zugriff auf die Robotersteuerung nicht m\u00f6glich.");

    try
    {
      // Bremsen lösen

      if (!brakes.areBrakesReleased ())
        brakes.releaseBrakes ();

      // Warten...

      Thread.sleep (5000);

      // Bremsen anziehen

      if (brakes.areBrakesReleased ())
        brakes.applyBrakes ();

      return null;
    }
    finally
    {
      // Zugriff auf Robotersteuerung wieder abgeben

      access.releaseAccess ();
    }
  }
}
