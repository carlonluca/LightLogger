#
# Author:  Luca Carlon
# Company: -
# Date:    08.25.2013
#

!android {
QT       += core gui sql qml quick
}
else {
QT       -= core
LIBS     += -lgnustl_shared
}

TARGET   = LightLogger
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES  += main.cpp applogging.cpp
HEADERS  += lc_logging_qt.h applogging.h

DEFINES  += BUILD_LOG_LEVEL_INFORMATION ENABLE_CODE_LOCATION

windows {
LIBS     += -lDbghelp
}

# Necessary to get the symbols when logging the stack trace.
!windows {
QMAKE_LFLAGS += -rdynamic
}

OTHER_FILES += \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/AndroidManifest.xml \
    android/res/values-rs/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/drawable/logo.png \
    android/res/drawable/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-hdpi/icon.png \
    android/res/layout/splash.xml \
    android/res/values-el/strings.xml \
    main.qml \
    lc_logging.h \
    lc_logging_qt.h \
    lc_logging_utils.h

RESOURCES += \
    resources.qrc
