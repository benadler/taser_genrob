package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;

import genRob.genControl.client.Client;

import uhh.fbi.tams.mobilerobot.unit.sensor.Battery;
import org.roblet.Roblet;
import org.roblet.Robot;


/**
 * Gibt die Spannung der Batterien des Serviceroboters des AB TAMS aus.
 * Der Roblet&reg;-Server muss auf dem Roboter auf <b>Port 6002</b> laufen.
 */
class BatteryVoltageTest
    implements Roblet, Serializable
{

  /** Startet die Anwendung. */
  public static void main (String[] args)
      throws Exception
  {
    Client client = new Client ();

    try
    {
      System.out.println
          ("Sende Roblet an tams59.informatik.uni-hamburg.de:6002.");

      Double voltage
          = (Double) client.run ("tams59:6002", new BatteryVoltageTest ());

      System.out.println ("Spannung der Batterien: "
                          + voltage
                          + " Volt");
    }
    finally
    {
      client.close ();
    }

    System.out.println ("Fertig!");
  }


  /** Liest auf dem Roboter die Batteriespannung aus. */
  public Object execute (Robot robot)
      throws Exception
  {
    Battery battery = (Battery) robot.getUnit (Battery.class);

    if (battery == null)
      throw new Exception ("Einheit <Battery> nicht gefunden!");

    Double voltage = new Double (battery.getVoltage ());

    return voltage;
  }
}
