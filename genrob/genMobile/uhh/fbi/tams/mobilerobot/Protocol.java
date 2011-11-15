package uhh.fbi.tams.mobilerobot;

import uhh.fbi.tams.mobilerobot.unit.ProtocolException;

/**
 * Diese Klasse bietet statische Methoden, die die Statusinformationen
 * in Telegrammen des Bin&auml;rprotokolls zwischen Roblet&reg;-Server
 * und Robotersteuerung <tt>mobiled</tt> &uuml;berpr&uuml;fen.
 *
 * @since uhh.fbi.tams.mobilerobot 0.1
 * @author Hagen Stanek, Daniel Westhoff
 * @version 0.2
 */
class Protocol
{
  /**
   * Name des Feldes mit der Anzahl der Befehle des Protokolls,
   *  die implementiert sind.
   */
  final static String COUNT = "COUNT";

  /** */
  final static String MARKS = "MARKS";

  /** */
  final static String NUM = "NUM";

  /** name des Feldes mit der Anzahl der Laserscan-Messwerte. */
  final static String SCANS = "SCANS";

  /**
   * &Uuml;berpr&uuml;ft, ob eine emfangene Antwort zum Kommando
   * der vorherigen Anfrage passt.
   *
   * @param reply Antwort-Telegramm, das &uuml;berprüft werden soll.
   * @param command Kommando, das erwartet wird.
   *
   * @throws ProtocolException Wird ausgel&ouml;st, wenn die Antwort nicht zum
   *                           &uuml;bergebenen Kommando passt.
   */
  static void ensureCorrectAnswerCommand (TelegramReply reply,
                                          int command)
      throws ProtocolException
  {
    // check command
    int received = reply.getCommand ();

    int expected = command | 0x80000000;

    if (received != expected)
    {
      throw new ProtocolException
          ("Empfangenen Antwort passt nicht zur Anfrage;"
           + " empfangen: " + Convert.toHex (received)
           + ", erwartet: " + Convert.toHex (expected));
    }
  }

  /**
   * &Uuml;berpr&uuml;ft, ob der Status eines Antwort-Telegramms gleich null
   * ist. Ist dies nicht der Fall, wird eine Ausnahme ausgel&ouml;st.
   *
   * @param reply Antwort-Telegramm, das &uuml;berpr&uuml;ft werden soll.
   *
   * @throws ProtocolException Wird ausgel&ouml;st, wenn der Status ungleich
   *                           Null ist und dadurch einen Fehler signalisiert.
   */
  static void ensureStatusOfZero (TelegramReply reply)
      throws ProtocolException
  {
    // check status
    int status = reply.getInteger (0);

    if (status != 0)
    {
      throw new ProtocolException
          ("Status-Feld ungleich Null"
          + ";  Wert: " + Convert.toDecAndHex (status)
          + ";  Kommando: "  + Convert.toHex (reply.getCommand ())
          );
    }
  }

  /**
   * &Uuml;berpr&uuml;ft, ob das Limit eines Feldes in einem
   * Antwort-Telegramm eingehalten wurde.
   *
   * @param reply Antwort-Telegramm, das &uuml;berpr&uuml;ft werden soll.
   * @param field Feld, das &uuml;berpr&uuml;ft werden soll.
   * @param fieldNr Nummer des Feldes.
   * @param
   *
   * @throws ProtocolException Wird ausgel&ouml;st, wenn der Wert im Feld
   *                           das limit &uuml;berschreitet oder der Wert
   *                           negativ ist.
   */
  static int ensureLimit (TelegramReply reply,
                          String field,
                          int fieldNr,
                          int limit)
      throws ProtocolException
  {
    // get number of available commands
    int value = reply.getInteger (fieldNr);

    if (value < 0 || limit < value)
    {
      throw new ProtocolException
          ("Der Wert im Feld " + field
          + " (" + fieldNr + ") \u00FCbersteigt das Maximum oder ist negativ"
          + ";  Wert: " + Convert.toDecAndHex (value)
          + ";  Maximum: " + Convert.toDecAndHex (limit)
          + ";  Kommando: "  + Convert.toHex (reply.getCommand ())
          );
    }
    return value;
  }
}
