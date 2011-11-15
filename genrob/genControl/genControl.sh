#!/bin/sh

#  Dies ist eine Beispiel-Start-Datei fuer genRob(R)-genControl.
#  ISO-8859-1

#  genControl wird hier ohne weitere Module an Port 2000
#  gestartet - stellt somit nur Standardfunktionalitaet bereit.
#  Mehr Dokumentation ist via  index.html  zu erhalten.

java  -DgenRob.genControl.port=2000  -jar genRob.genControl.jar


#  Beispiel mit Modul:
#  java  -DgenRob.genControl.port=2000  -classpath mypath/genRob.genControl.jar:mypath2/myModule.jar  genRob.genControl.Main  mypackage.MyModule
