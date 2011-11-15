package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;
import genRob.genControl.client.Client;
import uhh.fbi.tams.mobilerobot.unit.sensor.DriveTemperature;
import org.roblet.Roblet;
import org.roblet.Robot;


/**
 * Gibt die Temperaturen der Antriebsmotoren des Serviceroboters des
 * AB TAMS aus. Der Roblet&reg;-Server muss auf <b>Port 6002</b> laufen.
 */
class DriveTemperatureTest
    implements Roblet, Serializable
{

  /** Startet die Anwendung. */
  public static void main (String[] args)
      throws Exception
  {
    Client client = new Client ();

    try
    {
      System.out.println ("Sende Roblet an tams59.informatik.uni-hamburg.de!");

      double[] temperature
          = (double[]) client.run ("tams59:6002", new DriveTemperatureTest ());

      System.out.println ("Temperaturen (links/rechts): "
                          + temperature[0]
                          + " \u00B0C\t"
                          + temperature[1]
                          + " \u00B0C");
    }
    finally
    {
      client.close ();
    }

    System.out.println ("Fertig!");
  }

  /** List die Temperaturen der Antriebsmotoren auf dem Roboter aus. */
  public Object execute (Robot robot)
      throws Exception
  {
    DriveTemperature temperature
        = (DriveTemperature) robot.getUnit (DriveTemperature.class);

    if (temperature == null)
      throw new Exception ("Einheit <DriveTemperature> nicht gefunden!");

    return temperature.getTemperatures();
  }
}
