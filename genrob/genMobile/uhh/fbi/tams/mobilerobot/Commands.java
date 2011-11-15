package uhh.fbi.tams.mobilerobot;

import uhh.fbi.tams.mobilerobot.unit.Coord;
import uhh.fbi.tams.mobilerobot.unit.GoalInObstacleException;
import uhh.fbi.tams.mobilerobot.unit.InternalPathplannerException;
import uhh.fbi.tams.mobilerobot.unit.Landmark;
import uhh.fbi.tams.mobilerobot.unit.Laserscan;
import uhh.fbi.tams.mobilerobot.unit.PathNotFoundException;
import uhh.fbi.tams.mobilerobot.unit.Point2D;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.StartInObstacleException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.Robiant;
import uhh.fbi.tams.mobilerobot.unit.*;

/**
 * Bin&auml;rprotokoll zur Steuerung und Abfrage des mobilen
 * Serviceroboters in Hamburg &uuml;ber die Robotersteuerung <tt>mobiled</tt>.
 *
 * @since uhh.fbi.tams.mobilerobot 0.1
 * @author Hagen Stanek, Daniel Westhoff
 * @version 0.2
 */
class Commands
{

  //
  // Befehle des Kommunikationspotokoll (Version 0.4)
  //

  // 0x0000  Initialisierung
  static final int PING = 0x00000000;

  static final int CHECK_PROTOCOL = 0x00000001; // sollte immer vorhanden sein

  static final int DEBUGGING = 0x00000002;

  // 0x0001  Betriebsmodus
  static final int GET_MODE = 0x00010000;

  static final int SET_MODE = 0x00010001;

  // 0x0002  Localisation (Marken)
  static final int GET_POSITION = 0x00020000;

  static final int SET_POSITION = 0x00020001;

  static final int GET_POSITION_AND_VARIANCE = 0x00020002;

  static final int GET_POSITION_ODO = 0x00020003;

  static final int GET_NUM_MARKS = 0x00020010;

  static final int LIST_MARKS = 0x00020011;

  static final int GET_MARK = 0x00020012;

  static final int GET_MARKS = 0x00020013;

  static final int GET_ALL_MARKS = 0x00020014;

  static final int GET_MARK_INITIAL = 0x00020015;

  static final int GET_MARKS_INITIAL = 0x00020016;

  static final int GET_ALL_MARKS_INITIAL = 0x00020017;

  static final int GET_ALL_MARKS_WITH_VARIANCES = 0x00020018;

  static final int ADD_MARK = 0x00020020;

  static final int DELETE_MARK = 0x00020030;

  //public static final int DELETE_MARKS = 0x00020031;

  static final int DELETE_ALL_MARKS = 0x00020032;

  static final int MOVE_MARK = 0x00020040;

  // 0x0003  Bewegung
  static final int GET_SPEED = 0x00030000;

  static final int SET_SPEED = 0x00030001;

  static final int GET_SCALE = 0x00030010;

  static final int SET_SCALE = 0x00030011;

  static final int MODIFY_SCALE = 0x00030012;

  static final int STOP = 0x00030100;

  static final int TRANSLATE = 0x00030200;

  static final int FORWARD = 0x00030201;

  //public static final int BACKWARD = 0x00030202;

  static final int ROTATE_ANGLE = 0x00030300;

  static final int ROTATE_POINT = 0x00030301;

  public static final int TURN_ANGLE = 0x00030302;

  static final int MOVE = 0x00030400;

  static final int MOVE_POINT = 0x00030401;

  static final int IS_COMPLETED = 0x00030500;

  static final int WAIT_FOR_COMPLETED = 0x00030501;

  static final int GET_TRAJECTORY = 0x00030510;

  static final int GET_REMAINING_TRAJECTORY = 0x00030511;

  static final int STALLED = 0x00030600;

  static final int APPLY_BRAKES = 0x00030700;

  static final int RELEASE_BRAKES = 0x00030701;

  static final int ARE_BRAKES_RELEASED = 0x00030702;

  // 0x0004  Laserscanner
  static final int GET_NUM_SCANNERS = 0x00040000;

  static final int GET_SCANNER_POSITION = 0x00040010;

  static final int GET_SCAN_SCANNER = 0x00040020;

  static final int GET_SCAN_RADIAL_SCANNER = 0x00040021;

  static final int GET_SCAN_PLATFORM = 0x00040030;

  static final int GET_SCAN_RADIAL_PLATFORM = 0x00040031;

  static final int GET_SCAN_WORLD = 0x00040040;

  // 0x0005  Karte (Linien)
  static final int GET_NUM_LINES = 0x00050000;

  static final int LIST_LINES = 0x00050001;

  static final int GET_LINE = 0x00050002;

  static final int GET_LINES = 0x00050003;

  static final int GET_ALL_LINES = 0x00050004;

  static final int ADD_LINE = 0x00050010;

  static final int DELETE_LINE = 0x00050020;

  //public static final int DELETE_LINES = 0x00050021;

  static final int DELETE_ALL_LINES = 0x00050022;

  static final int MOVE_LINE = 0x00050030;

  //public static final int UPDATE_MAP = 0x00050040;

  static final int GET_PATH = 0x00050050;

  // 0x0006 (Odometrie)
  static final int START_ODO_LOGGING = 0x00060000;

  static final int STOP_ODO_LOGGING = 0x00060001;

  // 0x0007 (Sensoren)
  static final int GET_BATTERY_VOLTAGE = 0x00070000;

  static final int GET_DRIVE_TEMPERATURES = 0x00070010;

  //
  // private boolsche Variablen f�r Kommandos
  //

  // 0x0000  Initialisierung
  private boolean ping = false;

  private boolean debugging = false;

  // 0x0001  Betriebsmodus
  private boolean getMode = false;

  private boolean setMode = false;

  // 0x0002  Localisation (Marken)
  private boolean getPosition = false;

  private boolean setPosition = false;

  private boolean getPositionAndVariance = false;

  private boolean getPositionOdo = false;

  private boolean getNumMarks = false;

  private boolean listMarks = false;

  private boolean getMark = false;

  private boolean getMarks = false;

  private boolean getAllMarks = false;

  private boolean getMarkInitial = false;

  private boolean getMarksInitial = false;

  private boolean getAllMarksInitial = false;

  private boolean getAllMarksWithVariances = false;

  private boolean addMark = false;

  private boolean deleteMark = false;

  //public boolean m_bDeleteMarks = false;
  private boolean deleteAllMarks = false;

  private boolean moveMark = false;

  // 0x0003  Bewegung
  private boolean getSpeed = false;

  private boolean setSpeed = false;

  private boolean getScale = false;

  private boolean setScale = false;

  private boolean modifyScale = false;

  private boolean stop = false;

  private boolean translate = false;

  private boolean forward = false;

  //private boolean m_bBackward = false;
  private boolean rotateAngle = false;

  private boolean rotatePoint = false;

  private boolean turnAngle = false;

  private boolean move = false;

  private boolean movePoint = false;

  private boolean isCompleted = false;

  private boolean waitForCompleted = false;

  private boolean getTrajectory = false;

  private boolean getRemainingTrajectory = false;

  private boolean stalled = false;

  private boolean applyBrakes = false;

  private boolean releaseBrakes = false;

  private boolean areBrakesReleased = false;

  // 0x0004  Laserscanner
  private boolean getNumScanners = false;

  private boolean getScannerPosition = false;

  private boolean getScanScanner = false;

  private boolean getScanRadialScanner = false;

  private boolean getScanPlatform = false;

  private boolean getScanRadialPlatform = false;

  private boolean getScanWorld = false;

  // 0x0005  Karte (Linien)
  private boolean getNumLines = false;

  private boolean listLines = false;

  private boolean getLine = false;

  private boolean getLines = false;

  private boolean getAllLines = false;

  private boolean addLine = false;

  private boolean deleteLine = false;

  //private boolean m_bDeleteLines = false;

  private boolean deleteAllLines = false;

  private boolean moveLine = false;

  //private boolean m_bUpdateMap = false

  private boolean getPath = false;

  // 0x0006 Odometrie
  private boolean startOdoLogging = false;

  private boolean stopOdoLogging = false;

  // 0x0007 Sensoren
  private boolean getBatteryVoltage = false;

  private boolean getDriveTemperatures = false;

  //
  // Konstanten
  //
  private static final int COMMAND_LIMIT = 999;

  private static final int MARKS_LIMIT = 999;

  private static final int SCANNERS_LIMIT = 9;

  private static final int SCANS_LIMIT = 999;

  private static final int SCAN_MARKS_LIMIT = 99;

  private static final int LINES_LIMIT = 9999;

  /** TCP/IP-Socket zur Kommunikation mit der Roboter-Steuerung */
  private final BinarySocket binarySocket;

  /** Server-Log */
  private final ModuleLog out;

  /** Antwort-Telegram. */
  private TelegramReply reply;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Default-Konstruktor. */
  private Commands ()
  {
    this.binarySocket = null;
    this.out = null;
  }

  /**
   * Konstruktor.
   *
   * @param binarySocket Socket-Verbindung f&uuml;r die Kommunikation mit der
   *                     Robotersteuerung <tt>mobiled</tt>
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn bei der
   *                                 &Uuml;berpr&uuml;fung des Protokolls
   *                                 ein unbekanntes Kommando gefunden wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  Commands (final BinarySocket binarySocket,
            final ModuleLog out)
      throws UnknownCommandException,
             ProtocolException,
             SocketCommunicationException
  {
    this.binarySocket = binarySocket;
    this.out = out;

    this.checkProtocol ();
  }

  /**
   * &Uuml;berpr&uuml;ft welche Befehle des Bin&auml;r-Protokolls
   * die Robotersteuerung <tt>mobiled</tt> unterst&uuml;tzt.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn bei der
   *                                 &Uuml;berpr&uuml;fung des Protokolls
   *                                 ein unbekanntes Kommando gefunden wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  protected synchronized void checkProtocol ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException
  {
    if (out.module)
    {
      out.module
          (this,
           "Sende Anfrage nach bekannten Befehlen (checkProtocol)...");
    }
    // setze zu Abfragekommando
    int command = Commands.CHECK_PROTOCOL;

    if (out.module)
    {
      out.module
          (this,
           "Anfrage wird erstellt.");
    }

    // erstelle Telegramm
    TelegramRequest rTelegramToSend = new TelegramRequest (command, 0);

    if (out.module)
    {
      out.module
          (this,
           "Anfrage wird gesendet.");
    }

    // sende Telegramm und empfange Antwort
    TelegramReply rTelegramToReceive = binarySocket.send (
        rTelegramToSend);

    if (out.module)
    {
      out.module
          (this,
           "Antwort empfangen.");
    }

    // schreibe Telegramdaten auf stdout
    rTelegramToReceive.dump ();

    if (out.module)
    {
      out.module
          (this,
           "Antwort wird &uuml;berpr&uuml;ft.");
    }

    // kontrolliere das Kommando
    Protocol.ensureCorrectAnswerCommand (rTelegramToReceive, command);

    // kontrolliere den Status
    Protocol.ensureStatusOfZero (rTelegramToReceive);

    // ermittle die Versionsnummer (zur Zeit keine weiteren Kontrollen)
    int version = rTelegramToReceive.getInteger (1);

    if (out.module)
    {
      out.module (this, "Protokoll-Version: " + version);
    }

    // Anzahl unterst�tzter Kommandos
    int iCount = Protocol.ensureLimit (rTelegramToReceive,
                                       Protocol.COUNT,
                                       2,
                                       COMMAND_LIMIT);

    if (out.module)
    {
      out.module (this,
                  "Anzahl der unterst\u00FCtzten Befehle: "
                  + iCount);
    }

    // hole die unterst�tzten Kommandos
    int commands[] = new int[iCount];

    for (int i = 0; i < iCount; ++i)
    {
      commands[i] = rTelegramToReceive.getInteger (3 + i);
    }

    StringBuffer commandsStrBuff = new StringBuffer ("Unterst\u00FCtzte Befehle:<br>");

    try
    {
      for (int i = 0; i < iCount; ++i)
      {
        switch (commands[i])
        {
          // Initialisierung
          case Commands.PING:
            ping = true;
            commandsStrBuff.append ("- Ping()<br>");
            break;
          case Commands.CHECK_PROTOCOL: // eigentlich �berfl�ssig, hat ja schon funktioniert
            commandsStrBuff.append ("- CheckProtocol()<br>");
            break;
          case Commands.DEBUGGING:
            debugging = true;
            commandsStrBuff.append ("- Debugging()<br>");
            break;
            // Betriebsmodus
          case Commands.GET_MODE:
            getMode = true;
            commandsStrBuff.append ("- GetMode()<br>");
            break;
          case Commands.SET_MODE:
            setMode = true;
            commandsStrBuff.append ("- SetMode()<br>");
            break;
            // Lokalisation (Marken)
          case Commands.GET_POSITION:
            getPosition = true;
            commandsStrBuff.append ("- GetPosition()<br>");
            break;
          case Commands.SET_POSITION:
            setPosition = true;
            commandsStrBuff.append ("- SetPosition()<br>");
            break;
          case Commands.GET_POSITION_AND_VARIANCE:
            getPositionAndVariance = true;
            commandsStrBuff.append ("- GetPositionAndVariance()<br>");
            break;
          case Commands.GET_POSITION_ODO:
            getPositionOdo = true;
            commandsStrBuff.append ("- GetPositionOdo()<br>");
            break;
          case Commands.GET_NUM_MARKS:
            getNumMarks = true;
            commandsStrBuff.append ("- GetNumMarks()<br>");
            break;
          case Commands.LIST_MARKS:
            listMarks = true;
            commandsStrBuff.append ("- ListMarks()<br>");
            break;
          case Commands.GET_MARK:
            getMark = true;
            commandsStrBuff.append ("- GetMark()<br>");
            break;
          case Commands.GET_MARKS:
            getMarks = true;
            commandsStrBuff.append ("- GetMarks()<br>");
            break;
          case Commands.GET_ALL_MARKS:
            getAllMarks = true;
            commandsStrBuff.append ("- GetAllMarks()<br>");
            break;
          case Commands.GET_MARK_INITIAL:
            getMarkInitial = true;
            commandsStrBuff.append ("- GetMarkInitial()<br>");
            break;
          case Commands.GET_MARKS_INITIAL:
            getMarksInitial = true;
            commandsStrBuff.append ("- GetMarksInitial()<br>");
            break;
          case Commands.GET_ALL_MARKS_INITIAL:
            getAllMarksInitial = true;
            commandsStrBuff.append ("- GetAllMarksInitial()<br>");
            break;
          case Commands.GET_ALL_MARKS_WITH_VARIANCES:
            getAllMarksWithVariances = true;
            commandsStrBuff.append ("- GetAllMarksWithVariances()<br>");
            break;
          case Commands.ADD_MARK:
            addMark = true;
            commandsStrBuff.append (" -  AddMark()<br>");
            break;
          case Commands.DELETE_MARK:
            deleteMark = true;
            commandsStrBuff.append (" - DeleteMark()<br>");
            break;
          case Commands.DELETE_ALL_MARKS:
            deleteAllMarks = true;
            commandsStrBuff.append (" - GetDeleteAllMarks()<br>");
            break;
          case Commands.MOVE_MARK:
            moveMark = true;
            commandsStrBuff.append (" - MoveMark()<br>");
            break;
            // Bewegung
          case Commands.GET_SPEED:
            getSpeed = true;
            commandsStrBuff.append (" - GetSpeed()<br>");
            break;
          case Commands.SET_SPEED:
            setSpeed = true;
            commandsStrBuff.append (" - SetSpeed()<br>");
            break;
          case Commands.GET_SCALE:
            getScale = true;
            commandsStrBuff.append (" - GetScale()<br>");
            break;
          case Commands.SET_SCALE:
            setScale = true;
            commandsStrBuff.append (" - SetScale()<br>");
            break;
          case Commands.MODIFY_SCALE:
            modifyScale = true;
            commandsStrBuff.append (" - ModifyScale()<br>");
            break;
          case Commands.STOP:
            stop = true;
            commandsStrBuff.append (" - Stop()<br>");
            break;
          case Commands.TRANSLATE:
            translate = true;
            commandsStrBuff.append (" - Translate()<br>");
            break;
          case Commands.FORWARD:
            forward = true;
            commandsStrBuff.append (" - Forward()<br>");
            break;
          case Commands.ROTATE_ANGLE:
            rotateAngle = true;
            commandsStrBuff.append (" - Rotate()<br>");
            break;
          case Commands.ROTATE_POINT:
            rotatePoint = true;
            commandsStrBuff.append (" - RotatePoint()<br>");
            break;
          case Commands.TURN_ANGLE:
            turnAngle = true;
            commandsStrBuff.append (" - Turn()<br>");
            break;
          case Commands.MOVE:
            move = true;
            commandsStrBuff.append (" - Move()<br>");
            break;
          case Commands.MOVE_POINT:
            movePoint = true;
            commandsStrBuff.append (" - MovePoint()<br>");
            break;
          case Commands.IS_COMPLETED:
            isCompleted = true;
            commandsStrBuff.append (" - IsCompleted()<br>");
            break;
          case Commands.WAIT_FOR_COMPLETED:
            waitForCompleted = true;
            commandsStrBuff.append (" - WaitForCompleted()<br>");
            break;
          case Commands.GET_TRAJECTORY:
            getTrajectory = true;
            commandsStrBuff.append (" - GetTrajectory()<br>");
            break;
          case Commands.GET_REMAINING_TRAJECTORY:
            getRemainingTrajectory = true;
            commandsStrBuff.append (" - GetRemainingTrajectory()<br>");
            break;
          case Commands.STALLED:
            stalled = true;
            commandsStrBuff.append (" - Stalled()<br>");
            break;
          case Commands.APPLY_BRAKES:
            applyBrakes = true;
            commandsStrBuff.append (" - ApplyBrakes()<br>");
            break;
          case Commands.RELEASE_BRAKES:
            releaseBrakes = true;
            commandsStrBuff.append (" - ReleaseBrakes()<br>");
            break;
          case Commands.ARE_BRAKES_RELEASED:
            areBrakesReleased = true;
            commandsStrBuff.append (" - AreBrakesReleased()<br>");
            break;
            // Laserscanner
          case Commands.GET_NUM_SCANNERS:
            getNumScanners = true;
            commandsStrBuff.append (" - GetNumScanners()<br>");
            break;
          case Commands.GET_SCANNER_POSITION:
            getScannerPosition = true;
            commandsStrBuff.append (" - GetScannerPosition()<br>");
            break;
          case Commands.GET_SCAN_SCANNER:
            getScanScanner = true;
            commandsStrBuff.append (" - GetScanScanner()<br>");
            break;
          case Commands.GET_SCAN_RADIAL_SCANNER:
            getScanRadialScanner = true;
            commandsStrBuff.append (" - GetScanRadialScanner()<br>");
            break;
          case Commands.GET_SCAN_PLATFORM:
            getScanPlatform = true;
            commandsStrBuff.append (" - GetScanPlatform()<br>");
            break;
          case Commands.GET_SCAN_RADIAL_PLATFORM:
            getScanRadialPlatform = true;
            commandsStrBuff.append (" - GetScanRadialPlatform()<br>");
            break;
          case Commands.GET_SCAN_WORLD:
            getScanWorld = true;
            commandsStrBuff.append (" - GetScanWorld()<br>");
            break;
            // Karte (Linien)
          case Commands.GET_NUM_LINES:
            getNumLines = true;
            commandsStrBuff.append (" - GetNumLines()<br>");
            break;
          case Commands.LIST_LINES:
            listLines = true;
            commandsStrBuff.append (" - ListLines()<br>");
            break;
          case Commands.GET_LINE:
            getLine = true;
            commandsStrBuff.append (" - GetLine()<br>");
            break;
          case Commands.GET_LINES:
            getLines = true;
            commandsStrBuff.append (" - GetLines()<br>");
            break;
          case Commands.GET_ALL_LINES:
            getAllLines = true;
            commandsStrBuff.append (" - GetAllLines()<br>");
            break;
          case Commands.ADD_LINE:
            addLine = true;
            commandsStrBuff.append (" - AddLine()<br>");
            break;
          case Commands.DELETE_LINE:
            deleteLine = true;
            commandsStrBuff.append (" - DeleteLine()<br>");
            break;
          case Commands.DELETE_ALL_LINES:
            deleteAllLines = true;
            commandsStrBuff.append (" - DeleteAllLines()<br>");
            break;
          case Commands.MOVE_LINE:
            moveLine = true;
            commandsStrBuff.append (" - MoveLine()<br>");
            break;
          case Commands.GET_PATH:
            getPath = true;
            commandsStrBuff.append (" - GetPath()<br>");
            break;
            // Odometrie
          case Commands.START_ODO_LOGGING:
            startOdoLogging = true;
            commandsStrBuff.append (" - StartOdoLogging ()<br>");
            break;
          case Commands.STOP_ODO_LOGGING:
            stopOdoLogging = true;
            commandsStrBuff.append (" - StopOdoLogging ()<br>");
            break;
            // Batterie
          case Commands.GET_BATTERY_VOLTAGE:
            getBatteryVoltage = true;
            commandsStrBuff.append (" - GetBatteryVoltage ()<br>");
            break;
            // Temperature
          case Commands.GET_DRIVE_TEMPERATURES:
            getDriveTemperatures = true;
            commandsStrBuff.append (" - GetDriveTemperatures ()<br>");
            break;
          default:
            throw new UnknownCommandException ("Unbekanntes Kommando: "
                                               + commands[i]);
        }
      }
    }
    finally
    {
      if (out.module)
      {
        out.module (this, commandsStrBuff.toString ());
      }
    }
  }

  /**
   * &Uuml;berprüft, ob das &uuml;bergebene Kommando von der Robotersteuerung
   * <tt>mobiled</tt> implementiert ist.
   *
   * @param command Kommando, das &uuml;berpr&uuml;ft werden soll.
   *
   * @return <tt>true</tt>, wenn das Kommando implementiert ist, sonst
   *         <tt>false</tt>
   *
   * @throws UnknownCommandException Wird ausgel&ouml;st, wenn ein unbekanntes
   *                                 Kommando abgefragt wird.
   */
  synchronized boolean isEnabled (int command)
      throws UnknownCommandException
  {
    if (out.module)
    {
      out.module (this, "isEnabled(" + Convert.toHex (command) + ")");
    }

    switch (command)
    {
      // Initialisierung
      case Commands.PING:
        return ping;
      case Commands.CHECK_PROTOCOL: // sollte immer zur Verf�gung stehen
        return true;
      case Commands.DEBUGGING:
        return debugging;
        // Betriebsmodus
      case Commands.GET_MODE:
        return getMode;
      case Commands.SET_MODE:
        return setMode;
        // Lokalisation (Marken)
      case Commands.GET_POSITION:
        return getPosition;
      case Commands.SET_POSITION:
        return setPosition;
      case Commands.GET_POSITION_AND_VARIANCE:
        return getPositionAndVariance;
      case Commands.GET_POSITION_ODO:
        return getPositionOdo;
      case Commands.GET_NUM_MARKS:
        return getNumMarks;
      case Commands.LIST_MARKS:
        return listMarks;
      case Commands.GET_MARK:
        return getMark;
      case Commands.GET_MARKS:
        return getMarks;
      case Commands.GET_ALL_MARKS:
        return getAllMarks;
      case Commands.GET_MARK_INITIAL:
        return getMarkInitial;
      case Commands.GET_MARKS_INITIAL:
        return getMarksInitial;
      case Commands.GET_ALL_MARKS_INITIAL:
        return getAllMarksInitial;
      case Commands.GET_ALL_MARKS_WITH_VARIANCES:
        return getAllMarksWithVariances;
      case Commands.ADD_MARK:
        return addMark;
      case Commands.DELETE_MARK:
        return deleteMark;
      case Commands.DELETE_ALL_MARKS:
        return deleteAllMarks;
      case Commands.MOVE_MARK:
        return moveMark;
        // Bewegung
      case Commands.GET_SPEED:
        return getSpeed;
      case Commands.SET_SPEED:
        return setSpeed;
      case Commands.GET_SCALE:
        return getScale;
      case Commands.SET_SCALE:
        return setScale;
      case Commands.MODIFY_SCALE:
        return modifyScale;
      case Commands.STOP:
        return stop;
      case Commands.TRANSLATE:
        return translate;
      case Commands.FORWARD:
        return forward;
      case Commands.ROTATE_ANGLE:
        return rotateAngle;
      case Commands.ROTATE_POINT:
        return rotatePoint;
      case Commands.TURN_ANGLE:
        return turnAngle;
      case Commands.MOVE:
        return move;
      case Commands.MOVE_POINT:
        return movePoint;
      case Commands.IS_COMPLETED:
        return isCompleted;
      case Commands.WAIT_FOR_COMPLETED:
        return waitForCompleted;
      case Commands.GET_TRAJECTORY:
        return getTrajectory;
      case Commands.GET_REMAINING_TRAJECTORY:
        return getRemainingTrajectory;
      case Commands.STALLED:
        return stalled;
      case Commands.APPLY_BRAKES:
        return applyBrakes;
      case Commands.RELEASE_BRAKES:
        return releaseBrakes;
      case Commands.ARE_BRAKES_RELEASED:
        return areBrakesReleased;
        // Laserscanner
      case Commands.GET_NUM_SCANNERS:
        return getNumScanners;
      case Commands.GET_SCANNER_POSITION:
        return getScannerPosition;
      case Commands.GET_SCAN_SCANNER:
        return getScanScanner;
      case Commands.GET_SCAN_RADIAL_SCANNER:
        return getScanRadialScanner;
      case Commands.GET_SCAN_PLATFORM:
        return getScanPlatform;
      case Commands.GET_SCAN_RADIAL_PLATFORM:
        return getScanRadialPlatform;
      case Commands.GET_SCAN_WORLD:
        return getScanWorld;
        // Karte (Linien)
      case Commands.GET_NUM_LINES:
        return getNumLines;
      case Commands.LIST_LINES:
        return listLines;
      case Commands.GET_LINE:
        return getLine;
      case Commands.GET_LINES:
        return getLines;
      case Commands.GET_ALL_LINES:
        return getAllLines;
      case Commands.ADD_LINE:
        return addLine;
      case Commands.DELETE_LINE:
        return deleteLine;
      case Commands.DELETE_ALL_LINES:
        return deleteAllLines;
      case Commands.MOVE_LINE:
        return moveLine;
      case Commands.GET_PATH:
        return getPath;
        // Odometrie
      case Commands.START_ODO_LOGGING:
        return startOdoLogging;
      case Commands.STOP_ODO_LOGGING:
        return stopOdoLogging;
        // Batterie
      case Commands.GET_BATTERY_VOLTAGE:
        return getBatteryVoltage;
        // Temperaturen
      case Commands.GET_DRIVE_TEMPERATURES:
        return getDriveTemperatures;
      default:
        throw new UnknownCommandException
            ("Befehl nicht im Protokoll definiert.");
    }
  }

  /** Sende Kommando, warte auf Antwort-Telegramm und &uuml;berpr&uuml;fe
   * Antwort-Telegram.
   *
   * @param command Zu sendendes Kommando.
   *
   * @return Antwort-Telegram.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.

   */
  private TelegramReply send (TelegramRequest request)
      throws ProtocolException,
             UnknownCommandException,
             SocketCommunicationException
  {
    if (isEnabled (request.getCommand()))
    {
      // Telegram senden und Antwort empfangen
      reply = binarySocket.send (request);

      // Antwort ausgeben
      if (out.module)
      {
        reply.dump ();
      }

      // Teste Antwort
      Protocol.ensureCorrectAnswerCommand (reply, request.getCommand());

      // Teste Status
      Protocol.ensureStatusOfZero (reply);

      return reply;
    }
    else
    {
      throw new UnknownCommandException
          ("Befehl "
           + Convert.toHex(request.getCommand())
           + " nicht verf\u00FCgbar.");
    }
  }

  /**
   * Sendet den Befehl, um den Roboter die &uuml;bergebene Distanz vorw&auml;rts
   * bzw. r&uuml;ckw&auml;rts zu fahren.
   *
   * @param distance Distanz in [mm], bei positiven Werten f&auml;hrt der
   *                 Roboter nach vorne, bei negaitven r&uuml;ckwärts.
   *
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           gel&ouml;st wurden.
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized void forward (int distance)
      throws BrakesOnException,
             ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // Telegram erstellen
    TelegramRequest request = new TelegramRequest (Commands.FORWARD, 1);

    // setze DISTANCE
    request.putFloat (0, distance / 1000.0f);

    try
    {
      send (request);
    }
    catch (ProtocolException pe)
    {
      // Falls der Status gleich 1 ist, dann waren die Bremsen noch angezogen.
      if (this.reply.getInteger(0) == 1)
        throw new BrakesOnException ("Bremsen sind angezogen, Roboter kann nicht bewegt werden.", pe);
      else
        throw pe;
    }
  }

  /**
   * Sendet den Befehl um den Roboter in die angegebene Richtung zu drehen.
   *
   * @param orientation Zielorientierung in
   *                    [{@link uhh.fbi.tams.mobilerobot.unit.Robiant}]
   *
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           gel&ouml;st wurden.
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized void rotate (int orientation)
      throws BrakesOnException,
             ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // Telegram erstellen
    TelegramRequest request = new TelegramRequest (Commands.ROTATE_ANGLE, 1);

    // setze ANGLE
    request.putFloat (0, Robiant.robiant2radAsFloat (orientation));

    try
    {
      send (request);
    }
    catch (ProtocolException pe)
    {
      if (this.reply.getInteger (0) == 1)
        throw new BrakesOnException
            ("Bremsen sind angezogen, Roboter kann nicht bewegt werden.", pe);
      else
        throw pe;
    }
  }

  /**
   * Sendet den Befehl um den Roboter in die angegebene Richtung zu drehen,
   * <font color="red"> wobei die Kollisionsvermeidung abgeschaltet ist.</font>.
   *
   * @param orientation Zielorientierung in
   *                    [{@link uhh.fbi.tams.mobilerobot.unit.Robiant}]
   *
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           gel&ouml;st wurden.
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized void turn (int orientation)
      throws BrakesOnException,
             ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // Telegram erstellen
    TelegramRequest request = new TelegramRequest (Commands.TURN_ANGLE, 1);

    // setze ANGLE
    request.putFloat (0, Robiant.robiant2radAsFloat (orientation));

    try
    {
      send (request);
    }
    catch (ProtocolException pe)
    {
      if (this.reply.getInteger (0) == 1)
        throw new BrakesOnException
            ("Bremsen sind angezogen, Roboter kann nicht bewegt werden.", pe);
      else
        throw pe;
    }
  }


  /**
   * Sendet den Befehl um den Roboter gerade auf die angegebene Pose
   * zu fahren.
   *
   * @param pose Pose zu der der Roboter fahren soll.
   *
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           gel&ouml;st wurden.
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist
   */
   synchronized void translate (Pose pose)
      throws BrakesOnException,
             ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // Telegram erstellen
    TelegramRequest request = new TelegramRequest (Commands.TRANSLATE, 2);

    // Umwandeln in Meter und Float
    float posXinM = pose.x / 1000.0f;
    float posYinM = pose.y / 1000.0f;

    // setze POSX
    request.putFloat (0, posXinM);
    // setze POSY
    request.putFloat (1, posYinM);

    try
    {
      send (request);
    }
    catch (ProtocolException pe)
    {
      // Falls der Status gleich 1 ist, dann waren die Bremsen noch angezogen.
      if (this.reply.getInteger(0) == 1)
        throw new BrakesOnException ("Bremsen sind angezogen, Roboter kann nicht bewegt werden.", pe);
      else
        throw pe;
    }
  }

  /**
   * Sendet den Befehl um den Roboter auf die angegebene Pose
   * zu fahren.
   *
   * @param pose Pose zu der der Roboter fahren soll.
   *
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           gel&ouml;st wurden.
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   * @throws InternalPathplannerException Wird ausgel&ouml;st, wenn die interne
   *                                      Pfadplanung der Robotersteuerung
   *                                      <tt>mobiled</tt> einen Fehler
   *                                      festgestellt hat.
   */
  synchronized void move (Pose pose)
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException,
             InternalPathplannerException,
             BrakesOnException
  {
    // Telegram erstellen
    TelegramRequest request = new TelegramRequest (Commands.MOVE, 3);

    // Umwandeln in Meter, Radiant und Float
    float posXinM = pose.x / 1000.0f;
    float posYinM = pose.y / 1000.0f;
    float posAinRad = Robiant.robiant2radAsFloat (pose.theta);

    // setze POSX
    request.putFloat (0, posXinM);
    // setze POSY
    request.putFloat (1, posYinM);
    // setze POSA
    request.putFloat (2, posAinRad);

    try
    {
      this.reply = send (request);
    }
    catch (ProtocolException pe)
    {
      int iStatus = this.reply.getInteger (0);

      switch (iStatus)
      {
        case 1:
          out.module (this,
                      "<font color=red>Bremsen sind angezogen, Roboter kann nicht bewegt werden.</font>");
          throw new BrakesOnException ("Bremsen sind angezogen, Roboter kann nicht bewegt werden.");
        case -1:
          out.module (this,
                      "<font color=red>Roboter steht in einem Hinderniss! Roboter von Hand verschieben!</font>");
          throw new StartInObstacleException
              ("Roboter steht in einem Hinderniss! Roboter von Hand verschieben!",
               pe);
        case -2:
          out.module (this, "<font color=red>Zielpunkt liegt in einem Hinderniss! Roboter kann Ziel nicht erreichen!</font>");
          throw new GoalInObstacleException
              ("Zielpunkt liegt in einem Hinderniss! Roboter kann Ziel nicht erreichen!",
               pe);
        case -3:
          out.module (this,
                      "<font color=red>Keinen befahrbarer Pfad zum Zielpunkt gefunden!</font>");
          throw new PathNotFoundException
              ("Keinen befahrbarer Pfad zum Zielpunkt gefunden!",
               pe);
        default:
          out.module (this, pe);
          throw pe;
      }
    }
  }

  /**
   * Gibt zur&uuml;ck, ob eine Bewegung aktiv ist.
   *
   * @return <tt>true</tt>, wenn keine Bewegung aktiv ist, sonst <tt>false</tt>.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized boolean isCompleted ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // Telegram senden und Antwort empfangen
    final TelegramReply reply = send (new TelegramRequest (Commands.IS_COMPLETED, 0));

    // Antwort auslesen
    int flag = reply.getInteger (1);

    if (flag == 0)
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  /**
   * Wartet bis eine aktive Bewegung beendet ist.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  void waitForCompleted ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    while (!isCompleted ())
    {
      try
      {
        Thread.sleep (100);
      }
      catch (InterruptedException ex)
      {
        out.module (this, ex);
      }
    }
  }

  /**
   * H&auml;lt eine aktive Bewegung an.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized void stop ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
     send (new TelegramRequest (Commands.STOP, 0));
  }

  /**
   * Liefert die Pose an der sich der Roboter laut Selbst-Lokalisierung
   * befindet.
   *
   * @return Pose Pose an der sich der Roboter befindet.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized Pose getPosition ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // senden und empfangen
    TelegramReply reply = send (new TelegramRequest (Commands.GET_POSITION, 0));

    // get POSX
    float posXinM = reply.getFloat (1);
    // get POSY
    float posYinM = reply.getFloat (2);
    // get POSA
    float posAinRad = reply.getFloat (3);

    // Umwandeln in Millimeter, Robiant und int
    int x = (int) (posXinM * 1000);
    int y = (int) (posYinM * 1000);
    int c = Robiant.rad2robiant (posAinRad);

    return new Pose (x, y, c);
  }

  /**
   * Setzt die Pose des Roboters. Die Selbst-Lokalisierung geht von dieser
   * Position als neuer initaler Sch&auml;tzung aus.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized void setPosition (Pose pose)
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // Telegramm erstellen (3 Datenworte)
    TelegramRequest request = new TelegramRequest (Commands.SET_POSITION, 3);

    // Umwandeln in Meter, Radiant und Float
    float posXinM = pose.x / 1000.0f;
    float posYinM = pose.y / 1000.0f;
    float posAinRad = Robiant.robiant2radAsFloat (pose.theta);

    // setze POSX
    request.putFloat (0, posXinM);
    // setze POSY
    request.putFloat (1, posYinM);
    // setze POSA
    request.putFloat (2, posAinRad);

    // senden und empfangen
    send (request);
  }

  /**
   * Fragt die Anzahl der Laserscanner ab.
   *
   * @return int Anzahl der Laserscanner (>= 0)
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized int getNumScanners ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // senden und empfangen
    TelegramReply reply = send (new TelegramRequest (GET_NUM_SCANNERS, 0));

    // Anzahl der verf�gbaren Laserscanner
    return Protocol.ensureLimit (reply,
                                 Protocol.NUM,
                                 1,
                                 SCANNERS_LIMIT);
  }

  /**
   * Liefert die aktuellen Abstandsmessungen des Laserscanners
   * mit der &uuml;bergebenen ID zur&uuml;ck.
   *
   * @param scanner ID des Laserscanners (>= 0)
   *
   * @return int[]  Die 361 Abstandsmessungen.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
    *                                 von der Robotersteuerung <tt>mobiled</tt>
    *                                 implementiert wird.
    * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
    *                           nicht zul&auml;ssig ist.
    * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
    *                                      Socket-Kommunikation mit der
    *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized int[] getScanRadial (int scanner)
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // Telegramm, das gesendet wird. IDX des Scanners wird gesendet,
    // also Datenwort
    TelegramRequest request
        = new TelegramRequest (Commands.GET_SCAN_RADIAL_SCANNER, 1);

    // setze Laserscanner IDX
    request.putInteger (0, scanner);

    // senden und empfangen
    TelegramReply reply = send (request);

    // Z�hler f�r aktuelles Datum
    int slot = 0;

    int[] scan = new int[361];

    // Alle Scans aus Telegramm lesen und in Millimeter umrechnen
    for (int n = 0; n < 361; ++n)
    {
      scan[n] = (int) Math.round (1000.0 * reply.getFloat (++slot));
    }

    return scan;
  }

  /**
   * Liefert je nach Kommando einen Laserscan in dem zugehörigen
   * Koordinatensystem.
   *
   * @param command Kommando.
   * @param scanner ID des Scanners (ID >= 0).
   * @return Laserscan.
   *
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   */
  private Laserscan getScan (int command, int scanner)
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException
  {
    // Telegramm, das gesendet wird. IDX des Scanners wird gesendet,
    // also Datenwort
    TelegramRequest request = new TelegramRequest (command, 1);

    // setze Laserscanner IDX
    request.putInteger (0, scanner);

    // senden und empfangen
    TelegramReply reply = send (request);

    // Z�hler f�r aktuelles Datum
    int slot = 0;

    // Anzahl der Scanpunkte
    int scans = Protocol.ensureLimit (reply,
                                      Protocol.SCANS,
                                      ++slot,
                                      SCANS_LIMIT);

    Point2D[] scan = new Point2D[scans];

    Point2D point;

    // Alle Scans aus Telegramm lesen
    for (int n = 0; n < scans; ++n)
    {
      // Punkt in Roboterbasiskoordinaten auslesen und in mm umrechnen
      point = new Point2D ((int) (1000 * reply.getFloat (++slot)),
                           (int) (1000 * reply.getFloat (++slot)));

      scan[n] = point;
    }

    // Anzahl der Marken
    int numberOfMarks = Protocol.ensureLimit (reply,
                                              Protocol.MARKS,
                                              ++slot,
                                              SCAN_MARKS_LIMIT);

    Point2D[] marks = new Point2D[numberOfMarks];

    // Alle	Marken aus Telegramm lesen
    for (int n = 0; n < numberOfMarks; ++n)
    {
      // Marke in Roboterbasiskoordinaten auslesen und in mm umrechnen
      point = new Point2D ((int) (1000 * reply.getFloat (++slot)),
                           (int) (1000 * reply.getFloat (++slot)));

      // Marken-ID
      // @todo zur Zeit wird die Marken-IDs ignoriert
      /*int  iID = rTelegramToReceive. getInteger (*/
      ++slot /*)*/;

      marks[n] = point;
    }

    return new Laserscan (scan, marks);
  }


  /**
   * Liefert den aktuellen Scan des Laserscanners mit der &uuml;bergebenen ID
   * in Laserscanner-Koordinaten zur&uuml;ck.
   *
   * @param scanner int  ID des Laserscanners (>= 0)
   *
   * @return Lasercan mit den Scanpunkten und den Markenpositionen in
   *         Roboterkoordinaten
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized Laserscan getScanScanner (int scanner)
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    return getScan (Commands.GET_SCAN_SCANNER, scanner);
  }

  /**
   * Liefert den aktuellen Scan des Laserscanners mit der &uuml;bergebenen ID
   * in Roboter-Koordinaten zur&uuml;ck.
   *
   * @param scanner int  ID des Laserscanners (>= 0)
   *
   * @return Lasercan mit den Scanpunkten und den Markenpositionen in
   *         Roboterkoordinaten
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized Laserscan getScanPlatform (int scanner)
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    return getScan (Commands.GET_SCAN_PLATFORM, scanner);
  }

  /**
   * Liefert den aktuellen Scan des Laserscanners mit der &uuml;bergebenen ID
   * im Weltkoordinatensystem zur&uuml;ck.
   *
   * @param scanner int  ID des Laserscanners (>= 0)
   *
   * @return Lasercan mit den Scanpunkten und den Markenpositionen in
   *         Weltkoordinaten.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized Laserscan getScanWorld (int scanner)
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    return getScan (Commands.GET_SCAN_WORLD, scanner);
  }

  /**
   * Liefert die position und Orientierung eines Laserscanners
   * in Roboterkoordinaten als {@link Frame2D}.
   *
   * @param scanner ID des laserscanners.
   *
   * @return Poisiton und Orientierung des Laserscanners.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized Frame2D getScannerPosition (int scanner)
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // Telegramm erstellen
    TelegramRequest request
        = new TelegramRequest (Commands.GET_SCANNER_POSITION, 1);

    request.putInteger (0, scanner);

    // senden und empfangen
    TelegramReply reply = send (request);

    // get POSX
    float posXinM = reply.getFloat (1);
    // get POSY
    float posYinM = reply.getFloat (2);
    // get POSA
    float posAinRad = reply.getFloat (3);

    // Umwandeln in Millimeter, Robiant und int
    int x = (int) Math.round (posXinM * 1000.0);
    int y = (int) Math.round (posYinM * 1000.0);
    int theta = Robiant.rad2robiant (posAinRad);

    return new Frame2D (x, y, theta);
  }

  /**
   * Liefert je nach Kommando die aktuellen oder initialen Positionen
   * der Reflektormarken, die von der Roboterstuerung <tt>mobiled</tt>
   * zur Selbst-Lokalisierung verwendet werden.
   *
   * @return Liste der Reflektormarken.
   *
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   */
  private Landmark[] getMarks (int command)
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException
  {
    // senden und empfangen
    TelegramReply reply = send (new TelegramRequest (command, 0));

     // Z�hler f�r aktuelles Datum
    int slot = 0;

    // Anzahl der Landmarken
    int iNumberOfMarks = Protocol.ensureLimit (reply,
                                               Protocol.MARKS,
                                               ++slot,
                                               MARKS_LIMIT);

    Landmark[] landmark = new Landmark[iNumberOfMarks];

    Landmark mark;

    int id, x, y, angle, range;

    // Alle Landmarken aus Telegramm lesen
    for (int n = 0; n < iNumberOfMarks; ++n)
    {
      id = reply.getInteger (++slot);
      x = (int) Math.round (1000.0 * reply.getFloat (++slot));
      y = (int) Math.round (1000.0 * reply.getFloat (++slot));
      angle = Robiant.rad2robiant (reply.getFloat (++slot));
      range = Robiant.rad2robiant (reply.getFloat (++slot));

      // neue Landmarke
      mark = new Landmark (id, new Coord (x, y), angle, range);

      landmark[n] = mark;

      if (out.module)
      {
        out.module (this, "getAllMarks - " + mark);
      }
    }

    return landmark;
  }

  /**
   * Liefert die von der Robotersteuerung <tt>mobiled</tt> f&uuml;r die
   * Selbst-Lokalisierung verwendeten Positionen der Reflektormarken.
   *
   * @return Liste der Reflektormarken.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized Landmark[] getAllMarks ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    return this.getMarks (Commands.GET_ALL_MARKS);
  }

  /**
   * Liefert die von der Robotersteuerung <tt>mobiled</tt> f&uuml;r die
   * Selbst-Lokalisierung verwendeten initialen Positionen der Reflektormarken.
   *
   * @return Lister der Reflektormarken.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized Landmark[] getAllMarksInitial ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    return this.getMarks (Commands.GET_ALL_MARKS_INITIAL);
  }

  /**
   * Liefert die Skalierung der Geschwindigkeit für die
   * Translations- und Rotationsbewegungen des Roboters.
   *
   * @return float[] Skalierung für Translation und Rotation (2 Werte)
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized float[] getScale ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // senden und empfangen
    TelegramReply reply = send (new TelegramRequest (Commands.GET_SCALE, 0));

    // Speicher für Rotationsskalierung und Translationsskalierung
    float[] scales = new float[2];

    // Skalierungen auslesen
    scales[0] = reply.getFloat (1);
    scales[1] = reply.getFloat (2);

    if (out.module)
    {
      out.module (this,
                  "Skalierungen der Geschwindigkeiten: Translation="
                  + scales[0]
                  + " Rotation="
                  + scales[1]);
    }

    return scales;
  }

  /**
   * Setzt die Skalierung der Geschwindigkeit für die
   * Translations- und Rotationsbewegungen des Roboters.
   *
   * @param translationScale Skalierung f&uuml;r die Translationsgeschwindigkeit
   *                         (>0).
   * @param rotationScale Skalierung f&uuml;r die Rotationsgeschwindigkeit
   *                      (>0).
   *
   * @return float[] vorherige Skalierung für Translation und Rotation
   *                 (enthält 2 Werte)
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist oder die Skalierungen
   *                           kleiner 0 sind.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized float[] setScale (float translationScale,
                                        float rotationScale)
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    if (out.module)
    {
      out.module (this,
                  "Setze neue Skalierungen: Translation="
                  + translationScale
                  + " Rotation="
                  + rotationScale);
    }

    // teste ob die Skalierungen größer 0.0 sind
    if (!((0.0 < translationScale) && (0.0 < rotationScale)))
    {
      throw new ProtocolException
          ("Neue Skalierungen m\u00FCssen gr\u00F6\u00DFer 0.0 sein!");
    }

    // Telegramm, das gesendet wird.
    TelegramRequest request = new TelegramRequest (Commands.SET_SCALE, 2);

    request.putFloat (0, translationScale);
    request.putFloat (1, rotationScale);

    // senden und empfangen
    TelegramReply reply = send (request);

    // Speicher für vorherige Rotationsskalierung und Translationsskalierung
    float[] scales = new float[2];

    // vorherige Skalierungen auslesen
    scales[0] = reply.getFloat (0);
    scales[1] = reply.getFloat (1);

    return scales;
  }

  /**
   * Multipliziert die Skalierung der Geschwindigkeit für die
   * Translations- und Rotationsbewegungen des Roboters mit den &uuml;bergebenen
   * Werten.
   *
   * @param translationScale Faktor f&uuml;r Skalierung f&uuml;r die
   *                         Translationsgeschwindigkeit (>0).
   * @param rotationScale Faktor f&uuml;r die Skalierung f&uuml;r die
   *                      Rotationsgeschwindigkeit (>0).
   *
   * @return float[] vorherige Skalierung für Translation und Rotation
   *                 (enthält 2 Werte)
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist oder die Faktoren
   *                           kleiner 0 sind.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized float[] modifyScale (float translationScaleFactor,
                                   float rotationScaleFactor)
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // teste ob die Skalierungsfaktor größer 0.0 sind
    if (!((0.0 < translationScaleFactor) && (0.0 < rotationScaleFactor)))
    {
      throw new ProtocolException
          ("Skalierungsfaktoren nicht gr\u00F6\u00DFer 0.0.");
    }

    // Telegramm, das gesendet wird.
    TelegramRequest request = new TelegramRequest (Commands.MODIFY_SCALE, 2);

    request.putFloat (0, translationScaleFactor);
    request.putFloat (1, rotationScaleFactor);

    // senden und empfangen
    TelegramReply reply = send (request);

    // Speicher für vorherige Rotationsskalierung und Translationsskalierung
    float[] scales = new float[2];

    // vorherige Skalierungen auslesen
    scales[0] = reply.getFloat (0);
    scales[1] = reply.getFloat (1);

    return scales;
  }

  /**
   * Startet die Aufzeichnung der Odometriedaten auf dem Roboter.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized void startOdoLogging ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // senden und empfangen
    send (new TelegramRequest (Commands.START_ODO_LOGGING, 0));
  }

  /**
   * H&auml;lt die Aufzeichnung der Odometriedaten auf dem Roboter an.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized void stopOdoLogging ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // senden und empfangen
    send (new TelegramRequest (Commands.STOP_ODO_LOGGING, 0));
  }

  /**
   * Liefert die Spannung der Onboard-Batterie des Roboters.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized double getBatteryVoltage ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // senden und empfangen
    TelegramReply reply
        = send (new TelegramRequest (Commands.GET_BATTERY_VOLTAGE, 0));

    // gib die Spannung zurück
    return reply.getFloat (1);
  }

  /**
   * Liefert die Temperaturen der Antriebsmotoren.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized double[] getDriveTemperatures ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // senden und empfangen
    TelegramReply reply
        = send (new TelegramRequest (Commands.GET_DRIVE_TEMPERATURES, 0));

    double[] temperature = new double[2];

    temperature[0] = reply.getFloat (1);
    temperature[1] = reply.getFloat (2);

    // gib die Temperaturen zurück
    return temperature;
  }

  /**
   * Liefert <tt>true</tt>, wenn der Roboter blockiert ist.
   *
   * @return <tt>true</tt>, wenn der Roboter blockiert ist, sonst
   *         <tt>false</tt>
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized boolean isStalled ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // senden und empfangen
    TelegramReply reply = send (new TelegramRequest (Commands.STALLED, 0));

    int stalled = reply.getInteger (1);

    if (stalled == 0)
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  /**
   * Zieht die Bremsen der Antriebsmotoren des Roboters an, der Roboter
   * kann dann <b>keine</b> Bewegung ausf&uuml;hren.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized void applyBrakes ()
      throws ProtocolException,
             SocketCommunicationException,
             UnknownCommandException
  {
    // senden und empfangen
    send (new TelegramRequest (Commands.APPLY_BRAKES, 0));
  }

  /**
   * L&ouml;st die Bremsen der Antriebsmotoren des Roboters, der Roboter
   * kann Bewegungen ausf&uuml;hren.
   *
   * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   */
  synchronized void releaseBrakes ()
     throws ProtocolException,
            SocketCommunicationException,
            UnknownCommandException
 {
   // senden und empfangen
   send (new TelegramRequest (Commands.RELEASE_BRAKES, 0));
 }

 /**
  * Liefert <tt>true</tt>, wenn die Bremsen des Roboters gel&ouml;st sind.
  *
  * @return <tt>true</tt>, wenn die Bremsen des Roboters gel&ouml;st sind,
  *         sonst <tt>false</tt>
  *
  * @throws UnknownCommandException Wird ausgelöst, wenn das Kommando nicht
  *                                 von der Robotersteuerung <tt>mobiled</tt>
  *                                 implementiert wird.
  * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
  *                           nicht zul&auml;ssig ist.
  * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
  *                                      Socket-Kommunikation mit der
  *                                      Robotersteuerung <tt>mobiled</tt>
  *                                      ein Fehler aufgetreten ist.
  */
 synchronized boolean areBrakesReleased ()
     throws ProtocolException,
            SocketCommunicationException,
            UnknownCommandException
 {
   // senden und empfangen
   TelegramReply reply
       = send (new TelegramRequest (Commands.ARE_BRAKES_RELEASED, 0));

   int released = reply.getInteger (1);

   if (released == 0)
   {
     return false;
   }
   else
   {
     return true;
   }
 }
}
