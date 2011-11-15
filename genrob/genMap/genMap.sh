#!/bin/sh

cp="genRob.genControl.jar:genRob.genMap.jar"
dp="genRob.genControl.port=6000"
ec="genRob.genMap.ModuleImpl"

echo "java  -classpath $cp  -D$dp  genRob.genControl.Main  $ec"
java  -classpath $cp  -D$dp  genRob.genControl.Main  $ec
