package uhh.fbi.tams.mobilerobot;

/**
 * Die Klasse <tt>Convert</tt> bietet verschiedene statische
 * Methoden zu Konvertierung von Integern ins Hexadezimalformat
 * und ins Bin&auml;rformat.
 *
 * @author Daniel Westhoff
 * @version 0.1
 * @since uhh.fbi.tams.mobilerobot (Version 0.6.1)
 */
class Convert
{

  /**
   * Konvertiert einen int-Wert in eine Zeichenkette mit Dezimalzahl
   * und Hexadezimal in Klammern. Die Hexadezimalzahl wird in einer
   * durch acht teilbaren Gesamtl&auml;nge angezeigt.
   *
   * @param value Wert, der in eine Zeichenkette umgewandelt werden soll.
   * @return Zeichenkette mit Dezimalzahl und Hexadezimalzahl in Klammern.
   */
  public static String toDecAndHex (int value)
  {
    return value + " (" + toFormatedHex (value) + ")";
  }


  /**
   * Konvertiert einen int-Wert in eine Zeichenkette mit Hexadezimalzahl.
   * Die Hexadezimalzahl wird in einer durch acht teilbaren Gesamtl&auml;nge
   * angezeigt.
   *
   * @param value Wert, der in eine Zeichenkette umgewandelt werden soll.
   * @return Zeichenkette mit Wert als Hexadezimalzahl.
   */
  public static String toHex (int value)
  {
    return toFormatedHex (value);
  }


  /**
   * Konvertiert einen int-Wert in eine Zeichenkette mit Bin&auml;rzahl.
   *
   * @param value Wert, der in eine Zeichenkette umgewandelt werden soll.
   * @return Zeichenkette mit Wert als Bin&auml;rzahl.
   */
  public static String toBinary (int value)
  {
    return Integer.toBinaryString(value);
  }

  /////////////////////////////////////////////////////////////////////////////
  // private
  /////////////////////////////////////////////////////////////////////////////

  /**
   * Formatierte Zeichenkette einer Hexzahl,
   * es werden führende Nullen eingesetzt
   * und eine durch acht teilbare Gesamtl&auml;nge erzeugt.
   */
  private static String toFormatedHex (int value)
  {
    String unformatedHex = Integer.toHexString (value);

    int length = unformatedHex.length() % 8;

    StringBuffer formatedHex = new StringBuffer ("0x");

    for (int i = 0; (i < (8 - length) && (0 != length)); ++i)
    {
      formatedHex.append("0");
    }

    formatedHex.append(unformatedHex);

    return formatedHex.toString();
  }

}
