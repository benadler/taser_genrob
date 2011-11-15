package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;

import org.roblet.Roblet;
import org.roblet.Robot;
import genRob.genControl.client.Client;
import genRob.genControl.unit.log.Logger;
import uhh.fbi.tams.mobilerobot.unit.Access;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.localization.Localization;
import uhh.fbi.tams.mobilerobot.unit.motion.Brakes;
import uhh.fbi.tams.mobilerobot.unit.motion.Motion;
import uhh.fbi.tams.mobilerobot.unit.Robiant;


/**
 * Bewegt den Roboter ein wenig mit den Befehlen <tt>forward</tt>,
 * <tt>backward</tt> und <tt>turn</tt>,
 * <font color="red">die Kollisionsvermeidung ist dabei ausgeschaltet.</font>
 */
class CollisionAvoidenceOffTest
    implements Roblet, Serializable
{

  /** Startet die Anwendung. */
  public static void main (String args[])
      throws Exception
  {
    System.out.println ("ACHTUNG: Keine Kollisionsvermeidung!");
//    System.out.println ("         Weiter mit <RETURN>.");
//    System.in.read ();

    // Initialisierung der Kommunikationsumgebung
    Client client = new Client ();

    // Kontaktaufnahme mit dem Roblet-Server und Ausführung des Roblets
    try
    {
      // Schaffung eines Roblets
      Roblet rRoblet = new CollisionAvoidenceOffTest ();

      // Schicken des Roblets, Warten auf Ende dessen Ausführung
      Object o = client.run ("tams59:6002", rRoblet);
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
   {
     throw new Exception ("Einheit <Logger> nicht gefunden.");
   }

   log.log ("Roblet&reg; gestartet.");

   try
   {
     Access access = (Access) robot.getUnit (Access.class);

     if (access == null)
     {
       throw new Exception ("Einheit <Access> nicht gefunden.");
     }

     log.log ("Einheit <i>Access</i> gefunden.");

     Brakes brakes = (Brakes) robot.getUnit (Brakes.class);

     if (brakes == null)
     {
       throw new Exception ("Einheit <Brakes> nicht gefunden.");
     }

     Motion motion
         = (Motion) robot.getUnit (Motion.class);

     if (motion == null)
     {
       throw new Exception ("Einheit <Motion> nicht gefunden.");
     }

     log.log ("Einheit <i>Motion</i> gefunden.");

     if (!access.getAccess ())
         throw new Exception
             ("Zugriff auf die Robotersteuerung nicht m\u00f6glich.");

     try
     {
       if (!brakes.areBrakesReleased())
       {
         brakes.releaseBrakes();

         log.log ("Bremsen gelöst.");
       }

       motion.forward (200);

       log.log ("Fahre 20cm vorw&auml;rts.");

       log.log ("Warte auf das Ende der Bewegung.");

       motion.waitForCompleted ();

       log.log ("Bewegung beendet.");

       motion.backward (200);

       log.log ("Fahre 20cm r&uuml;ckw&auml;rts.");

       log.log ("Warte auf das Ende der Bewegung.");

       motion.waitForCompleted ();

       log.log ("Bewegung beendet.");

       motion.rotate (Robiant.degree2robiant (90.0));

       log.log ("Drehe auf 90 Grad.");

       log.log ("Warte auf das Ende der Bewegung.");

       motion.waitForCompleted ();

       log.log ("Bewegung beendet.");

       motion.turn (Robiant.degree2robiant (0.0));

       log.log ("Drehe auf 0 Grad.");

       log.log ("Warte auf das Ende der Bewegung.");

       motion.waitForCompleted ();

       log.log ("Bewegung beendet.");

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
