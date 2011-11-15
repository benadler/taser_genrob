@echo off

rem  Dies ist eine Beispiel-Start-Datei fuer genRob(R)-genControl.
rem  ISO-8859-1

rem  genControl wird hier ohne weitere Module an Port 2000
rem  gestartet - stellt somit nur Standardfunktionalitaet bereit.
rem  Mehr Dokumentation ist via  index.html  zu erhalten.

java  -DgenRob.genControl.port=2000  -jar genRob.genControl.jar


rem  Beispiel mit Modul:
rem  java  -DgenRob.genControl.port=2000  -classpath mypath\genRob.genControl.jar;mypath2\myModule.jar  genRob.genControl.Main  mypackage.MyModule
