#-------------------------------------------------
#
# Project created by QtCreator 2015-10-27T21:13:02
#
#-------------------------------------------------

QT       += core gui openglwidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BoidFlockingAlgorithm
TEMPLATE = app


SOURCES += main.cpp\
    MyOpenGLWidget.cpp \
           mainwindow.cpp\
           openclwrapper.cpp\
           platforminfodialog.cpp \
           boidsim.cpp \
#           boid_simulation.cl

HEADERS  += mainwindow.h\
            MyOpenGLWidget.h \
            openclwrapper.h\
            platforminfodialog.h \
            boid.h \
            boidsim.h

FORMS    += mainwindow.ui\
        platforminfodialog.ui

win32 {

INCLUDEPATH += C:\Intel\INDE\code_builder_5.2.0.65\include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/\
-L"C:\Intel\INDE\code_builder_5.2.0.65\lib\x64" \
-L"C:\PROGRA~2\Microsoft SDKs\Windows\v7.1A\Lib\x64" \
-lOpenCL -lopengl32 -lglu32

else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ \
-L"C:\Intel\INDE\code_builder_5.2.0.65\lib\x64" \
-L"C:\PROGRA~2\Microsoft SDKs\Windows\v7.1A\Lib\x64" \
-lOpenCL -lopengl32 -lglu32

}
!win32 {
    LIBS += -lOpenCL -lGLU
    INCLUDEPATH += /usr/include/

    CONFIG += c++11
}


INCLUDEPATH += $$PWD/../external/opencl/src/OpenCL/include
DEPENDPATH += $$PWD/../external/opencl/src/OpenCL/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../external/opencl/src/OpenCL/lib/ -lOpenCL
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../external/opencl/src/OpenCL/lib/ -lOpenCL

