package uhh.fbi.tams.mobilerobot;

import genRob.genControl.modules.Use;
import uhh.fbi.tams.mobilerobot.unit.Point2D;
import uhh.fbi.tams.mobilerobot.unit.Polygon;
import uhh.fbi.tams.mobilerobot.unit.geometry.Geometry2D;

class Geometry2DImpl
    implements Geometry2D
{
  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  /** Nutzungszähler */
  private final Use use;

  /** Log. */
  private final ModuleLog out;

  /** Umriss: x-Koordinaten. */
  private final int[] x
      = { -328, -298, -258, -258, -81, -81, 81, 81, 258, 258, 298, 328,
           328, 298, 258, 258, 81, 81, -81, -81, -258, -258, -298, -328
        };

  /** Umriss: y-Koordinaten. */
  private final int[] y
      = { -258, -298, -298, -328, -328, -391, -391, -328, -328, -298, -298, -258,
           258, 298, 298, 328, 328, 391, 391, 328, 328, 298, 298, 258
        };

  /** Umriss. */
  private final Polygon outline;

  //////////////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////////////

  // default-Kostruktor
  private Geometry2DImpl ()
  {
    this.use = null;
    this.out = null;
    this.outline = null;
  }

  /** Konstruktor */
  Geometry2DImpl (Use use, ModuleLog out)
  {
    this.use = use;
    this.out = out;

    // statischen Umriss erstellen
    assert (x.length == y.length);

    Point2D[] points = new Point2D[x.length];

    for (int i = 0; i < x.length; ++i)
    {
      points[i] = new Point2D (y[i], x[i]);
    }

    this.outline = new Polygon (points);
  }

  // Geometry2D
  public Polygon getOutline ()
  {
    if (out.module)
    {
      out.module (this, "getOutline()");
    }

    // Nutzungszähler erhöhen
    use.raise ();

    try
    {
      return this.outline;
    }
    finally // in jedem Fall auch bei Exception
    {
      // Nutzungszähler verringern
      use.lower ();
    }
  }

}
