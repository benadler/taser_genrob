package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;

import genRob.genControl.client.Client;
import uhh.fbi.tams.mobilerobot.unit.sensor.OdometryLog;
import org.roblet.Roblet;
import org.roblet.Robot;

/**
 * Testet das Loggen der Odometrie auf dem Serviceroboter des AB TAMS.
 * Der Roblet&reg;-Server muss auf dem Roboter auf <b>Port 6002</b> laufen.
 */
class OdoLoggingTest
    implements Roblet, Serializable
{

  /** Startet die Anwendung. */
  public static void main (String[] args)
      throws Exception
  {
    Client client = new Client ();

    try
    {
      System.out.println ("Sende Roblet!");

      client.run ("tams59:6002", new OdoLoggingTest ());
    }
    finally
    {
      client.close ();
    }

    System.out.println ("Fertig!");
  }


  /**
   * Startet auf dem Roboter das Loggen der Odometrie, wartet f&uuml;nf
   * Sekunden und h&auml;t dann das Loggen wieder an.
   */
  public Object execute (Robot robot)
      throws Exception
  {
    OdometryLog odoLog = (OdometryLog) robot.getUnit (OdometryLog.class);

    if (odoLog == null)
      throw new Exception ("Einheit <OdometryLog> nicht gefunden!");

    odoLog.startLogging();

    Thread.sleep (5000);  // 5 Sekunden

    odoLog.stopLogging();

    return null;
  }
}
