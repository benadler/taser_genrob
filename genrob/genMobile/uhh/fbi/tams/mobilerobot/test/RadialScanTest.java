package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;
import genRob.genControl.client.Client;
import uhh.fbi.tams.mobilerobot.unit.sensor.Laserscanner;
import org.roblet.Roblet;
import org.roblet.Robot;

/**
 * Gibt einen Laserscan des ersten angeschlossenen Laserscanners in
 * Polar-Koordinaten aus. Der Roblet&reg;-Server auf dem Roboter muss
 * auf <b>Port 6002</b> laufen. Es muss mindestens ein Laserscanner
 * angeschlossen sein.
 */
class RadialScanTest
    implements Roblet, Serializable
{
  /** Startet ie Anwendung. */
  public static void main (String[] args)
      throws Exception
  {
    Client c = new Client ();

    try
    {
      int[] scan = (int[]) c.run ("tams59:6002", new RadialScanTest ());

      for (int i = 0; i < scan.length; ++i)
        System.out.println ("(" + (i * 0.5) + "\u00B0, " + scan[i] + "mm)");
    }
    finally
    {
      c.close ();
    }

    System.out.println ("FERTIG.");
  }

  /** Liest einen Laserscan vom ersten Laserscanner in Polar-Koordinaten aus. */
  public Object execute (Robot robot)
      throws Exception
  {
    Laserscanner scanner = (Laserscanner) robot.getUnit (Laserscanner.class);

    if (scanner == null)
    {
      throw new Exception ("Einheit <Laserscanner> nicht gefunden.");
    }

    return scanner.getScanRadial(0);
  }
}
