TARGET = rssevents
QT += dbus network
QT -= gui

#DEFINES += DEBUG

HEADERS += \
    src/feedparser.h \
    src/rssevents.h

SOURCES += \
    src/feedparser.cpp \
    src/main.cpp \
    src/rssevents.cpp

desktop.files = desktop/rssevents.desktop
desktop.path = /opt/hildonevents/settings

settings.files = src/SettingsDialog.qml
settings.path = /opt/rssevents/qml

target.path = /opt/rssevents/bin

INSTALLS += \
    target \
    desktop \
    settings
