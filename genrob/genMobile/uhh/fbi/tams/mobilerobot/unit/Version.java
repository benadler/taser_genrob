package uhh.fbi.tams.mobilerobot.unit;

import org.roblet.Unit;


/**
 * Diese Einheit gibt die Version des Moduls <tt>uhh.fbi.tams.mobilerobot</tt> an.
 * <p>
 * Die Versionierung erfolgt in drei Stufen.&nbsp;
 * Der &Uuml;bergang zwischen den drei Stufen ist nicht immer ganz klar,
 * folgt aber folgendem Prinzip:
 * <ul>
 *      <li>Gr&ouml;&szlig;ere &Auml;nderungen oder Erweiterungen gehen
 *          in eine Hauptversion (major).
 *      <li>Kleinere &auml;nderungen oder Erweiterungen ohne weitreichende Auswirkung
 *          gehen in eine Zwischenversion (minor).
 *      <li>Ergeben sich beim Einsatz einer Haupt- oder Nebenversion Probleme,
 *          bieten Korrekturversionen hier Abhilfe (patch).
 * </ul>
 *
 * @see #getMajor()
 * @see #getMinor()
 * @see #getPatch()
 *
 * @author Daniel Westhoff
 * @version 0.1
 * @since uhh.fbi.tams.moobilerobot 0.2
 */
public interface  Version
    extends Unit
{

    /**
     * Ergibt die Hauptversionsnummer.
     * @return Hauptversion
     */
    public int  getMajor ();
    /**
     * Ergibt die Zwischenversionsnummer.
     * @return Zwischenversion
     */
    public int  getMinor ();
    /**
     * Ergibt die Korrekturversionsnummer.
     * @return Korrekturversion
     */
    public int  getPatch ();

}
