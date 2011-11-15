
import  genRob.genControl.client.Client;
import  genRob.genMap.unit.basic.Line2;
import  genRob.genMap.unit.basic.Lines2;
import  java.io.FileNotFoundException;
import  java.io.FileReader;
import  java.io.IOException;
import  java.io.LineNumberReader;
import  java.io.Serializable;
import  java.util.ArrayList;
import  java.util.Iterator;
import  java.util.StringTokenizer;
import  org.roblet.Roblet;
import  org.roblet.Robot;


/**
 * Dies ist ein einfaches Beispiel-Programm, welches ein Roblet® auf einem
 * passenden Roblet®-Server laufen läßt.&nbsp;
 * Dabei wird ein Roblet® mit der Aufgabe zu einem Kartenserver geschickt,
 * dort Kartenlinien hinzuzufügen.&nbsp;
 * Die Liniendaten müssen sich in einer Textdatei mit einer Linie
 * pro Zeile befinden.
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
public class  Add
{

    /**
     * Hier findet unsere lokale Aktivität statt.
     * @param   args    [0] Host-Name des Roblet®-Servers (Kartenservers);&nbsp;
     *                  [1] Name der Karten-Daten-Datei
     * @throws  Exception   Bei beliebigem Fehler
     */
    public static void  main (String[] args)
        throws Exception
    {
        if (args. length < 1)
            throw new Exception ("Name des Karten-Servers fehlt");
        if (args. length < 2)
            throw new Exception ("Name der Datei mit den Karten-Daten fehlt");

		// Erzeugung einer Kommunikationsumgebung
		Client  rClient = new Client ();

        // Hole Linien aus der Datei
        Line2[]  aLine2 = load (args[1]);

        // Schicken des Roblets, Warten auf Ende dessen Ausführung
        rClient. run (args[0], new RobletImpl (aLine2));

        // Abbau der Kommunikationsumgebung
        rClient. close ();
    }

    /**
     * Lade Karte-Daten aus einer Datei.
     * @param  strFile  Pfad+Name der Datei
     * @return  Liniendaten
     * @throws FileNotFoundException falls die Datei nicht gefunden wurde
     * @throws IOException falls beim Arbeiten mit der Datei ein Fehler auftrat
     * @throws Exception falls in der Syntax der Datei (Zahlenabfolge)
     *                      ein Fehler auftrat
     */
    private static Line2[]  load (String strFile)
        throws FileNotFoundException, IOException, Exception
    {
        Line2[]  aLine2 = null;

        FileReader  fr = new FileReader (strFile);
        LineNumberReader  lnr = new LineNumberReader (fr);

        {
            ArrayList  al = new ArrayList ();

            String  strLine;
            while (null != (strLine = lnr. readLine ()))
            {
//                System.out.println (strLine);
                StringTokenizer  st = new StringTokenizer (strLine);
                String  t;

                if (! st. hasMoreTokens ())  continue;

                t = st. nextToken ();
                if (0 == t. indexOf (';'))  continue;

                int  iAx,  iAy,  iBx,  iBy;
                try
                {
                    iAx = Integer. parseInt (t);

                    t = st. nextToken ();
                    iAy = Integer. parseInt (t);

                    t = st. nextToken ();
                    iBx = Integer. parseInt (t);

                    t = st. nextToken ();
                    iBy = Integer. parseInt (t);
                }
                catch (NumberFormatException nfe)
                {
                    throw new Exception ("Fehler in Zeile "
                                            + lnr. getLineNumber (), nfe);
                }

                Line2  e = new Line2 (iAx, iAy, iBx, iBy);
                al. add (e);
            }

            aLine2 = new Line2 [al. size ()];
            int  i = 0;
            for (Iterator  it = al. iterator ();  it. hasNext ();  ++i)
                aLine2 [i] = (Line2) it. next ();
        }

        lnr. close ();	// Schließt auch Datei

        return aLine2;
    }


    /**
     * Dies ist die Klasse, aus dem unser Roblet® gemacht ist.
     */
    private static class  RobletImpl
        implements Roblet, Serializable
    {

        /**
         * Initialisiere die Roblet®-Instanz, damit sie beim
         * Versenden die nötigen Daten trägt.
         * @param  aLine2  Linien, die zum Karten-Server hinzuzufügen sind
         */
        RobletImpl (Line2[] aLine2)
        {
            mf_aLine2 = aLine2;
        }
        /** Karten-Daten für den Karten-Server */
        private final Line2[]  mf_aLine2;

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
            // Wir wollen die Karteneinheit, die uns die
            // Linien in der Karte einträgt.
            Lines2  rLines2 = (Lines2) rRobot. getUnit (Lines2.class);

            // Wenn diese Einheit nicht vorhanden ist, werfen wir eine Ausnahme
            // an unseren Auftraggeber zurück.
            if (rLines2 == null)
                throw new Exception ("Keine Kartenschnittstelle gefunden!");

            // Ansonsten fügen wir die Linien hinzu
            for (int  i = 0;  i < mf_aLine2. length;  ++i)
                rLines2. add (mf_aLine2[i]);

            // Wir haben nichts zurückzugeben
            return null;
        }
    }
}
