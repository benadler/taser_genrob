#!/bin/sh

cp="genRob.genControl.jar:genRob.genPath.jar:uhh.fbi.tams.pathplanner.jar"
dp="genRob.genControl.port=7000"
ec="genRob.genPath.ModuleImpl"
da0=genRob.genPath.algorithm.0=uhh.fbi.tams.pathplanner.Planner

echo "java  -classpath $cp  -D$dp  -D$da0  genRob.genControl.Main  $ec"

java  -classpath $cp  -D$dp  -D$da0  genRob.genControl.Main  $ec
