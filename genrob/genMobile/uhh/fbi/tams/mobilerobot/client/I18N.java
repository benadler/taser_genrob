package uhh.fbi.tams.mobilerobot.client;

class I18N
{
  // Log-Meldungen
  static final String INIT                           = "Initialisiere Roblet.";
  static final String INIT_COMPLETED                 = "Intialisierung beendet.";
  static final String ACCESS                         = "Zugriff auf Roboter erhalten (Unit: Access).";
  static final String BRAKES                         = "Zugriff auf Bremsen erhalten (Unit: Brakes).";
  static final String BRAKES_RELEASED                = "Bremsen gelöst.";
  static final String BRAKES_APPLIED                 = "Bremsen angezogen.";
  static final String MOTION                         = "Roboter kann bewegt werden (Unit: Motion).";
  static final String MOTION_COMPLETED               = "Bewegung beendet.";
  static final String VELOCITY                       = "Geschwindigkeiten können angepasst werden.";
  static final String COLLISION                      = "Abfragen, ob Roboter blockiert ist, ist möglich (Unit: Collision)";
  static final String LOCALIZATION                   = "Roboter kann seine Position bestimmen (Unit: Localization).";
  static final String DRIVE_TO_POSE                  = "Fahre zur Pose ";
  static final String WAIT_FOR_COMPLETED             = "Warte auf das Ende der Bewegung.";
  static final String GET_POSE                       = "Liefere Pose.";
  static final String BATTERY                        = "Spannung der Akkus kann abgefragt werden (Unit: Battery).";
  static final String GET_COLLISION_RADIUS           = "Abfrage des Kollisionsradius.";
  static final String DESTINATION                    = "Ziel der aktuellen BEwegung kann abgefragt werden.";
  static final String GET_DESTINATION                = "Abfrage des Ziels der aktuellen Bewegung.";
  static final String LASERSCANNER                   = "Laserscans können abgefragt werden.";
  static final String GET_LASERSCAN                  = "Abfrage der Laserscans.";
  static final String GET_RAW_LASERSCAN              = "Abfrage der Entfernungsmessungen.";
  static final String ROTATE_TO_ORIENTATION          = "Auf neue Orientierung drehen: ";
  static final String GET_NUMBER_OF_LASERSCANNERS    = "Abfrage der Anzahl der Laserscanner.";
  static final String GET_POSITIONS_OF_LASERSCANNERS = "Abfrage der Positionen der Laserscanner.";
  
  // Fehlermeldungen bei Ausnahmen
  static final String EXCEPTION_CLIENT               = "Fehler beim Aufbau der Verbindung zum Roblet-Netzwerk.";
  static final String EXCEPTION_ROBLET_START         = "Fehler bei dem Starten und der Ausführung des Robolet auf dem Server.";
  static final String EXCEPTION_REMOTE               = "Fehler bei der Kommunikation mit dem Roboter.";
  static final String EXCEPTION_INIT                 = "Mobilerobot wurde nicht oder fehlerhaft initialisiert.";
  static final String EXCEPTION_HOST                 = "Fehler beim Ermitteln des lokalen Hostnamens.";
  static final String EXCEPTION_NO_UNIT_ACCESS       = "Keine Einheit: Access. Zugriff auf Roboter nicht m\u00f6glich.";
  static final String EXEPTIION_NO_UNIT_BRAKES       = "Keine Einheit: Brakes. Bremsen nicht zugreifbar.";
  static final String EXCEPTION_NO_UNIT_MOTION       = "Keine Einheit: Motion. Einheit Bewegen des Roboters nicht möglich.";
  static final String EXCEPTION_NO_UNIT_VELOCITY     = "Keine Einheit: Velocity. Einheit Bewegen des Roboters nicht möglich.";
  static final String EXCEPTION_NO_UNIT_COLLISION    = "Keine Einheit: Collision. Abfragen, ob der Roboter blockiert ist, ist nicht möglich.";
  static final String EXCEPTION_NO_UNIT_LOCALIZATION = "Keine Einheit: Localisation. Position des Roboters kann nicht bestimmt werden!";
  static final String EXCEPTION_NO_UNIT_BATTERY      = "Keine Einheit: Battery. Spannung der Akkus kann nicht abgefragt werden.";
  static final String EXCEPTION_NO_UNIT_DESTINATION  = "Keine Einheit: Destination. Ziel der aktuellen Bewegung kann nicht abgefragt werden.";
  static final String EXCEPTION_NO_UNIT_REMOTEEXPORT = "Keine Einheit: RemoteExport. Es kann keine RMI-Server auf den Roblet-Server gestartet werden.";
  static final String EXCEPTION_NO_UNIT_LASERSCANNER = "Keine Einheit: Laserscanner. Es können keine Laserscans erfragt werden.";
  static final String EXCEPTION_ACCESS               = "Zugriff auf die Robotersteuerung nicht m\u00f6glich. Roboter wird bereits verwendet.";
  static final String EXCEPTION_ACCESS_DENIED        = "Zugriff verweigert.";
  static final String EXCEPTION_APPLY_BRAKES         = "Bremsen konnten nicht angezogen werden.";
  static final String EXCEPTION_RELEASE_BRAKES       = "Bremsen konnten nicht gel\u00f6st werden";
  static final String EXCEPTION_BRAKES_ON            = "Die Bremsen sind angezogen.";
  static final String EXCEPTION_UNKNOWN_COMMAND      = "Unbekannter Befehl.";
  static final String EXCEPTION_PROTOKOLL            = "Fehler im Protokoll.";
  static final String EXCEPTION_SOCKET               = "Socket-Kommunikation fehlerhaft.";
  static final String EXCEPTION_MOTION               = "Bewegung konnte nicht ausgeführt werden.";
  static final String EXCEPTION_PATH                 = "Kein Weg zum Zielpunkt.";
  static final String EXCEPTION_WAIT_FOR_COMPLETED   = "Auf das Ende der Bewegung konnte nicht gewartet werden.";
  static final String EXCEPTION_GET_POSE             = "Fehler beim Ermitteln der Pose.";
  static final String EXCEPTION_REMOTE_EXPORT        = "Die RMI-Schnittstelle konnte nicht exportiert werden.";
  static final String EXCEPTION_RMI_UNEXPORT         = "Fehler beim Beenden des RMI-Services.";
  static final String EXCEPTION_GET_VOLTAGE          = "Fehler beim Abfragen der Akkuspannung.";
  static final String EXCEPTION_IS_STALLED           = "Fehler bei der Abfrage, ob der Roboter blockiert ist.";
  static final String EXCEPTION_STOP                 = "Fehler beim Anhalten des Roboters.";
  static final String EXCEPTION_IS_COMPLETED         = "Abfrage nach Ende der Bewegung fehlgeschlagen.";
  static final String EXCEPTION_ROTATE               = "Fehler beim Versuch den Roboter zu drehen.";
  static final String EXCEPTION_GET_DESTINATION      = "Fehler bei der Abfrage des Ziels der aktuellen Bewegung.";
    
}
