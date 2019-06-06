SOURCES += $$PWD/lc_logging.cpp
HEADERS += $$PWD/lc_logging.h

INCLUDEPATH += $$PWD

# Necessary to get the symbols when logging the stack trace.
!windows {
QMAKE_LFLAGS += -rdynamic
}
