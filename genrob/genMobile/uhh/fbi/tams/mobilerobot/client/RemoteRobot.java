package uhh.fbi.tams.mobilerobot.client;

import uhh.fbi.tams.mobilerobot.unit.Frame2D;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import java.rmi.RemoteException;
import java.rmi.Remote;
import uhh.fbi.tams.mobilerobot.unit.Laserscan;

interface RemoteRobot
    extends Remote
{
  /** Beendet das Roblet auf dem Roblet-Server.- */
  public void end ()
      throws RemoteException;

  /** Einheiten auf dem Roboter holen. */
  public void init ()
      throws ClientException, RemoteException;

  /** Rotationsgeschwindigkeit anopassen. */
  public void setRotationalVelocityScale (float scale)
    throws ClientException, RemoteException;
  
  /** Translationsgeschwindigkeit anopassen. */
  public void setTranslationalVelocityScale (float scale)
    throws ClientException, RemoteException;
  
  /** Roboter bewegen. */
  public void move (Pose pose)
      throws ClientException, RemoteException;

  /** Roboter drehen auf Orientierung. */
  public void rotate (double orientation)
    throws ClientException, RemoteException;
  
  /** Roboter geradeaus fahren auf Pose. */
  public void translate (Pose pose)
    throws ClientException, RemoteException;
    
  /** Roboter drehen. */
  public void turn (double angle)
      throws ClientException, RemoteException;

  /** Bewegt den Roboter vorwärts. */
  public void forward (int millimeter)
      throws ClientException, RemoteException;

  /** Roboter anhalten. */
  public void stop ()
      throws ClientException, RemoteException;

  /** Bremsen setzen/l&ouml;sen. */
  public void setBrakes (boolean set)
      throws ClientException, RemoteException;

  /** Ist die aktuelle Bewegung zuende? */
  public boolean isCompleted ()
      throws ClientException, RemoteException;

  /** Wartet auf das Ende der aktuellen Bewegung. */
  public void waitForCompleted ()
      throws ClientException, RemoteException;

  /** Roboter blockiert? */
  public boolean isStalled ()
      throws ClientException, RemoteException;

  /** Liefert die Pose des Roboters. */
  public Pose getPose ()
      throws ClientException, RemoteException;

  /** Liefert die Spannung der Akkus. */
  public double getVoltage ()
      throws ClientException, RemoteException;

  /** Liefert den Wert, ab wann die Spannung der Akkus niedrig ist. */
  public double getLowVoltage ()
      throws ClientException, RemoteException;

  /** Liefert den minimal erlaubten Wert für die Spannung der Akkus. */
  public double getMinVoltage ()
      throws ClientException, RemoteException;

  /**
   * Liefert den Radius des Kreises um den Roboter-Mittelpunkt,
   * innerhalb dessen ein Hinderniss dazu führt, dass der Roboter
   * seine Bewegung anhält.
   * @return Radius in <b>Meter</b>.
   */
  public double getCollisionRadius ()
      throws RemoteException;

  /**
   * Liefert das Ziel der aktuellen Bewegung, <i>null</i>, wenn keine
   * Bewegung ausgeführt wird.
   */
  public Pose getDestination ()
      throws ClientException, RemoteException;

  /**
   * Liefert die Anzahl der Laserscanner.
   */
  public int getNumberOfLaserscanner ()
      throws RemoteException;
  
  /** 
   * Liefert die Positionen der Laserscanner.
   */
  public Frame2D[] getLaserscannerPositions ()
      throws ClientException, RemoteException;
  
  /**
   * Liefert aktuelle Laserscans aller Laserscanner in Roboter-Koordinaten.
   */
  public Laserscan[] getLaserscan ()
    throws ClientException, RemoteException;

  /**
   * Liefert die Aktuellen Entfernungsmessungen jedes Laserscanners.
   * 
   * @return Feld mit int[] für jeden Laserscanner.
   */
  public Object[] getRawLascans ()
    throws ClientException, RemoteException;
}