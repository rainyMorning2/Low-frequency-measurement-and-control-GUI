QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11
DEFINES += QCUSTOMPLOT_USE_OPENGL

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customdatatrans.cpp \
    datatrans.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    threadTcp.cpp

HEADERS += \
    datatrans.h \
    customdatatrans.h \
    mainwindow.h \
    qcustomplot.h \
    threadTcp.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
win32:LIBS += -lOpengl32
-lglu32
# LIBS+=D:\workSpace\QtProjects\build-dataTrans-Desktop_Qt_5_15_0_MinGW_64_bit-Release\release\dataTrans.dll
