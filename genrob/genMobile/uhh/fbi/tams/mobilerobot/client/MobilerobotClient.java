package uhh.fbi.tams.mobilerobot.client;

import genRob.genControl.client.*;
import java.rmi.*;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.List;

import uhh.fbi.tams.mobilerobot.unit.*;

/** Zugriff auf einen mobilen Roboter wie den Serviceroboter TASER des AB TAMS. */
public class MobilerobotClient
    implements ClientApplication
{
  /** Zugiff auf die Roblet-server. */
  private final Client CLIENT;

  /** Roboter. */
  private final String ROBOT;

  /** Roblet. */
  private RemoteRobot remoteRobot;

  /**
   * Konstruktor.
   *
   * @param robot IP-Adresse oder DNS-Name des Roboters plus Port ("tams59:6002")
   */
  public MobilerobotClient (Client client, String robot)
  {
    this.ROBOT = robot;
    this.CLIENT = client;
  }

  /** Verbindung zum Roboter initialisieren. */
  public void init ()
      throws ClientException
  {
    // RMI-Interface nach aussen präsentieren
    try
    {
      UnicastRemoteObject.exportObject (this);
    }
    catch (RemoteException ex)
    {
      new ClientException (I18N.EXCEPTION_REMOTE_EXPORT, ex);
    }

    // RMI-Roblet zum Roboter schicken
    try
    {
      remoteRobot = (RemoteRobot) CLIENT.run (ROBOT, new RemoteRobotImpl (this));
      remoteRobot.init ();
    }
    catch (Exception ex1)
    {
      throw new ClientException (I18N.EXCEPTION_ROBLET_START, ex1);
    }
  }

  /**
   * Verbindung zum Roboter beenden. Danach kann die Verbindung
   * nicht wieder neu initialisiert werden. Es muss ein neues
   * Objekt vom Typ MobilerobotClient erzeugt werden.  */
  public void close ()
      throws ClientException
  {
    ClientException e = null;

    if (remoteRobot != null)
    {
      try
      {
        remoteRobot.end ();
      }
      catch (RemoteException ex)
      {
        e = new ClientException (I18N.EXCEPTION_REMOTE, ex);
      }
    }

    // RMI beenden
    try
    {
      UnicastRemoteObject.unexportObject (this, true);
    }
    catch (NoSuchObjectException ex1)
    {
      throw new ClientException (I18N.EXCEPTION_RMI_UNEXPORT);
    }

    if (e != null) throw e;
  }

  /** Rotationsgeschwindigkeit anopassen. */
  public void setRotationalVelocityScale (float scale)
    throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.setRotationalVelocityScale (scale);
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }  
  }
  
  /** Translationsgeschwindigkeit anopassen. */
  public void setTranslationalVelocityScale (float scale)
    throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.setTranslationalVelocityScale (scale);
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }  
  }
  
  /** Roboter f&auml;hrt an die &uuml;bergebene Pose. */
  public void move (Pose pose)
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.move (pose);
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /** Liefert die Pose des Roboters. */
  public Pose getPose ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.getPose ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /** Liefert die Spannung der Akkus in <b>Volt</b>. */
  public double getVoltage ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.getVoltage ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /** Liefert den Wert in <b>Volt</b>, ab wann die Spannung der Akkus niedrig ist. */
  public double getLowVoltage ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.getLowVoltage ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /** Liefert den minimal erlaubten Wert f&uuml;r die Spannung der Akkus in <b>Volt</b>. */
  public double getMinVoltage ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.getMinVoltage ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /** Ist der Roboter blockiert? */
  public boolean isStalled ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.isStalled ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /** Roboter anhalten. */
  public void stop ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.stop ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /**
   * Roboter um angegebenen Wert in [mm] nach vorne bewegen.
   * Negativer Wert bewegt Roboter nach hinten.
   *
   * @param millimeter Strecke in [mm]
   */
  public void forward (int millimeter)
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.forward (millimeter);
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /**
   * Roboter um einen Winkel in <b>rad</b> drehen.
   *
   * @param angle Winkel in <b>rad</b>
   */
  public void turn (double angle)
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.turn (angle);
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }


  /**
   * Roboter dreht sich in die angegebene Orientierung.
   *
   * @param angle Winkel in <b>rad</b>
   */
  public void rotate (double angle)
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.rotate (angle);
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /**
   * Roboter geradeaus auf Pose fahren.
   *
   * @param pose Pose
   */
  public void translate (Pose pose)
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.translate (pose);
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  
  /**
   * Ist der Roboter mit der letzten Bewegung fertig?
   * @return <b>true</b>, wenn keine Bewegung ausgef&uuml;hrt wird, sonst <b>false</b>
   */
  public boolean isCompleted ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.isCompleted ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /**
   * Methode kehrt erst wieder zur&uuml;ck, wenn die Bewegung des
   * Roboters beendet ist.
   */
  public void waitForCompleted ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.waitForCompleted ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /** Bremsen anziehen/l&ouml;sen. */
  public void setBrakes (boolean set)
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      remoteRobot.setBrakes (set);
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /**
   * Liefert den Radius des Kreises um den Roboter-Mittelpunkt,
   * innerhalb dessen ein Hinderniss dazu f&uuml;hrt, dass der Roboter
   * seine Bewegung anh&auml;lt.
   * @return Radius in <b>Meter</b>.
   */
  public double getCollisionRadius ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.getCollisionRadius ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /**
   * Liefert das Ziel der aktuellen Bewegung, <i>null</i>, wenn keine
   * Bewegung ausgef&uuml;hrt wird.
   */
  public Pose getDestination ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.getDestination();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }

  /**
   * Liefert die Anzahl der Laserscanner.
   */
  public int getNumberOfLaserscanners () 
    throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.getNumberOfLaserscanner ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }
  
  /** 
   * Liefert die Positionen der Laserscanner in Roboterkoordinaten.
   * @throws ClientException 
   */
  public Frame2D[] getLaserscannerPositions () 
    throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      return remoteRobot.getLaserscannerPositions ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }
  
  /**
   * Liefert die aktuellen Entfernungsmessungen aller Laserscanner.
   * 
   * @return Feld mit int[] in denen die Entfernungsmessungen stehen.
   */
  public Object[] getRawLaserscans ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }
    
    try
    {
      return remoteRobot.getRawLascans ();
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }
  
  /**
   * Liefert einen aktuellen Laserscan in Roboterkoordinaten.
   */
  public Laserscan getLaserscan ()
      throws ClientException
  {
    if (remoteRobot == null)
    {
      throw new ClientException (I18N.EXCEPTION_INIT);
    }

    try
    {
      final Laserscan[] scan = remoteRobot.getLaserscan ();
      
      List list = new ArrayList ();
      
      for (int i=0; i<scan.length; i++)
      {
        final Point2D[] scanPoint = scan[i].getScan ();
        
        for (int p=0; p<scanPoint.length; p++)
        {
          list.add (scanPoint[p]);
        }
      }
      
      final Point2D[] points = (Point2D[]) list.toArray (new Point2D[list.size()]);
      
      list = new ArrayList ();
      
      for (int i=0; i<scan.length; i++)
      {
        final Point2D[] markPoint = scan[i].getMarks ();
        
        for (int p=0; p<markPoint.length; p++)
        {
          list.add (markPoint[p]);
        }
      }
      
      final Point2D[] marks = (Point2D[]) list.toArray (new Point2D[list.size()]);
      
      return new Laserscan (points, marks);
    }
    catch (RemoteException ex)
    {
      throw new ClientException (I18N.EXCEPTION_REMOTE, ex);
    }
  }
  
  // ClientApplication
  public void ping ()
      throws RemoteException
  {
    // nichts zu tun
  }
}
