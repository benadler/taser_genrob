package uhh.fbi.tams.mobilerobot.test;

import java.io.Serializable;

import genRob.genControl.client.Client;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.localization.Localization;
import org.roblet.Roblet;
import org.roblet.Robot;

/**
 * Fragt die aktuelle Pose des Roboters ab. Der Roblet&reg;-Server muss
 * auf dem Roboter auf <b>Port 6002</b> laufen.
 */
class MobileShow
    implements Roblet, Serializable
{
  /** Startet die Anwendung. */
  public static void main (String args[])
      throws Exception // JVM behandelt mögliche Ausnahmen
  {
    // Initialisierung der Kommunikationsumgebung
    Client client = new Client ();

    // Kontaktaufnahme mit dem Roblet-Server und Ausführung des Roblets
    try
    {
      // Schicken des Roblets, Warten auf Ende dessen Ausführung
      // und übernahme des Roblet-Resultates
      Pose pose = (Pose) client.run ("tams59:6002", new MobileShow ());

      System.out.println ("Die Pose des Roboters ist " + pose + ".");
    }
    finally
    {
      // Abbau der Kommunikationsumgebung
      client.close ();
    }

    System.out.println ("Fertig.");
  }

  /**
  * Die Haupt-Methode des Roblets&reg; - hier findet unsere
  * Aktivität auf dem Roboter (Roblet&reg;-Server) statt.
  */
 public Object execute (Robot rRobot)
     throws Exception
 {
   Localization localisation
       = (Localization) rRobot.getUnit (Localization.class);

   if (localisation == null)
   {
     throw new Exception (
         "Einheit <Localization> nicht gefunden!");
   }

   return localisation.getPose ();
 }

}
