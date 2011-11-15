######################################################################
# Automatically generated by qmake (2.01a) Tue Feb 5 17:08:53 2008
######################################################################

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . ../ ../devices ../canbus
QT += network

# Input
HEADERS +=	canserver.h \
		../logger.h \
		../devices/battery.h \
		../devices/drive.h \
		../devices/remotecontrol.h \
		../canbus/can.h \
		../canbus/canmessage.h \
		../canbus/candevice.h \
		../packet.h \
		../configuration.h \
		../conversion.h \
		../pose.h

SOURCES +=	canserver.cpp \
		../logger.cpp \
		../devices/battery.cpp \
		../devices/drive.cpp \
		../devices/remotecontrol.cpp \
		../canbus/can.cpp \
		../canbus/canmessage.cpp \
		../canbus/candevice.cpp \
		../packet.cpp \
		../configuration.cpp \
		../conversion.cpp \
		../pose.cpp
