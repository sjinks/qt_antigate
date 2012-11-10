QT       += network
QT       -= gui
TARGET    = antigate
TEMPLATE  = lib
CONFIG   += debug_and_release create_prl build_all
DEFINES  += ANTIGATE_LIBRARY

SOURCES += client.cpp
HEADERS += antigate.h client.h client_p.h
