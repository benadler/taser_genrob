package uhh.fbi.tams.mobilerobot;

import uhh.fbi.tams.mobilerobot.unit.MobileRobotException;

/** */
class Parameters
{

  /** Bezeichner der Eigenschaften. */
  private final String PROPERTY_HOST = "uhh.fbi.tams.mobilerobot.host";
  private final String PROPERTY_PORT = "uhh.fbi.tams.mobilerobot.port";

  /** Rechner auf dem die Robotersteuerung <tt>mobiled</tt> gestartet ist. */
  final String HOST;

  /**
   * Port an den man sich mit der Robotersteuerung <tt>mobiled</tt>
   * verbindet.
   */
  final int PORT;

  /** Konstruktor. */
  Parameters ()
      throws Exception
  {
    // Rechnername für 'mobiled'
    HOST = System.getProperty (PROPERTY_HOST, "localhost");

    // Port-Nummer für 'mobiled'
    try
    {
      PORT = Integer.parseInt (System.getProperty (PROPERTY_PORT, "9002"));
    }
    catch (NumberFormatException e)
    {
      throw new MobileRobotException
          ("W\u00E4rend das Modul initialisiert wurde, "
           + "ist ein Fehler aufgetreten:  Die Systemeigenschaft "
           + PROPERTY_PORT
           + " enth\u00E4lt keinen g\u00FCltigen Integer-Wert",
           e);
    }
  }



}
