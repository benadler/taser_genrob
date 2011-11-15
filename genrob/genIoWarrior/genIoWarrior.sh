#!/bin/sh

#set -e

# Log-Ausgaben   : -DgenRob.genControl.out=jini,base,main,modules,shell,slots
#                  -Duhh.fbi.tams.iowarror.log=version,module

#cd /usr/local/src/service_robot/genRob/iowarrior-0.5.1/

java $1 -Djava.library.path=. -Duhh.fbi.tams.iowarrior.log=version -classpath .:genRob.genControl.jar:uhh.fbi.tams.iowarrior.unit.jar:uhh.fbi.tams.iowarrior.jar:uhh.fbi.tams.utils.jar genRob.genControl.Main uhh.fbi.tams.iowarrior.ModuleImpl
