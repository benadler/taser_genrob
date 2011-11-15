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

/**
 * F&auml;hrt der Roboter an die Pose (12500 <i>mm</i>, 12000 <i>mm</i>,
 * 0 <i>Grad</i>). Die Selbst-Lokalisierung des Roboters muss vorher
 * kalibriert sein. Der Roblet&reg;-Server muss auf <b>Port 6002</b> laufen.
 */
class MobileMove
    implements Roblet, Serializable
{
  /** Zielposition. */
  private final static Pose TARGET = new Pose (12500, 12000, 0);

  /** Startet die Anwendung. */
  public static void main (String args[])
      throws Exception
  {
    System.out.println ("Ziel: " + TARGET);

    // Initialisierung der Kommunikationsumgebung
    Client client = new Client ();

    // Kontaktaufnahme mit dem Roblet-Server und Ausführung des Roblets
    try
    {
      // Schaffung eines Roblets
      Roblet rRoblet = new MobileMove ();

      // Schicken des Roblets, Warten auf Ende dessen Ausführung
      Object o = client.run ("tams59:6002", rRoblet);

      System.out.println ((Pose) o);
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

       motion.move (TARGET);

       log.log ("Fahre zur Pose " + TARGET + ".");

       log.log ("Warte auf das Ende der Bewegung.");

       motion.waitForCompleted ();

       log.log ("Bewegung beendet.");

       Localization loc
           = (Localization) robot.getUnit (Localization.class);

       if (loc == null)
       {
         throw new Exception ("Einheit <Localization> nicht gefunden!");
       }

       log.log ("Einheit <i>Localization</i> gefunden.");

       log.log ("Gebe die aktuelle Pose des Roboters zur&uuml;ck.");

       return loc.getPose ();
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
