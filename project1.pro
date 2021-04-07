QT       += core gui
QT       += charts
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11
DEFINES += QCUSTOMPLOT_USE_OPENGL

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    analogDisplay.cpp \
    customscrollbar.cpp \
    customtab.cpp \
    customtabwidget.cpp \
    customtext.cpp \
    main.cpp \
    mainwindow.cpp \
    modeControl.cpp \
    qcustomplot.cpp \
    stateDisplay.cpp \
    tcpConnection.cpp \
    threadTcp.cpp \
    udpServer.cpp

HEADERS += \
    customscrollbar.h \
    customtab.h \
    customtabwidget.h \
    customtext.h \
    mainwindow.h \
    qcustomplot.h \
    threadTcp.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    project1_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
win32:LIBS += -lOpengl32
-lglu32
unix:LIBS += -lglut -lGLU
