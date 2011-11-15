package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;

import org.roblet.Roblet;
import org.roblet.Robot;
import org.roblet.math.Robiant;
import genRob.genControl.client.Client;
import genRob.genControl.unit.log.Logger;
import uhh.fbi.tams.mobilerobot.unit.Access;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.localization.Localization;
import uhh.fbi.tams.mobilerobot.unit.motion.Brakes;
import uhh.fbi.tams.mobilerobot.unit.motion.Motion;

/**
 * Anwendung, die den Roboter TASER des AB TAMS an die Pose (10m,13m), (10m,12m), (14m,12m),
 * (14m,13m) und wieder zur&uuml;ck zum ersten Punkt f&auml;hrt .
 * Die Selbst-Lokalisierung des Roboters muss vorher kalibriert sein.
 * Der Roblet&reg;-Server muss auf <b>Port 6002</b> laufen.
 *
 * <p><b>Verwendung:</b> <tt>driveSquare.sh</tt>
 */
public class DriveSquare
{
  /** Zielposition. */
  // CW (im Uhrzeigersinn)
//  private final static Pose[] TARGET = {new Pose (11000, 10700, Robiant.dd2rr(90.0)),
//                                       new Pose (11000, 14700, Robiant.dd2rr(0.0)),
//                                       new Pose (15000, 14700, Robiant.dd2rr(-90.0)),
//                                       new Pose (15000, 10700, Robiant.dd2rr(180.0))};

// CCW (gegen den Uhrzeigersinn)
  private final static Pose[] TARGET =
      {
      new Pose (15000, 14700, Robiant.dd2rr (180.0)),
      new Pose (11000, 14700, Robiant.dd2rr (-90.0)),
      new Pose (11000, 10700, Robiant.dd2rr (0)),
      new Pose (15000, 10700, Robiant.dd2rr (90.0))
  };

  /** Konstruktor */
  private DriveSquare ()
  {}

  /** Startet die Anwendung. */
  public static void main (String args[])
      throws Exception
  {
    // Initialisierung der Kommunikationsumgebung
    Client client = new Client ();

    // Kontaktaufnahme mit dem Roblet-Server und Ausführung des Roblets
    try
    {
      // Schicken des Roblets, Warten auf Ende dessen Ausführung
      Object o = client.run ("tams59:6002", new DriveSquareRoblet ());

      System.out.println ((Pose) o);
    }
    finally
    {
      // Abbau der Kommunikationsumgebung
      client.close ();
    }

    System.out.println ("Fertig.");
  }

  ///////////////////////////////////////////////////////////////////////

  private static class DriveSquareRoblet
      implements Roblet, Serializable
  {
    /** Konstruktor. */
    private DriveSquareRoblet ()
    {}

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
        {
          throw new Exception
              ("Zugriff auf die Robotersteuerung nicht m\u00f6glich.");
        }

        try
        {
          if (!brakes.areBrakesReleased ())
          {
            brakes.releaseBrakes ();

            log.log ("Bremsen gelöst.");
          }

          for (int i = 0; i < TARGET.length; ++i)
          {
            motion.move (TARGET[i]);

            log.log ("Fahre zur Pose " + TARGET[i] + ".");

            log.log ("Warte auf das Ende der Bewegung.");

            motion.waitForCompleted ();

            log.log ("Bewegung beendet.");
          }

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
          brakes.applyBrakes ();

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
}
