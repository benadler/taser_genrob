package uhh.fbi.tams.mobilerobot;

import uhh.fbi.tams.mobilerobot.unit.*;
import java.util.LinkedList;
import java.text.DecimalFormat;

/** Subsystem zur automatischen Ermittlung der Roboter-Pose. */
class AutoCalibration
    implements Runnable
{
  /** Ursprung des Roboterkoordinatensystems. */
  private final static Point2D ORIGIN = new Point2D ();

  /** Schwellwert für die Ähnlichkeit zweier Abstände in [mm]. */
  private static final int DISTANCE_THRESHOLD = 50;

  /** Wie weit darf ein Punkt vom ClusterMittelpunkt weg sein? */
  private static final double CLUSTER_THRESHOLD_SQUARED = 100 * 100;

  /** Auf zwei Nachkommastellen runden. */
  private static final DecimalFormat DF = new DecimalFormat ("0.00");

  /** Zugriff auf 'mobiled'. */
  private final Commands commands;

  /** Zugriff auf das Logbuch. */
  private final ModuleLog out;

  /** Feld mit den Initialen MArken. */
  private final Point2D[] mark;

  /** Dreiecksmatrix mit paarweisen Distanzen zwischen den initialen Marken. */
  private final int[][] distance;

  /** Thread der Regelmäßig die Pose des 'mobiled' &uuml;berwacht. */
  private volatile Thread thread;

  /** Zeit für die Pose-Schätzung in [ms]. */
  private volatile long time = 0;

  /** Default-Konstruktor. */
  private AutoCalibration ()
  {
    commands = null;
    out = null;
    mark = null;
    distance = null;
  }

  /**
   * Konstruktor.
   *
   * @throws UnknownCommandException Fehler bei der Kommunikation mit 'mobiled'
   * @throws SocketCommunicationException Fehler bei der Kommunikation mit
   *         'mobiled'
   * @throws ProtocolException Fehler bei der Kommunikation mit 'mobiled'
   */
  AutoCalibration (final Commands commands, final ModuleLog out)
      throws UnknownCommandException,
      SocketCommunicationException,
      ProtocolException
  {
    this.commands = commands;
    this.out = out;

    // Landmarken abfragen
    mark = this.getMarksInitial ();

    // die paarweisen Abstaende zischen den Marken berechnen
    // und in einer Dreiecksmatrix speichern
    if (out.calibration)
    {
      out.calibration
          (this, "Berechne die paarweisen Abst&auml;nde der initialen Marken.");
    }

    distance = this.getDistanceMatrix (mark);

    commands.setPosition (this.getPoseEstimate ());

    if (out.calibration)
    {
      out.calibration (this, "Starte &uuml;berwachung der Pose-Sch&auml;tzung.");

      thread = new Thread (this);
      thread.start ();
    }
    else
    {
      thread = null;
    }
  }

  /** H&auml;lt den Thread an. */
  void stopEstimateThread ()
  {
    thread = null;
  }

  /** Berechnet die euklidische Distanz zwischen zwei Marken. */
  private int getDistance (final Point2D p1, final Point2D p2)
  {
    return (int) (Math.sqrt (sqr (p1.x - p2.x) + sqr (p1.y - p2.y)) + 0.5);
  }

  /** Berechnet die euklidische Distanz zwischen zwei Marken. */
  private int getDistanceSquare (final Pose p1, final Pose p2)
  {
    return sqr (p1.x - p2.x) + sqr (p1.y - p2.y);
  }

  /** Berechnet das Quadrat. */
  private int sqr (final int a)
  {
    return a * a;
  }

  /** Holt die initialen Marken. */
  private Point2D[] getMarksInitial ()
      throws UnknownCommandException,
      SocketCommunicationException,
      ProtocolException
  {
    Landmark[] landmark = commands.getAllMarksInitial ();

    int l = landmark.length;

    Point2D[] mark = new Point2D[l];

    for (int i = 0; i < l; i++)
    {
      mark[i] = new Point2D (landmark[i].position.x, landmark[i].position.y);
    }

    landmark = null;

    return mark;
  }

  /**
   * Liefert die Pose des Roboter anhand des aktuellen Scans.
   *
   * @throws UnknownCommandException Fehler bei der Kommunikation mit 'mobiled'
   * @throws SocketCommunicationException Fehler bei der Kommunikation mit
   *         'mobiled'
   * @throws ProtocolException Fehler bei der Kommunikation mit 'mobiled'
   */
  Pose getPoseEstimate ()
      throws ProtocolException,
      SocketCommunicationException,
      UnknownCommandException
  {
    long start = 0;

    // die paarweisen Abstaende zischen den gescannten Marken berechnen
    // und in einer Dreiecksmatrix speichern
    if (out.calibration)
    {
      out.calibration
          (this, "Berechne die paarweisen Abst&auml;nde der gescannten Marken.");

      start = System.currentTimeMillis ();
    }

    // Landmarken aus Scan
    Point2D[] scan = getMarksScanned ();

    int[][] scannedDistance = getDistanceMatrix (scan);

    // Zuordnung der Distanzen
    LinkedList poses = new LinkedList ();

    int i, j, k, l;

    ClusterPose p1, p2;

    int iMax = scannedDistance.length;
    int kMax = distance.length;

    for (i = 0; i < iMax; i++)
    {
      for (j = 0; j <= i; j++)
      {
        for (k = 0; k < kMax; k++)
        {
          for (l = 0; l <= k; l++)
          {
            // Wenn Distanz aehnlich...
            if (Math.abs (distance[k][l] - scannedDistance[i][j])
                < DISTANCE_THRESHOLD)
            {
              // ...dann die durch die Zuordnung bestimmten zwei Posen berechnen
              p1 = getPoseEstimate (mark[k + 1], mark[l], scan[i + 1], scan[j]);
              p2 = getPoseEstimate (mark[l], mark[k + 1], scan[i + 1], scan[j]);

              if (p1 != null)
              {
                poses.add (p1);
              }

              if (p2 != null)
              {
                poses.add (p2);
              }
            }
          }
        }
      }
    }

    // Pose schätzen
    ClusterPose p = getBiggestCluster (poses);

    if (out.calibration)
    {
      time = System.currentTimeMillis () - start;

      out.calibration (this, "Anzahl der initialen Marken : " + mark.length);
      out.calibration (this, "Anzahl der gescannten Marken: " + scan.length);
      out.calibration (this, "Anzahl der Posen im Cluster : " + p.number);
      out.calibration (this, "Anzahl der Cluster          : " + poses.size ());
      out.calibration (this, "Kalibriere Roboter          : Pose = " + p);
    }

    return p;
  }

  /** Holt die aktuell gescannten Marken in Roboterkoordinaten. */
  private Point2D[] getMarksScanned ()
      throws UnknownCommandException,
      SocketCommunicationException,
      ProtocolException
  {
    Point2D[] scan0 = commands.getScanPlatform (0).getMarks ();
    Point2D[] scan1 = commands.getScanPlatform (1).getMarks ();

    int l0 = scan0.length;
    int l1 = scan1.length;

    Point2D[] scan = new Point2D[l0 + l1];
    System.arraycopy (scan0, 0, scan, 0, l0);
    System.arraycopy (scan1, 0, scan, l0, l1);

    scan0 = null;
    scan1 = null;

    return scan;
  }

  /** Liefert eine Dreiecksmatrix mit den paarweisen Abständen der Punkte. */
  private int[][] getDistanceMatrix (Point2D[] point)
  {
    int i, j;
    int length = point.length;

    if (length < 0)
    {
      throw new IllegalArgumentException
          ("Kein Scanpunkte von der Robotersteuerung erhalten.");
    }
    else if (length == 0)
    {
      return new int[0][0];
    }

    int[][] distance = new int[length - 1][];

    for (i = 1; i < length; i++)
    {
      distance[i - 1] = new int[i];

      for (j = 0; j < i; j++)
      {
        distance[i - 1][j] = getDistance (point[i], point[j]);
      }
    }

    return distance;
  }

  /**
   * Berechnet aus zwei korrespondierendem Punktpaaren die durch Triangulation
   * geschätzte Pose.
   *
   * @param mark1 Point2D Punkt in Weltkoordinaten
   * @param mark2 Point2D Punkt in Weltkoordinaten
   * @param scan1 Point2D Punkt in Roboterkoordinaten
   * @param scan2 Point2D Punkt in Roboterkoordinaten
   * @return Pose daraus resultierende Pose des Robbis
   */
  private ClusterPose getPoseEstimate (Point2D mark1,
                                       Point2D mark2,
                                       Point2D scan1,
                                       Point2D scan2)
  {
    Polar polar1 = new Polar (scan1);
    Polar polar2 = new Polar (scan2);

    // Winkel nach Größe sortieren
    if (polar1.alpha > polar2.alpha)
    {
      Polar p3 = polar1;
      polar1 = polar2;
      polar2 = p3;
    }

    // Berechne die drei Kantenlaengen des Dreiecks ABC
    // B -> Roboter; A -> Marke1; C -> Marke2
    int a = polar1.radius;
    int b = getDistance (mark1, mark2);
    int c = polar2.radius;

    // die Innenwinkel des Dreiecks mit cos-Satz
    int help1 = -(sqr (a) - sqr (b) - sqr (c));
    int help2 = 2 * b * c;

    double alpha = Math.acos ((double) help1 / (double) help2);

    help1 = -(sqr (c) - sqr (a) - sqr (b));
    help2 = 2 * a * b;

    double gamma = Math.acos ((double) help1 / (double) help2);

    // weltwinkel A->C , daraus Weltwinkel A->B
    double omega1 = Math.atan2 ((double) mark2.y - (double) mark1.y,
                                (double) mark2.x - (double) mark1.x);

    omega1 = omega1 + gamma;

    // weltwinkel C->A , daraus Weltwinkel C->B
    double omega2 = Math.atan2 ((double) mark1.y - (double) mark2.y,
                                (double) mark1.x - (double) mark2.x);

    omega2 = omega2 - alpha;

    // B ist mittel der beiden Rueckprojektionen A->B und C->B
    double x = ((double) mark1.x + Math.cos (omega1) * polar1.radius +
                (double) mark2.x + Math.cos (omega2) * polar2.radius) * 0.5;

    double y = ((double) mark1.y + Math.sin (omega1) * polar1.radius +
                (double) mark2.y + Math.sin (omega2) * polar2.radius) * 0.5;

    // Weltwinkel B->A und B->C
    omega1 = Math.atan2 ((double) mark1.y - y, (double) mark1.x - x);
    omega2 = Math.atan2 ((double) mark2.y - y, (double) mark2.x - x);

    // wann ist atan2 = NaN ???
    if ((Double.isNaN (omega1)) || (Double.isNaN (omega2)))
    {
      return null;
    }

    // Drehung des Robbis delta: delta + gemessen = welt
    int delta_1 = Robiant.rad2robiant (omega1) - polar1.alpha;
    int delta_2 = Robiant.rad2robiant (omega2) - polar2.alpha;

    // mitteln der Drehung ist nicht trivial !!!
    int min, max;

    if (delta_1 < delta_2)
    {
      min = delta_1;
      max = delta_2;
    }
    else
    {
      max = delta_1;
      min = delta_2;
    }

    int diff = (int) (0.5 * (max - min));

    // Pose zurueck geben
    return new ClusterPose ((int) x, (int) y, min + diff);
  }

  /** Gibt das Cluster mit den meisten zusammengefügten Posen zurück.  */
  private ClusterPose getBiggestCluster (LinkedList cluster)
  {
    ClusterPose p1 = null;
    ClusterPose p2 = null; ;
    ClusterPose best1 = null;
    ClusterPose best2 = null;
    ClusterPose bestCluster = new ClusterPose (0, 0, 0);

    int i, j, distance, bestDist;

    while (true)
    {
      // finde kleinsten Abstand zwischen cluster
      bestDist = Integer.MAX_VALUE;

      for (i = 0; i < cluster.size (); i++)
      {
        p1 = (ClusterPose) cluster.get (i);

        for (j = i + 1; j < cluster.size (); j++)
        {
          p2 = (ClusterPose) cluster.get (j);

          distance = getDistanceSquare (p1, p2);

          if (distance < bestDist)
          {
            bestDist = distance;
            best1 = p1;
            best2 = p2;
          }
        }
      }

      // wenn keine weitere clusterung moeglich
      if (bestDist > CLUSTER_THRESHOLD_SQUARED)
      {
        break;
      }
      else // sonst loesche die beiden cluster und verbinde diese zu neuem
      {
        cluster.remove (best1);
        cluster.remove (best2);

        p1 = new ClusterPose (best1, best2);

        cluster.add (p1);

        // bestes Cluster gleich merken
        if (p1.number > bestCluster.number)
        {
          bestCluster = p1;
        }
      }
    }

    return bestCluster;
  }

  // Runnable
  public void run ()
  {
    Thread thisThread = Thread.currentThread ();

    long counter = 0, diff;
    Pose estimate;
    Pose pose;
    int estimateDistance;
    double averageDistance = 0;
    double averageTime = 0;

    while (thread == thisThread)
    {
      try
      {
        // iterativ "die letzten 100 Werte mitteln"
        if (counter < 100) counter++;

        pose = commands.getPosition ();

        estimate = getPoseEstimate ();

        estimateDistance = getDistance (new Point2D (pose.x, pose.y),
                                        new Point2D (estimate.x, estimate.y));

        averageTime = ((averageTime * (double) (counter - 1))
                       + (double) time) / (double) counter;

        averageDistance = ((averageDistance * (double) (counter - 1))
                           + (double) estimateDistance) / (double) counter;

        out.calibration (this, "Dauer der Kalibrierung (mittl. Dauer): "
                         + time
                         + " ms ("
                         + DF.format (averageTime)
                         + " ms)");

        out.calibration (this, "Positionsfehler (mittl. Fehler)      : "
                         + estimateDistance
                         + " mm ("
                         + DF.format (averageDistance)
                         + " mm)");


        // Zeit, die geschlafen werden soll
        diff = 5000 - time;

        if (diff > 0)
        {
          Thread.sleep (diff);
        }
        else
        {
          Thread.sleep (50);
        }
      }
      catch (Exception e)
      {
        e.printStackTrace ();
        out.calibration (this, e);
        thread = null;
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////

  private class Polar
  {

    /** Winkel in [Robiant]. */
    final int alpha;

    /** Radius in [mm]. */
    final int radius;

    /** Default-Konstruktor. */
    private Polar ()
    {
      alpha = 0;
      radius = 0;
    }

    /** Konstruktor, rechnet den übergebenen Punkt in Polarkoordinaten um. */
    Polar (Point2D point)
    {
      this.alpha = Robiant.atan2 (point.y, point.x);
      this.radius = getDistance (point, ORIGIN);
    }

//        public double distanceTo(PolarPoint2D p) {
//            double x = Math.cos(alpha) * range;
//            double y = Math.sin(alpha) * range;
//            double x1 = Math.cos(p.alpha) * p.range;
//            double y1 = Math.sin(p.alpha) * p.range;
//            return java.awt.geom.Point2D.distanceSq(x, y, x1, y1);
//        }
//
//        public int compareTo(Object o) {
//            PolarPoint2D p = (PolarPoint2D) o;
//            if (this.alpha+4*Math.PI < p.alpha+4*Math.PI) {
//                return -1;
//            }
//            return -1;
//        }
//
//        public String toString() {
//            return "PolarPoint2D("+alpha+","+range+")";
//        }

  }

  ///////////////////////////////////////////////////////////////////////////


  private class ClusterPose
      extends Pose
  {
    /** Anzahl der gemittelten Posen. */
    int number = 0;

    /** Default-Konstruktor. */
    private ClusterPose ()
    {}

    /** Konstruktor. */
    ClusterPose (int x, int y, int theta)
    {
      this.x = x;
      this.y = y;
      this.theta = theta;
      number = 1;
    }

    /** Konstruktor. */
    ClusterPose (ClusterPose p1, ClusterPose p2)
    {
      this.number = p1.number + p2.number;
      this.x = (p1.x * p1.number + p2.x * p2.number) / this.number;
      this.y = (p1.y * p1.number + p2.y * p2.number) / this.number;

      double weight;

      if (p1.theta < p2.theta)
      {
        weight = (double) p2.number / (double)this.number;

        this.theta = p1.theta + (int) (weight * 0.5 * (p2.theta - p1.theta));
      }
      else
      {
        weight = (double) p1.number / (double)this.number;

        this.theta = p2.theta + (int) (weight * 0.5 * (p1.theta - p2.theta));
      }

    }
  }

}
