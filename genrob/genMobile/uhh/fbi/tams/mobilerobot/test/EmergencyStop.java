package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;

import genRob.genControl.client.Client;
import genRob.genControl.unit.log.Logger;

import uhh.fbi.tams.mobilerobot.unit.Access;
import uhh.fbi.tams.mobilerobot.unit.motion.Brakes;
import uhh.fbi.tams.mobilerobot.unit.motion.Motion;
import org.roblet.Roblet;
import org.roblet.Robot;

/**
 * Holt sich den Zugriff auf die Steuerung des Roboters und h&auml;lt eine
 * laufende Bewegung an. Der Roboter ist nach der Bewegung wieder frei
 * f&uuml;r neue Roblets&reg;, die Bremsen sind angezogen.
 *
 * @author Daniel westhoff
 * @since uhh.fbi.tams.mobilerobot 0.4
 * @version 0.1
 */
class EmergencyStop
    implements Roblet, Serializable
{

   /** Startet die Anwendung. */
   public static void main (String args[])
       throws Exception
   {
     System.out.println ("EMERGENCY STOP!");
     // Initialisierung der Kommunikationsumgebung
     Client client = new Client ();

     // Kontaktaufnahme mit dem Roblet-Server und Ausführung des Roblets
     try
     {
       // Schaffung eines Roblets
       Roblet rRoblet = new EmergencyStop ();

       // Schicken des Roblets, Warten auf Ende dessen Ausführung
       client.run ("tams59:6002", rRoblet);
     }
     finally
     {
       // Abbau der Kommunikationsumgebung
       client.close ();
     }

     System.out.println ("Fertig.");
   }

   /**
   * Die Haupt-Methode des Roblets&reg; - hier findet unsere
   * Aktivität auf dem Roboter (Roblet&reg;-Server) statt.
   */
  // Roblet
  public Object execute (Robot robot)
      throws Exception
  {
    // Logging
    Logger log
        = (Logger) robot.getUnit (Logger.class);

    // Dies sollte nie passieren
    if (log == null)
      throw new Exception ("Einheit <Logger> nicht gefunden.");

    log.log ("<font color=red> EMERGENCY STOP.</font>");

    try
    {
      Access access = (Access) robot.getUnit (Access.class);

      if (access == null)
        throw new Exception ("Einheit <Access> nicht gefunden.");

      Brakes brakes = (Brakes) robot.getUnit (Brakes.class);

      if (brakes == null)
        throw new Exception ("Einheit <Brakes> nicht gefunden.");

      Motion motion
          = (Motion) robot.getUnit (Motion.class);

      if (motion == null)
        throw new Exception ("Einheit <Motion> nicht gefunden.");

      // Zugriff auf Steuerung fordern

      access.forceAccess ();

      try
      {
        motion.stop ();

        log.log ("Bewegung angehalten.");

        return null;
      }
      finally
      {
        brakes.applyBrakes();

        log.log ("Bremsen angezogen.");

        access.releaseAccess ();
      }
    }
    finally
    {
      log.log ("Roblet&reg; beendet.");
    }
  }
 }
