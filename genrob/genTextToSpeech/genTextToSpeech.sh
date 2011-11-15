#!/bin/sh

java -DgenRob.genControl.port=6007 -DgenRob.genControl.log=output -Duhh.fbi.tams.TextToSpeech.log=version,module,TTS -cp genRob.genControl.jar:uhh.fbi.tams.TextToSpeech.jar:uhh.fbi.tams.TextToSpeech.unit.jar genRob.genControl.Main uhh.fbi.tams.TextToSpeech.ModuleImpl
