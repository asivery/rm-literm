QT = core gui qml quick 

CONFIG -= app_bundle

MOC_DIR = .moc
OBJECTS_DIR = .obj

CONFIG += shared link_pkgconfig

TEMPLATE = lib
TARGET = literm
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lutil

# Input
HEADERS += \
    ptyiface.h \
    terminal.h \
    textrender.h \
    version.h \
    utilities.h \
    keyloader.h \
    parser.h

SOURCES += \
    xoviout/xovi.c \
    main.cpp \
    terminal.cpp \
    textrender.cpp \
    ptyiface.cpp \
    utilities.cpp \
    keyloader.cpp \
    parser.cpp

OTHER_FILES += \
    qml/mobile/Main.qml \
    qml/mobile/Keyboard.qml \
    qml/mobile/Key.qml \
    qml/mobile/Lineview.qml \
    qml/mobile/Button.qml \
    qml/mobile/MenuLiterm.qml \
    qml/mobile/NotifyWin.qml \
    qml/mobile/UrlWindow.qml \
    qml/mobile/LayoutWindow.qml \
    qml/mobile/PopupWindow.qml

RESOURCES += \
    resources.qrc

target.path = /usr/bin
INSTALLS += target

isEmpty(LITERM_TARGET): LITERM_TARGET=desktop
!include(targets/$${LITERM_TARGET}.pri): error("Can't load LITERM_TARGET definition: $$LITERM_TARGET")
