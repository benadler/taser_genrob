
import  genRob.genControl.client.Client;
import  genRob.genMap.unit.basic.Line2;
import  genRob.genMap.unit.basic.Lines2;
import  java.io.Serializable;
import  org.roblet.Roblet;
import  org.roblet.Robot;


/**
 * Dies ist ein einfaches Beispiel-Programm, welches ein Roblet® auf einem
 * passenden Roblet®-Server laufen läßt.&nbsp;
 * Dabei wird ein Roblet® mit der Aufgabe zu einem Kartenserver geschickt,
 * dort alle Kartenlinien zu löschen.
 *
 * <h4>Kompilation:</h4>
 * Zur Vereinfachung existiert
 * das Script <tt>make.sh</tt> (für Unix/Linux/MacOS-sh)
 * bzw. die Batch-Datei <tt>make.bat</tt> (für Windows).&nbsp;
 * <tt>make</tt> muß in dem Verzeichnis der Beispiele ausgeführt werden.
 *
 * <h4>Ausführung:</h4>
 * Der Name des Roblet®-Servers wird als Parameter erwartet.
 * <p>
 * Zur Vereinfachung existiert
 * das Script <tt>run.sh</tt> (für Unix/Linux/MacOS-sh)
 * bzw. die Batch-Datei <tt>run.bat</tt> (für Windows).&nbsp;
 * Dieses Script/Batch erwartet als ersten Parameter den Namen dieser Klasse
 * und akzeptiert auch weitere Parameter.
 * <blockquote><tt>
 *  sh run.sh  KLASSENNAME
 * </tt></blockquote>
 * bzw.
 * <blockquote><tt>
 *  run  KLASSENNAME
 * </tt></blockquote>
 */
public class  Clear
{

    /**
     * Hier findet unsere lokale Aktivität statt.
     * @param   args    [0] Host-Name des Roblet®-Servers (Kartenservers)
     * @throws  Exception   Bei beliebigem Fehler
     */
    public static void  main (String[] args)
        throws Exception
    {
        if (args. length < 1)
            throw new Exception ("Name des Karten-Servers fehlt");

		// Erzeugung einer Kommunikationsumgebung
		Client  rClient = new Client ();

        // Schicken des Roblets, Warten auf Ende dessen Ausführung
        rClient. run (args[0], new RobletImpl ());

        // Abbau der Kommunikationsumgebung
        rClient. close ();
    }


    /**
     * Dies ist die Klasse, aus dem unser Roblet® gemacht ist.
     */
    private static class  RobletImpl
        implements Roblet, Serializable
    {

        /**
         * Die Haupt-Methode des Roblets®, welches dann auf dem Roblet®-Server
         * läuft.
         * @param  rRobot   Roblet®-Server-Kontext
         * @return <tt>null</tt> (nichts)
         * @throws  Exception   Bei beliebigem Fehler
         */
        public Object  execute (Robot rRobot)
            throws Exception
        {
            // Wir wollen die Karten-Einheit, die uns die
            // Linien der Karte direkt löscht.
            Lines2  rLines2 = (Lines2) rRobot. getUnit (Lines2.class);

            // Wenn diese Einheit nicht vorhanden ist, werfen wir eine Ausnahme
            // an unseren Auftraggeber zurück.
            if (rLines2 == null)
                throw new Exception ("Keine Kartenschnittstelle gefunden!");

            // Ansonsten löschen wir die Liste der Linien
            Line2[]  aLine2 = rLines2. list ();
            for (int  i = 0;  i < aLine2. length;  ++i)
                rLines2. remove (aLine2 [i]);

            // Wir haben nichts zurückzugeben
            return null;
        }
    }
}
