package uhh.fbi.tams.mobilerobot.unit;

import org.roblet.Unit;


/**
 * Diese Einheit regelt den Zugriff auf Methoden, die den Roboter steuern.
 * Solche Methoden sind in verschiedenen Einheiten zu finden und daran zu
 * erkennen, dass sie eine {@link AccessException} ausl&ouml;sen k&ouml;nnen.
 * Bevor eine solche Methode durch ein Roblet&reg; aufgerufen werden kann
 * muss das Roblet&reg; den Zugriff auf die Steuerung des Roboters mit
 * der Methode {@link #getAccess()} erhalten haben. Andere Roblets&reg;
 * k&ouml;nnen den Roboter erst wieder steuern, wenn der Zugriff durch den
 * Aufruf von {@link #releaseAccess()} freigegeben wurde. Die &Uuml;bergabe
 * des Zugriffs kann mit {@link #forceAccess()} erzwungen werden.
 *
 * @see uhh.fbi.tams.mobilerobot.unit.localization.Calibration
 * @see uhh.fbi.tams.mobilerobot.unit.motion.Brakes
 * @see uhh.fbi.tams.mobilerobot.unit.motion.Motion
 * @see uhh.fbi.tams.mobilerobot.unit.motion.Velocity
 *
 * @author Daniel Westhoff
 * @version 0.1
 * @since uhh.fbi.tams.mobilerobot 0.4
 */
public interface Access
    extends Unit
{
  /**
   * Muss aufgerufen werden, bevor ein Roblet&reg; Methoden
   * von Einheiten aufrufen kann, die den Roboter steuern. Methoden, die den
   * Roboter steuern, k&ouml;nnen eine {@link AccessException} ausl&ouml;sen.
   *
   * @return <tt>true</tt>, wenn dem Roblet@reg; der Zugriff auf die Steuerung
   *         des Roboters erlaubt wurde, sonst <tt>false</tt>.
   */
  public boolean getAccess ();

  /**
   * Gibt den Zugriff auf die Steuerung des Roboters wieder frei,
   * so dass andere Roblets&reg; den Roboter kontrollieren k&ouml;nnen.
   * Sollte ein anderes Roblet&reg; bereits den Zugriff haben, passiert
   * nichts und der Zugriff bleibt beim anderen Roblet&reg;.
   */
  public void releaseAccess ();

  /**
   * Nimmt einem anderen Roblet&reg; den Zugriff auf die
   * Steuerung weg. Das Roblet&reg;, das diese Methode aufgerufen hat,
   * kann nun den Roboter kontrollieren.
   */
  public void forceAccess ();

}
