#!/bin/sh

cd /usr/local/src/service_robot/mobile2/mobile || exit 1

rm -f obj.i586-linux/genBase.o obj.i586-linux/laserFeeder.o mobiled


cd .. || exit 1

make



