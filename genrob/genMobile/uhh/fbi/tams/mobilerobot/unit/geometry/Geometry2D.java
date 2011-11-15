package uhh.fbi.tams.mobilerobot.unit.geometry;

import uhh.fbi.tams.mobilerobot.unit.Polygon;
import org.roblet.Unit;

/**
 * Diese Einheit liefert Informationen &uuml;ber den 2D-Grundriss der Hardware/des
 * Roboters.
 *
 * @author Daniel Westhoff
 * @version 0.1
 */
public interface Geometry2D
    extends Unit
{
  /** Liefert den Grundriss der Hardware/des Roboters in [mm]. */
  public Polygon getOutline ();
}
