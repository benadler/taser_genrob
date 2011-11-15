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
 * Anwendung, die den Roboter TASER des AB TAMS auf die in Metern angegebenen
 * Koordinaten dreht und dannn geradeaus auf diesen Punkt f&auml;hrt. Die
 * Selbst-Lokalisierung des Roboters muss vorher kalibriert sein. Der
 * Roblet&reg;-Server muss auf <b>Port 6002</b> laufen (default).
 *
 * <p><b>Verwendung:</b> <tt>translate.sh x y</tt>
 *
 * @author Daniel Westhoff
 * @since uhh.fbi.tams.mobilerobot 0.7.3
 */
public class Translate
{
  /** Konstruktor. */
  private Translate () {}

  /** Startet die Anwendung. */
  public static void main (String args[])
      throws Exception
  {
    if (args.length != 2)
    {
      System.out.println ("usage: mobileTranslate x y");
      System.exit (-1);
    }

    final double x = Double.parseDouble (args[0]);
    final double y = Double.parseDouble (args[1]);

    System.out.println ("Fahre Roboter zu (" + x + " m, " + y + " m).");

    // Initialisierung der Kommunikationsumgebung
    Client client = new Client ();

    // Kontaktaufnahme mit dem Roblet-Server und Ausführung des Roblets
    try
    {
      // Schicken des Roblets, Warten auf Ende dessen Ausführung
      Object o = client.run ("tams59:6002", new TranslateRoblet (x, y));

      System.out.println ("Pose: " + (Pose) o);
    }
    finally
    {
      // Abbau der Kommunikationsumgebung
      client.close ();
    }

    System.out.println ("Fertig.");
  }

  ////////////////////////////////////////////////////////////////////////

  private static class TranslateRoblet
      implements Roblet, Serializable
  {

    private final double X;
    private final double Y;

    /** Konstruktor. */
    private TranslateRoblet (final double x, final double y)
    {
      this.X = x;
      this.Y = y;
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
        {
          throw new Exception
              ("Zugriff auf die Robotersteuerung nicht m\u00f6glich.");
        }

        Localization loc
            = (Localization) robot.getUnit (Localization.class);

        if (loc == null)
        {
          throw new Exception ("Einheit <Localization> nicht gefunden!");
        }

        log.log ("Einheit <i>Localization</i> gefunden.");

        try
        {
          if (!brakes.areBrakesReleased ())
          {
            brakes.releaseBrakes ();

            log.log ("Bremsen gelöst.");
          }

          Pose pose = loc.getPose ();

          double ae = Math.atan2 (Y - pose.y, X - pose.x);

          motion.translate (new Pose ((int) (X * 1000.0),
                                      (int) (Y * 1000.0),
                                      Robiant.dr2rr (ae)));

          log.log ("Fahre zu (" + X + " m, " + Y + " m).");

          log.log ("Warte auf das Ende der Bewegung.");

          motion.waitForCompleted ();

          log.log ("Bewegung beendet.");

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
