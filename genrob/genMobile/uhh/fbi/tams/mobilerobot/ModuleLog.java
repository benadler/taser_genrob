package uhh.fbi.tams.mobilerobot;

import  genRob.genControl.modules.Log;

/**
 * Logeintr&auml;ge je nach gesetzten Properties im Log des
 * Roblet&reg;-Servers erzeugen.
 *
 * <p><tt>-Duhh.fbi.tams.mobilerobot.log=...</tt>
 * <ul>
 *  <li> <tt>module</tt> = Ausgaben &uuml;ber die T&auml;tigkeiten des Moduls
 *  <li> <tt>version</tt> = Ausgabe der Versionsnummer
 * </ul>
 *
 * @version 0.1
 * @since uhh.fbi.tams.mobilerobot 0.2
 **/
class  ModuleLog
{

    public boolean  module;
    public void  module (Object oCaller, Object oMessage)
    {
      log ("mobilerobot:module", oCaller, oMessage);
    }

    public boolean  version;
    public void  version (Object oCaller, Object oMessage)
    {
      log ("mobilerobot:version", oCaller, oMessage);
    }

    public boolean calibration;
    public void  calibration (Object oCaller, Object oMessage)
    {
      log ("mobilerobot:calibration", oCaller, oMessage);
    }



    /** Konstruktor. */
    ModuleLog (Log log)
    {
        this.log = log;

        String  strLog = System. getProperty ("uhh.fbi.tams.mobilerobot.log");
        if (strLog != null)
        {
            module = contains (strLog, "module");
            version = contains (strLog, "version");
            calibration = contains (strLog, "calibration");
        }
    }

    private final Log  log;

    private boolean  contains (String str, String substr)
    {
        return str. indexOf (substr) != -1;
    }

    private void log (String strPart, Object oInstance, Object oMessage)
    {
        if (oMessage instanceof Exception)
        {
          log.out (strPart,
                        oInstance. getClass (),
                        exceptionToString ((Exception) oMessage));
        }
        else
        {
          log.out (strPart, oInstance.getClass (), oMessage);
        }
    }

    private String exceptionToString (Exception e)
      {
        StringBuffer sb = new StringBuffer ("<font color=red><b>");
        sb.append (e.getClass().getName());
        sb.append (":</b> ");

        String message = e.getMessage ();

        if (message != null)
        {
          sb.append (message);
        }

        sb.append ("<font size=-2>");

        StackTraceElement[] se = e.getStackTrace();

        for (int i = 0; i < se.length; ++i)
        {
          sb.append ("<br>&nbsp;at ");
          sb.append (se[i].toString());
        }

        Throwable cause = e.getCause ();

        if (cause != null)
        {
          sb.append (processCause (cause));
        }

        sb.append ("</font>");  // Schrift kleiner
        sb.append ("</font>");  // Shriftfarbe rot

        return sb.toString ();
      }


      // Die Ursache der Exception rekursiv ermitteln
      private StringBuffer processCause (Throwable t)
      {


        StringBuffer sb = new StringBuffer ("<br>caused by <b>");
        sb.append (t.getClass().getName());
        sb.append (":</b> ");

        String message = t.getMessage ();

        if (message != null)
        {
          sb.append (message);
        }

        StackTraceElement[] se = t.getStackTrace();

        for (int i = 0; i < se.length; ++i)
        {
          sb.append ("<br>&nbsp;at ");
          sb.append (se[i].toString());
        }

        Throwable cause = t.getCause ();

        if (cause != null)
        {
          sb.append (processCause (cause));
        }

        return sb;
    }
}
