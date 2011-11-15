package uhh.fbi.tams.mobilerobot.client;

import java.io.Serializable;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.rmi.RemoteException;

import org.roblet.Roblet;
import org.roblet.Robot;
import genRob.genControl.unit.RemoteExport;
import genRob.genControl.unit.log.Logger2;
import uhh.fbi.tams.mobilerobot.unit.*;
import uhh.fbi.tams.mobilerobot.unit.localization.Localization;
import uhh.fbi.tams.mobilerobot.unit.motion.*;
import uhh.fbi.tams.mobilerobot.unit.sensor.Battery;
import uhh.fbi.tams.mobilerobot.unit.sensor.Laserscanner;
import uhh.fbi.tams.mobilerobot.unit.Laserscan;

class RemoteRobotImpl
    implements RemoteRobot, Roblet, Serializable, Runnable
{
  private static final long serialVersionUID = -2382170605571815071L;

  /** Client-Name. */
  private final String CLIENT;

  /** Client-Anwendung. */
  private final ClientApplication CLIENT_APP;

  /** Zugriff auf den Roblet-Server. */
  private Robot robot;

  /** Thread, der dafür sorgt, dass das Roblet nicht beendet wird. */
  transient private Thread thread;

  /** Einheiten auf dem Roboter. */
  private Access access;
  private Brakes brakes;
  private Motion motion;
  private Velocity velocity;
  private Collision collision;
  private Localization loc;
  private Battery battery;
  private Destination destination;
  private Laserscanner scanner;
  private Logger2 logger;

  /** Anzahl der Laserscanner. */
  private int numOfScanners;
  
  /** Konstruktor. */
  RemoteRobotImpl (ClientApplication ca)
      throws ClientException, RemoteException
  {
    // Name des Client-Computers
    try
    {
      CLIENT = InetAddress.getLocalHost ().getHostName ();
    }
    catch (UnknownHostException ex)
    {
      throw new ClientException (I18N.EXCEPTION_HOST, ex);
    }

    CLIENT_APP = ca;
  }

  /** Eintrag ins Logbuch vornehmen. */
  private void log (String text)
  {
    if (logger != null)
    {
      logger.log (text);
    }
  }

  // RemoteRobot
  public void init ()
      throws ClientException
  {
    logger = (Logger2) robot.getUnit (Logger2.class);

    log (I18N.INIT + " (Client: " + CLIENT + ").");

    access = (Access) robot.getUnit (Access.class);

    if (access == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_ACCESS);
    }

    log (I18N.ACCESS);

    brakes = (Brakes) robot.getUnit (Brakes.class);

    if (brakes == null)
    {
      throw new ClientException (I18N.EXEPTIION_NO_UNIT_BRAKES);
    }

    log (I18N.BRAKES);

    motion = (Motion) robot.getUnit (Motion.class);

    if (motion == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_MOTION);
    }

    log (I18N.MOTION);

    velocity = (Velocity) robot.getUnit (Velocity.class);

    if (velocity == null)
    {
//      throw new ClientException (I18N.EXCEPTION_NO_UNIT_VELOCITY);
      log (I18N.EXCEPTION_NO_UNIT_VELOCITY);
    }
    else
    {
      log (I18N.VELOCITY);
    }
    
    collision = (Collision) robot.getUnit (Collision.class);

    if (collision == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_COLLISION);
    }

    log (I18N.COLLISION);

    loc = (Localization) robot.getUnit (Localization.class);

    if (loc == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_LOCALIZATION);
    }

    log (I18N.LOCALIZATION);

    battery = (Battery) robot.getUnit (Battery.class);

    if (battery == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_BATTERY);
    }

    log (I18N.BATTERY);

    destination = (Destination) robot.getUnit (Destination.class);

    if (destination == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_DESTINATION);
    }

    log (I18N.DESTINATION);

    scanner = (Laserscanner) robot.getUnit (Laserscanner.class);

    if (scanner == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_LASERSCANNER);
    }

    try
    {
      numOfScanners = scanner.getNumScanners ();      
    }
    catch (Exception e)
    {
      throw new ClientException (e);
    }

    log (I18N.LASERSCANNER);

    log (I18N.INIT_COMPLETED);
  }

  // RemoteRobot
  public void end ()
  {
    thread.interrupt ();
  }

  // RemoteRobot
  public void setRotationalVelocityScale (float scale) 
    throws ClientException
  {
    if (access == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_ACCESS);
    }

    if (!access.getAccess ())
    {
      throw new ClientException (I18N.EXCEPTION_ACCESS);
    }

    if (velocity == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_VELOCITY);
    }
    
    try
    {
      try
      {
        velocity.setRotationScale (scale);
      }
      catch (UnknownCommandException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_UNKNOWN_COMMAND, ex1);
      }
      catch (ProtocolException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_PROTOKOLL, ex1);
      }
      catch (SocketCommunicationException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_SOCKET, ex1);
      }
      catch (AccessException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_ACCESS_DENIED, ex1);
      }
    }
    finally
    {
      access.releaseAccess ();
    }
  }
  
  // RemoteRobot
  public void setTranslationalVelocityScale (float scale)
    throws ClientException
  {
    if (access == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_ACCESS);
    }

    if (!access.getAccess ())
    {
      throw new ClientException (I18N.EXCEPTION_ACCESS);
    }
    
    if (velocity == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_VELOCITY);
    }

    try
    {
      try
      {
        velocity.setTranslationScale (scale);
      }
      catch (UnknownCommandException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_UNKNOWN_COMMAND, ex1);
      }
      catch (ProtocolException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_PROTOKOLL, ex1);
      }
      catch (SocketCommunicationException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_SOCKET, ex1);
      }
      catch (AccessException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_ACCESS_DENIED, ex1);
      }
    }
    finally
    {
      access.releaseAccess ();
    }
  }
  
  
  // RemoteRobot
  public void move (Pose pose)
      throws ClientException
  {
    if (access == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_ACCESS);
    }

    if (!access.getAccess ())
    {
      throw new ClientException (I18N.EXCEPTION_ACCESS);
    }

    try
    {
      try
      {
        motion.move (pose);
      }
      catch (UnknownCommandException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_UNKNOWN_COMMAND, ex1);
      }
      catch (ProtocolException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_PROTOKOLL, ex1);
      }
      catch (SocketCommunicationException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_SOCKET, ex1);
      }
      catch (InternalPathplannerException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_PATH, ex1);
      }
      catch (BrakesOnException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_BRAKES_ON, ex1);
      }
      catch (AccessException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_ACCESS_DENIED, ex1);
      }

      log (I18N.DRIVE_TO_POSE + pose + ".");
    }
    finally
    {
      access.releaseAccess ();
    }
  }

  // RemoteRobot
  public void stop ()
      throws ClientException, RemoteException
  {
    if (access == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_ACCESS);
    }

    if (!access.getAccess ())
    {
      throw new ClientException
          (I18N.EXCEPTION_ACCESS);
    }

    try
    {
      motion.stop ();
    }
    catch (UnknownCommandException ex)
    {
      throw new ClientException (I18N.EXCEPTION_STOP + " "
                                 + I18N.EXCEPTION_UNKNOWN_COMMAND, ex);
    }
    catch (ProtocolException ex)
    {
      throw new ClientException (I18N.EXCEPTION_STOP + " "
                                 + I18N.EXCEPTION_PROTOKOLL, ex);
    }
    catch (SocketCommunicationException ex)
    {
      throw new ClientException (I18N.EXCEPTION_STOP + " "
                                 + I18N.EXCEPTION_SOCKET, ex);
    }
    catch (uhh.fbi.tams.mobilerobot.unit.AccessException ex)
    {
      throw new ClientException (I18N.EXCEPTION_STOP + " "
                                 + I18N.EXCEPTION_ACCESS_DENIED, ex);
    }
    finally
    {
      access.releaseAccess ();
    }
  }

  // RemoteRobot
  public boolean isStalled ()
      throws ClientException, RemoteException
  {
    try
    {
      return collision.isStalled ();
    }
    catch (UnknownCommandException ex)
    {
      throw new ClientException (I18N.EXCEPTION_IS_STALLED + " "
                                 + I18N.EXCEPTION_UNKNOWN_COMMAND, ex);
    }
    catch (ProtocolException ex)
    {
      throw new ClientException (I18N.EXCEPTION_IS_STALLED + " "
                                 + I18N.EXCEPTION_PROTOKOLL, ex);
    }
    catch (SocketCommunicationException ex)
    {
      throw new ClientException (I18N.EXCEPTION_IS_STALLED + " "
                                 + I18N.EXCEPTION_SOCKET, ex);
    }
  }

  // RemoteRobot
  public Pose getPose ()
      throws ClientException
  {
    log (I18N.GET_POSE);

    try
    {
      return loc.getPose ();
    }
    catch (UnknownCommandException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_POSE + " "
                                 + I18N.EXCEPTION_UNKNOWN_COMMAND, ex);
    }
    catch (ProtocolException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_POSE + " "
                                 + I18N.EXCEPTION_PROTOKOLL, ex);
    }
    catch (SocketCommunicationException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_POSE + " "
                                 + I18N.EXCEPTION_SOCKET, ex);
    }
  }

  // RemoteRobot
  public double getVoltage ()
      throws ClientException, RemoteException
  {
    try
    {
      return battery.getVoltage ();
    }
    catch (UnknownCommandException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_VOLTAGE + " "
                                 + I18N.EXCEPTION_UNKNOWN_COMMAND, ex);
    }
    catch (ProtocolException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_VOLTAGE + " "
                                 + I18N.EXCEPTION_PROTOKOLL, ex);
    }
    catch (SocketCommunicationException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_VOLTAGE + " "
                                 + I18N.EXCEPTION_SOCKET, ex);
    }
  }

  // RemoteRobot
  public double getLowVoltage ()
      throws ClientException, RemoteException
  {
    return battery.getLowVoltage ();
  }

  // RemoteRobot
  public double getMinVoltage ()
      throws ClientException, RemoteException
  {
    return battery.getMinVoltage ();
  }

  // RMIRoblet
  /**
   * Ein mal pro Sekunde Verbindung mit der Clientapplikation aufnehmen.
   * Ist diese nicht mehr verfügbar, wird eine Ausnahme ausgelöst, der
   * Thread und anschließend das Roblet beendet.
   */
  protected void doInThread ()
      throws Exception
  {
    while (true) // Wird durch eine Ausnahme beendet
    {
      CLIENT_APP.ping ();

      Thread.sleep (1000);
    }
  }

  // RemoteRobot
  public void turn (double angle)
      throws ClientException
  {
    if (access == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_ACCESS);
    }

    if (!access.getAccess ())
    {
      throw new ClientException
          (I18N.EXCEPTION_ACCESS);
    }

    try
    {
      try
      {
        // Aktuelle Pose bestimmen
        final Pose pose = loc.getPose ();

        // Winkel zur Orientierung hinzuaddieren
        final int theta = pose.theta + Robiant.rad2robiant (angle);

        log ("Drehe auf: " + Robiant.robiant2degree (theta));

        // Drehbewegung starten
        motion.turn (theta);
      }
      catch (UnknownCommandException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_UNKNOWN_COMMAND, ex1);
      }
      catch (ProtocolException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_PROTOKOLL, ex1);
      }
      catch (SocketCommunicationException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_SOCKET, ex1);
      }
      catch (BrakesOnException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_BRAKES_ON, ex1);
      }
      catch (AccessException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_ACCESS_DENIED, ex1);
      }
    }
    finally
    {
      access.releaseAccess ();
    }
  }

  // RemoteRobot
  public void setBrakes (boolean set)
      throws ClientException
  {
    if (!access.getAccess ())
    {
      throw new ClientException
          (I18N.EXCEPTION_ACCESS);
    }

    try
    {
      if (set)
      {
        if (!brakes.areBrakesReleased ())
        {
          brakes.applyBrakes ();

          log (I18N.BRAKES_APPLIED);
        }
      }
      else
      {
        if (!brakes.areBrakesReleased ())
        {
          brakes.releaseBrakes ();

          log (I18N.BRAKES_RELEASED);
        }
      }
    }
    catch (UnknownCommandException ex3)
    {
      throw new ClientException (I18N.EXCEPTION_APPLY_BRAKES + " "
                                 + I18N.EXCEPTION_UNKNOWN_COMMAND, ex3);
    }
    catch (ProtocolException ex3)
    {
      throw new ClientException (I18N.EXCEPTION_APPLY_BRAKES + " "
                                 + I18N.EXCEPTION_PROTOKOLL, ex3);
    }
    catch (SocketCommunicationException ex3)
    {
      throw new ClientException (I18N.EXCEPTION_APPLY_BRAKES + " "
                                 + I18N.EXCEPTION_SOCKET, ex3);
    }
    catch (AccessException ex3)
    {
      throw new ClientException (I18N.EXCEPTION_APPLY_BRAKES + " "
                                 + I18N.EXCEPTION_ACCESS_DENIED, ex3);
    }
    finally
    {
      access.releaseAccess();
    }

  }

  // RemoteRobot
  public void forward (int millimeter)
      throws ClientException
  {
    if (access == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_ACCESS);
    }

    if (!access.getAccess ())
    {
      throw new ClientException
          (I18N.EXCEPTION_ACCESS);
    }

    try
    {
      try
      {
        log ("Fahre: " + millimeter + "mm");

        // Bewegung starten
        motion.forward (millimeter);
      }
      catch (UnknownCommandException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_UNKNOWN_COMMAND, ex1);
      }
      catch (ProtocolException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_PROTOKOLL, ex1);
      }
      catch (SocketCommunicationException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_SOCKET, ex1);
      }
      catch (BrakesOnException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_BRAKES_ON, ex1);
      }
      catch (AccessException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_ROTATE + " "
                                   + I18N.EXCEPTION_ACCESS_DENIED, ex1);
      }
    }
    finally
    {
      access.releaseAccess ();
    }

  }

  // RemoteRobot
  public boolean isCompleted ()
      throws ClientException
  {
    try
    {
      boolean completed = motion.isCompleted ();

      log ("isCompleted() == " + completed);

      return completed;
    }
    catch (UnknownCommandException ex)
    {
      throw new ClientException (I18N.EXCEPTION_IS_COMPLETED + " "
                                 + I18N.EXCEPTION_UNKNOWN_COMMAND, ex);
    }
    catch (ProtocolException ex)
    {
      throw new ClientException (I18N.EXCEPTION_IS_COMPLETED + " "
                                 + I18N.EXCEPTION_PROTOKOLL, ex);
    }
    catch (SocketCommunicationException ex)
    {
      throw new ClientException (I18N.EXCEPTION_IS_COMPLETED + " "
                                 + I18N.EXCEPTION_SOCKET, ex);
    }
  }

  // RemoteRobot
  public void waitForCompleted ()
      throws ClientException
  {
    try
    {
      log (I18N.WAIT_FOR_COMPLETED);

      motion.waitForCompleted ();

      log (I18N.MOTION_COMPLETED);
    }
    catch (UnknownCommandException ex)
    {
      throw new ClientException (I18N.EXCEPTION_WAIT_FOR_COMPLETED + " "
                                 + I18N.EXCEPTION_UNKNOWN_COMMAND, ex);
    }
    catch (ProtocolException ex)
    {
      throw new ClientException (I18N.EXCEPTION_WAIT_FOR_COMPLETED + " "
                                 + I18N.EXCEPTION_PROTOKOLL, ex);
    }
    catch (SocketCommunicationException ex)
    {
      throw new ClientException (I18N.EXCEPTION_WAIT_FOR_COMPLETED + " "
                                 + I18N.EXCEPTION_SOCKET, ex);
    }
  }

  // RemoteRobot
  public double getCollisionRadius ()
      throws RemoteException
  {
    log (I18N.GET_COLLISION_RADIUS);

    return collision.getCollisionRadius () * 0.001;
  }

  // RemoteRobot
  public Pose getDestination ()
      throws ClientException, RemoteException
  {
    try
    {
      log (I18N.GET_DESTINATION);

      return destination.getDestination ();
    }
    catch (UnknownCommandException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_DESTINATION + " "
                                 + I18N.EXCEPTION_UNKNOWN_COMMAND, ex);
    }
    catch (ProtocolException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_DESTINATION + " "
                                 + I18N.EXCEPTION_PROTOKOLL, ex);
    }
    catch (SocketCommunicationException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_DESTINATION + " "
                                 + I18N.EXCEPTION_SOCKET, ex);
    }
  }

  // Roblet
  public Object execute (Robot robot)
      throws Exception
  {
    System.out.println ("RemoteRobotImpl: START");
    try
    {

      this.robot = robot;

      // RMI-Schnittstelle exportieren
      RemoteExport re = (RemoteExport) robot.getUnit (RemoteExport.class);

      if (re == null)
      {
        throw new ClientException (I18N.EXCEPTION_NO_UNIT_REMOTEEXPORT);
      }

      re.exportRemote (this);

      // Thread starten, damit das Roblet nich nach dem
      // Ende der execute-Methode vom Roblet-Server aus
      // seinem Slot entfernt wird
      thread = new Thread (this);
      thread.setDaemon (true);
      thread.setPriority (1);
      thread.start ();

      // RMI-Interface zurückgeben
      return this;
    }
    finally
    {
      System.out.println ("RemoteRobotImpl: execute () beendet.");
    }
  }

  // Runnable
  public void run ()
  {
    System.out.println ("RemoteRobotImpl: Thread läuft.");
    try
    {
      while (true)
      {
        Thread.sleep (2000);

        CLIENT_APP.ping ();
      }
    }
    catch (InterruptedException ex)
    {
      // Nichts tun, Thread wird nur beendet
    }
    catch (RemoteException re)
    {
      System.out.println ("RemoteRobotImpl: Verbindung verloren.");
    }
    System.out.println ("RemoteRobotImpl: Thread beendet.");
  }

  // RemoteRobot
  public Laserscan[] getLaserscan ()
      throws ClientException, RemoteException
  {
    try
    {
      log (I18N.GET_LASERSCAN);

      final Laserscan[] scan = new Laserscan[numOfScanners];
      
      for (int i=0; i<numOfScanners; i++)
      {
        scan[i] = scanner.getScanPlatform (i);
        
        // TODO System.out entfernen
        System.out.println ("scan-size: " + scan[i].getScan ().length);
      }
      
      return scan;
    }
    catch (UnknownCommandException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_DESTINATION + " "
                                   + I18N.EXCEPTION_UNKNOWN_COMMAND, ex);
    }
    catch (ProtocolException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_DESTINATION + " "
                                  + I18N.EXCEPTION_PROTOKOLL, ex);
    }
    catch (SocketCommunicationException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_DESTINATION + " "
                                  + I18N.EXCEPTION_SOCKET, ex);
    }
  }

  // RemoteRobot
  public void rotate (double orientation)
    throws ClientException, RemoteException
  {
    if (access == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_ACCESS);
    }

    if (!access.getAccess ())
    {
      throw new ClientException
          (I18N.EXCEPTION_ACCESS);
    }

    try
    {
      try
      {
        motion.rotate (Robiant.rad2robiant (orientation));
      }
      catch (UnknownCommandException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_UNKNOWN_COMMAND, ex1);
      }
      catch (ProtocolException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_PROTOKOLL, ex1);
      }
      catch (SocketCommunicationException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_SOCKET, ex1);
      }
      catch (BrakesOnException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_BRAKES_ON, ex1);
      }
      catch (AccessException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_ACCESS_DENIED, ex1);
      }

      log (I18N.ROTATE_TO_ORIENTATION);
    }
    finally
    {
      access.releaseAccess ();
    }
  }

  // RemoteRobot
  public void translate (Pose pose) 
    throws ClientException, RemoteException
  {
    if (access == null)
    {
      throw new ClientException (I18N.EXCEPTION_NO_UNIT_ACCESS);
    }

    if (!access.getAccess ())
    {
      throw new ClientException
          (I18N.EXCEPTION_ACCESS);
    }

    try
    {
      try
      {
        motion.translate (pose);
      }
      catch (UnknownCommandException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_UNKNOWN_COMMAND, ex1);
      }
      catch (ProtocolException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_PROTOKOLL, ex1);
      }
      catch (SocketCommunicationException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_SOCKET, ex1);
      }
      catch (BrakesOnException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_BRAKES_ON, ex1);
      }
      catch (AccessException ex1)
      {
        throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                   + I18N.EXCEPTION_ACCESS_DENIED, ex1);
      }

      log (I18N.ROTATE_TO_ORIENTATION);
    }
    finally
    {
      access.releaseAccess ();
    }
  }

  // RemoteRobot
  public int getNumberOfLaserscanner () 
    throws RemoteException
  {
    log (I18N.GET_NUMBER_OF_LASERSCANNERS);

    return numOfScanners;
  }

  // RemoteRobot
  public Frame2D[] getLaserscannerPositions () 
    throws RemoteException, ClientException
  {
    log (I18N.GET_POSITIONS_OF_LASERSCANNERS);

    try
    {
      final Frame2D[] pos = new Frame2D [numOfScanners];
      
      for (int i=0; i < numOfScanners; i++)
      {
          pos[i] = scanner.getScannerPosition (i);
      }
          
      return pos;
    }
    catch (UnknownCommandException ex1)
    {
      throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                 + I18N.EXCEPTION_UNKNOWN_COMMAND, ex1);
    }
    catch (ProtocolException ex1)
    {
      throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                 + I18N.EXCEPTION_PROTOKOLL, ex1);
    }
    catch (SocketCommunicationException ex1)
    {
      throw new ClientException (I18N.EXCEPTION_MOTION + " "
                                 + I18N.EXCEPTION_SOCKET, ex1);
    }
  }

  // RemoteRobot
  public Object[] getRawLascans () 
    throws ClientException, RemoteException
  {
    try
    {
      log (I18N.GET_RAW_LASERSCAN);

      final Object[] scan = new Object[numOfScanners];
      
      for (int i=0; i<numOfScanners; i++)
      {
        // ANMERKUNG Leider enthalten die Scans zurzeit nicht immer 361 Messungen, Grund nicht bekannt.
        scan[i] = scanner.getScanRadial (i);
      }
      
      return scan;
    }
    catch (UnknownCommandException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_DESTINATION + " "
                                   + I18N.EXCEPTION_UNKNOWN_COMMAND, ex);
    }
    catch (ProtocolException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_DESTINATION + " "
                                  + I18N.EXCEPTION_PROTOKOLL, ex);
    }
    catch (SocketCommunicationException ex)
    {
      throw new ClientException (I18N.EXCEPTION_GET_DESTINATION + " "
                                  + I18N.EXCEPTION_SOCKET, ex);
    }
  }
}
