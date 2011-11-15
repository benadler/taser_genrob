
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
 * von dort alle Kartenlinien zu holen und lokal auszugeben.
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
public class  List
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
        // und übernahme des Roblet-Resultates
        Object  rObject = rClient. run (args[0], new RobletImpl ());

        // Konvertiere in den wirklich zurückgegebenen Typ.
        Line2[] aLine2 = (Line2[]) rObject;

        // Wenn wir eine Liste von Linien bekommen haben ...
        if (aLine2 != null)
        {
            int  iCount = aLine2. length;

            if (iCount == 0)
                System.out.println ("Es gibt keine Karteneintraege");
            else
                // ... gehen wir jeden Listeneintrag durch ...
                for (int  i = 0;  i < aLine2.length;  ++i)
                {
                    // ... und geben ihn einfach aus.
                    System.out.println ("(" + i + "): " + aLine2[i]);
                }
        }
        else
            System.out.println ("Keine Eintraege gefunden");

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
         * @return  Linienfeld vom Typ {@Line}
         * @throws  Exception   Bei beliebigem Fehler
         */
        public Object  execute (Robot rRobot)
            throws Exception
        {
            // Wir wollen die Karten-Einheit, die uns die
            // Linien der Karte direkt zurückgibt.
            Lines2  rLines2 = (Lines2) rRobot. getUnit (Lines2.class);

            // Wenn diese Einheit nicht vorhanden ist, werfen wir eine Ausnahme
            if (rLines2 == null)
                throw new Exception ("Keine Kartenschnittstelle gefunden!");

            // Ansonsten holen wir die Liste der Linien und
            // geben einfach die Liste an den Auftraggeber zurück.
            return rLines2. list ();
        }
    }
}
