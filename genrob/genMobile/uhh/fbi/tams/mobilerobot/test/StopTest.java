package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;

import genRob.genControl.client.Client;
import uhh.fbi.tams.mobilerobot.unit.motion.Brakes;
import uhh.fbi.tams.mobilerobot.unit.motion.Motion;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.*;
import org.roblet.Roblet;
import org.roblet.Robot;

/**
 * Testet die Einheit {@link uhh.fbi.tams.mobilerobot.unit.motion.Motion}
 * des Serviceroboters des AB TAMS und insbesondere die Methode
 * {@link uhh.fbi.tams.mobilerobot.unit.motion.Motion#stop()}.
 * Der Roblet&reg;-Server muss auf dem  Roboter auf <b>Port 6002</b> laufen.
 */
class StopTest
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

      client.run ("tams59:6002", new StopTest ());
    }
    finally
    {
      client.close ();
    }

    System.out.println ("Fertig!");
  }


  /**
   * Führt einige Bewegungen aus, beendet diese aber nach wenigen Sekunden
   * mit dem Stop-Befehl der Einheit
   * {@link uhh.fbi.tams.mobilerobot.unit.motion.Motion}.
   */
  public Object execute (Robot robot)
      throws Exception
  {
    Access access = (Access) robot.getUnit (Access.class);

    if (access == null)
      throw new Exception ("Einheit <Access> nicht gefunden.");

    Brakes brakes = (Brakes) robot.getUnit (Brakes.class);

    if (brakes == null)
      throw new Exception ("Einheit <Brakes> nicht gefunden.");

    Motion motion = (Motion) robot.getUnit (Motion.class);

    if (motion == null)
      throw new Exception ("Einheit <Motion> nicht gefunden.");

    if (!access.getAccess ())
         throw new Exception
          ("Zugriff auf die Robotersteuerung nicht m\u00f6glich.");

    try
    {
      if (!brakes.areBrakesReleased ())
        brakes.releaseBrakes ();

      motion.move (new Pose (0, 0, 0));

      // Warten...
      Thread.sleep (5000);

      motion.stop ();

      // Warten...
      Thread.sleep (2000);

      motion.translate (new Pose (0, 0, 0));

      // Warten...
      Thread.sleep (5000);

      motion.stop ();

      // Warten...
      Thread.sleep (2000);

      motion.rotate (3000);

      // Warten...
      Thread.sleep (5000);

      motion.stop ();

      // Warten...
      Thread.sleep (2000);

      motion.forward (3000000);

      // Warten...
      Thread.sleep (3000);

      motion.stop ();

      if (brakes.areBrakesReleased ())
        brakes.applyBrakes ();

      return null;
    }
    finally
    {
      access.releaseAccess ();
    }
  }
}
