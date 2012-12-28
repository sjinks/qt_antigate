TARGET   = qtscript_antigate
QT      += script
QT      -= gui
CONFIG  += plugin release
TEMPLATE = lib

SOURCES += ../generated_cpp/antigate/plugin.cpp
exits(../generated_cpp/antigate/plugin.h) {
	HEADERS += ../generated_cpp/antigate/plugin.h
}

INCLUDEPATH += ../../src/

include(../generated_cpp/antigate/antigate.pri)
