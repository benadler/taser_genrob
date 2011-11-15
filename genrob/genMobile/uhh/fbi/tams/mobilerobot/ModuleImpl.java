package uhh.fbi.tams.mobilerobot;

import java.util.prefs.Preferences;

import org.roblet.Unit;
import genRob.genControl.modules.Registry;
import genRob.genControl.modules.Slot;
import genRob.genControl.modules.Supervisor;
import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.Access;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.Version;
import uhh.fbi.tams.mobilerobot.unit.motion.Brakes;


/**
 * Diese Implementierung eines genRob&reg;-genControl-Moduls realisiert einen
 * Roblet&reg;-Server zur Ansteuerung des mobile Serviceroboters des
 * AB TAMS an der Universit&auml;t Hamburg. Intern kommuniziert dieses
 * Modul &uuml;ber eine Socketverbindung und ein
 * <a href="protokoll.pdf">propriet&auml;res, bin&auml;res Protokoll</a>
 * mit der eigentlichen Robotersteuerung <i>mobiled</i>. Prinzipiell kann
 * dadurch jeder Roboter der dieses (oder einen Teil dieses) Protokoll(s)
 * anbietet &uuml;ber dieses genControl-Modul gesteuert werden.
 *
 * @version 0.8
 * @since uhh.fbi.tams.mobilerobot 0.1
 */
public class ModuleImpl
    implements genRob.genControl.modules.Module
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Version
  private final int major = 0;
  private final int minor = 8;
  private final int patch = 0;

  private final String version    = major + "." + minor + "." + patch;
  private final String moduleName = "uhh.fbi.tams.mobilerobot";
  private final String date       = "(22Aug07)";
  private final String copyright1 = "2002-2003";
  private final String author1    = "Hagen Stanek";
  private final String domain1    = "http://stanek.de, http://genRob.com";
  private final String copyright2 = "2004-2007";
  private final String author2    = "Daniel Westhoff";
  private final String domain2    = "http://tams-www.informatik.uni-hamburg.de";

  /** Logging */
  private ModuleLog out = null;

  /** Nutzungszähler */
  private Use use;

  /** Socketverbindung zur Robotersteuerung mobiled */
  private BinarySocket binarySocket;

  /** verfügbare Protokoll-Kommandos */
  private Commands commands;

  /** Ziel der aktuellen Bewegung. */
  private Pose destination = null;

  /** Automatische Kalibrierung. */
  private AutoCalibration autoCalibration = null;

  /**
   * Slot des Roblets&reg;, das Zugriff auf die Steuerungsmethoden des Roboters
   * hat.
   */
  private Slot slotOfControllingRoblet = null;

  /** System-Präferenzen. */
  private Preferences prefs = Preferences.systemNodeForPackage (getClass());

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // Module
  public void moduleInit (Supervisor supervisor,
                          Use use)
      throws Exception
  {
    // Logging, Modulname, Copyright und Domain
    {
      String strModule = moduleName
                         + " "
                         + version
                         + " "
                         + date;
      String strCopy1 = "(C) Copyright "
                        + copyright1
                        + ", "
                        + author1;
      String strCopy2 = "(C) Copyright "
                        + copyright2
                        + ", "
                        + author2;

      // Ausgabe f�r die Konsole
      System.out.println ("\n"
                          + strModule + "\n"
                          + strCopy1 + "\n"
                          + domain1 + "\n"
                          + strCopy2 + "\n"
                          + domain2 + "\n");
      System.out.flush ();

      // Logging
      out = new ModuleLog (supervisor.getLog ());

      // Ausgabe f�r das Log
      if (out.version)
      {
        out.version (this, "<b>" + strModule + "</b>");
        out.version (this,
                     "<b>" + strCopy1 + ", <font color=blue><u>" + domain1 + "</u></b></font>");
        out.version (this,
                     "<b>" + strCopy2 + ", <font color=blue><u>" + domain2 + "</u></b></font>");
      }
    }

    // Module initialisieren
    {
      if (out.module)
      {
        out.module (this, "moduleInit()");
      }

      // speichere Nutzungszähler
      this.use = use;

      // Parameter laden (default: host=localhost, port=9002)
      // Parameter setzen mit den Propertie-Werten:
      //   genRob.Hamburg.host=...
      //   genRob.Hamburg.port=...
      Parameters parameters = new Parameters ();

      // Socketverbindung aufbauen
      binarySocket = new BinarySocket (parameters.HOST, parameters.PORT, out);

      // Feststellen der verfügbaren Kommandos
      commands = new Commands (binarySocket, out);

      if (out.module)
      {
        out.module (this, "Modul initialisiert.");
      }
    }

    // Roboter kalibrieren
    {
      if (out.module)
      {
        out.module (this, "Lade zuletzt gespeicherte Roboter-Pose.");
      }

//      try
//      {
//        autoCalibration = new AutoCalibration (commands, out);
//
//        // Pose des Roboters abspeichern
//        storePose ();
//      }
//      catch (NegativeArraySizeException ex)
//      {
//        throw new MobileRobotException
//            ("Fehler in der automatischen Kalibrierung. Vielleicht sind die Laserscanner nicht an?", ex);
//      }

      // Pose laden
      loadPose ();

      if (out.module)
      {
        out.module (this, "Roboter kalibriert.");
      }
    }
  }


  // Module
  public void moduleDone ()
  {
    // Pose speichern
    storePose ();

    if (out.calibration)
    {
      out.calibration (this, "Beende &Uuml;berwachung der Pose-Sch&auml;tzung.");

      autoCalibration.stopEstimateThread();
      autoCalibration = null;
    }


    if (out.module)
     {
       out.module (this,
                   "Beende Verbindung zur Robotersteuerung <tt>mobiled</tt>.");
     }

    // Beende Socketverbindung zur Robotersteuerung
    binarySocket.close ();
  }


  //Module
  public Unit getUnit (Class rClass)
  {
    return null;
  }


  // Module
  public Unit getUnit4Slot (Class rClass,
                            Use use,
                            Slot slot)
  {
    Unit unit = null;

    if (out.module)
    {
      out.module (this, "getUnit4Slot (Unit: "
                  + rClass.getName ()
                  + "; Slot: "
                  + slot
                  + ")");
    }

    // Einheit Version
    if (rClass == Version.class)
    {
      unit = new VersionImpl (out, use, major, minor, patch);
    }

    // Einheit Access
    else if (rClass == Access.class)
    {
      unit = new AccessImpl (out, use, this, slot);
    }

    // OdometryLog?
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.sensor.OdometryLog.class)
    {
      unit = new OdometryLogImpl (out, use, commands);
    }

    // Localisation?
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.localization.Localization.class)
    {
      unit = createLocalization ();
    }

    // Calibration?
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.localization.Calibration.class)
    {
      unit = createCalibration (slot);
    }

    // Landmarks?
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.localization.Landmarks.class)
    {
      unit = createLandmarks ();
    }

    // Geometry2D?
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.geometry.Geometry2D.class)
    {
      unit = new Geometry2DImpl (use, out);
    }

    // velocity?
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.motion.Velocity.class)
    {
      unit = createVelocity (slot);
    }

    // laserscanner?
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.sensor.Laserscanner.class)
    {
      unit = createLaserscanner ();
    }

    // motion?
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.motion.Motion.class)
    {
      unit = createMotion (slot);
    }

    // Einheit Brakes
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.motion.Brakes.class)
    {
      unit = createBrakes (slot);
    }

    // Einheit Destination
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.motion.Destination.class)
    {
      unit = new DestinationImpl (use, commands, this, out);
    }

    // Einheit Battery
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.sensor.Battery.class)
    {
      unit = new BatteryImpl (out, use, commands);
    }

    // Einheit DriveTemperature
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.sensor.DriveTemperature.class)
    {
      unit = new DriveTemperatureImpl (out, use, commands);
    }

    // Einheit Collision
    else if (rClass == uhh.fbi.tams.mobilerobot.unit.motion.Collision.class)
    {
      unit = new CollisionImpl (out, use, commands);
    }

    // falls Einheit dem Modul unbekannt,
    // liefere nichts zurück, sonst die Einheit
    if (unit == null)
    {
      if (out.module)
      {
        out.module (this, "getUnit4Slot: No corresponding unit found!");
      }

      return null;
    }
    else
    {
      if (out.module)
      {
        out.module (this,
                    "getUnit4Slot: Returning Unit "
                    + unit.getClass().getName()
                    + "!");
      }

      return unit;
    }
  }


  // Module
  public boolean resetUnit4Slot (Unit unit)
  {
    if (out.module)
    {
      out.module (this,
                  "resetUnit4Slot (Unit: "
                  + unit
                  + ")");
    }

    // Access
    if (unit instanceof AccessImpl)
    {
      // Zugriff auf die Robotersteuerung für andere Roblets freigeben
      ((AccessImpl) unit).releaseAccess ();

      return true;
    }

    // andere Einheiten
    if ((unit instanceof VersionImpl)      ||
        (unit instanceof OdometryLogImpl)  ||
        (unit instanceof LocalizationImpl) ||
        (unit instanceof CalibrationImpl)  ||
        (unit instanceof LandmarksImpl)    ||
        (unit instanceof Geometry2DImpl)   ||
        (unit instanceof VelocityImpl)     ||
        (unit instanceof LaserscannerImpl) ||
        (unit instanceof MotionImpl)       ||
        (unit instanceof Brakes)           ||
        (unit instanceof DestinationImpl)  ||
        (unit instanceof BatteryImpl)      ||
        (unit instanceof DriveTemperatureImpl))
    {
      return true; // Nichts zu erledigen
    }

    // falls Einheit dem Modul unbekannt, liefere FALSE zurück
    return false;
  }


  //////////////////////////////////////////////////////
  // Units für Hamburger Roboter erzeugen
  //////////////////////////////////////////////////////

  /** Testet, ob die &uuml;bergebene Kommandos verf&uuml;gbar sind. */
  private boolean enabled (int[] command)
  {
    for (int i=0; i < command.length; ++i)
    {
      try
      {
        if (!commands.isEnabled (command[i]))
        {
          if (out.module)
          {
            out.module (this,
                        "Befehl " + command[i] + " nicht verf\u00FCgbar.");
          }

          return false;
        }
      }
      catch (UnknownCommandException ex)
      {
        out.module (this, ex);

        return false;
      }
    }

    return true;
  }

  /** erzeugt eine Brakes-Einheit. */
  private BrakesImpl createBrakes (Slot slot)
  {
    if (out.module)
    {
      out.module (this,
                  "Erzeuge Einheit -> Brakes");
    }

    int[] neededCommands = new int[] { Commands.APPLY_BRAKES,
                                       Commands.RELEASE_BRAKES,
                                       Commands.ARE_BRAKES_RELEASED };

    // prüfe, ob die Pose des Roboters abfragbar ist,
    // falls nicht kann davon ausgegangen werden, dass überhaupt keine
    // Lokalisation vorhanden ist
    if (enabled (neededCommands))
    {
      return new BrakesImpl (out, use, this, slot, commands);
    }
    else
    {
      if (out.module)
      {
        out.module (this,
                    "Bremsen nicht abfragbar und setzbar. "
                    + "Keine Einheit erzeugt.");
      }

      return null;
    }

  }

  /** Erzeugt eine Localization-Einheit. */
  private LocalizationImpl createLocalization ()
  {
    if (out.module)
    {
      out.module (this,
                  "Erzeuge Einheit -> Localization");
    }

    int[] neededCommands = new int[] { Commands.GET_POSITION };

    // prüfe, ob die Pose des Roboters abfragbar ist,
    // falls nicht kann davon ausgegangen werden, dass überhaupt keine
    // Lokalisation vorhanden ist
    if (enabled (neededCommands))
    {
      return new LocalizationImpl (use, commands, out);
    }
    else
    {
      if (out.module)
      {
        out.module (this,
                    "Keine Positionsinformation abfragbar und setzbar. "
                    + "Keine Einheit erzeugt.");
      }

      return null;
    }
  }

  /** Erzeugt eine Calibration-Einheit. */
  private CalibrationImpl createCalibration (Slot slot)
  {
    if (out.module)
    {
      out.module (this, "Erzeuge Einheit -> Calibration");
    }

    int[] neededCommands = new int[] { Commands.SET_POSITION };

    if (enabled (neededCommands))
    {
      return new CalibrationImpl (out, use, this, slot, commands);
    }
    else
    {
      if (out.module)
      {
        out.module (this,
                    "Keine Kalibrierung durchf&uuml;hrbar. "
                    + "Keine Einheit erzeugt.");
      }

      return null;
    }
  }

  /** Erzeugt eine Landmark-Einheit. */
  private LandmarksImpl createLandmarks ()
  {
    if (out.module)
    {
      out.module (this, "Erzeuge Einheit -> Landmarks");
    }

    int[] neededCommands = new int[] { Commands.GET_ALL_MARKS,
                                       Commands.GET_ALL_MARKS_INITIAL };

    if (enabled (neededCommands))
    {
      return new LandmarksImpl (use, commands, out);
    }
    else
    {
      if (out.module)
      {
        out.module (this,
                    "Keine Landmarken abfragbar. Keine Einheit erzeugt.");
      }

      return null;
    }
  }

  /** Erzeugt eine Velocity-Einheit. */
  private VelocityImpl createVelocity (Slot slot)
  {
    if (out.module)
    {
      out.module (this, "Erzeuge Einheit -> Velocity");
    }

    int[] neededCommands = new int[] { Commands.GET_SCALE,
                                       Commands.SET_SCALE,
                                       Commands.MODIFY_SCALE };

    if (enabled (neededCommands))
    {
      return new VelocityImpl (out, use, this, slot, commands);
    }
    else
    {
      if (out.module)
      {
        out.module (this,
                    "Geschwindigkeiten nicht änderbar. "
                    + "Keine Einheit erzeugt.");
      }

      return null;
    }
  }

  /** Erzeugt eine Laserscanner-Einheit. */
  private LaserscannerImpl createLaserscanner ()
  {
    if (out.module)
    {
      out.module (this, "Erzeuge Einheit -> Laserscanner");
    }

    int[] neededCommands = new int[] { Commands.GET_NUM_SCANNERS,
                                       Commands.GET_SCAN_PLATFORM,
                                       Commands.GET_SCAN_WORLD,
                                       Commands.GET_SCANNER_POSITION };

    if (enabled (neededCommands))
    {
      return new LaserscannerImpl (use, commands, out);
    }
    else
    {
      if (out.module)
      {
        out.module (this,
                    "Kein Laserscanner abfragbar. "
                    + "Keine Einheit erzeugt.");
      }

      return null;
    }
  }


  /** Erzeugt eine Motion-Einheit. */
  private MotionImpl createMotion (Slot slot)
  {
    if (out.module)
    {
      out.module (this, "Erzeuge Einheit -> Motion");
    }

    int[] neededCommands = new int[] { Commands.MOVE,
                                       Commands.ROTATE_ANGLE,
                                       Commands.TRANSLATE,
                                       Commands.FORWARD,
                                       Commands.IS_COMPLETED,
                                       Commands.WAIT_FOR_COMPLETED };
    if (enabled (neededCommands))
    {
      return new MotionImpl (out, use, this, slot, commands);
    }
    else
    {
      if (out.module)
      {
        out.module (this,
                    "Kein Laserscanner abfragbar. "
                    + "Keine Einheit erzeugt.");
      }

      return null;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Zugriff auf das Ziel der aktuellen Bewegung
  /////////////////////////////////////////////////////////////////////////////

  /** Set eine neues Ziel. */
  synchronized void setDestination (Pose pose)
  {
    this.destination = pose;
  }

  /** Liefert das aktuelle Ziel. */
  Pose getDestination ()
  {
    return this.destination;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Zugriff auf die Steuerung
  /////////////////////////////////////////////////////////////////////////////

  /**
   * Liefert den Slot des Roblets&reg;, das den Roboter steuern darf und
   * <tt>null</tt>, wenn kein Roblet den Roboter kontrolliert.
   */
  Slot getSlotOfControllingRoblet ()
  {
    return this.slotOfControllingRoblet;
  }

  synchronized void setSlotOfControllingRoblet (Slot slot)
  {
    this.slotOfControllingRoblet = slot;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Pose des Roboters abspeichern
  /////////////////////////////////////////////////////////////////////////////

  private void storePose ()
  {
    try
    {
      Pose pose = commands.getPosition ();

      prefs.putInt ("poseX", pose.x);
      prefs.putInt ("poseY", pose.y);
      prefs.putInt ("poseTHETA", pose.theta);
    }
    catch (UnknownCommandException ex)
    {
      out.module(this, ex);
    }
    catch (SocketCommunicationException ex)
    {
      out.module(this, ex);
    }
    catch (ProtocolException ex)
    {
      out.module(this, ex);
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Pose des Roboters laden und setzen
  /////////////////////////////////////////////////////////////////////////////

  private void loadPose ()
  {
    try
    {
      int x = prefs.getInt ("poseX", 0);
      int y = prefs.getInt ("poseY", 0);
      int theta = prefs.getInt ("poseTHETA", 0);

      commands.setPosition (new Pose (x, y, theta));
    }
    catch (UnknownCommandException ex)
    {
      out.module(this, ex);
    }
    catch (SocketCommunicationException ex)
    {
      out.module(this, ex);
    }
    catch (ProtocolException ex)
    {
      out.module(this, ex);
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Registry
  /////////////////////////////////////////////////////////////////////////////

  // Module
  public Registry  getRegistry ()    { return registryImpl; }

  private RegistryImpl  registryImpl = new RegistryImpl ();
  private static class  RegistryImpl
      implements Registry
  {
    public boolean  claim (Slot slot, Unit unit)  { return false; }
    public Slot  force (Slot slot, Unit unit)     { return null;  }
    public void  free (Slot slot, Unit unit)      { }
    public void  free (Slot slot)                  { }
    public void  free ()                            { }
  }
}
