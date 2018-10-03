HEADERS = \
    mesh.h \
    glwidget.h \
    mainwindow.h \
    plarticle.h

SOURCES = \
          main.cpp \
    mesh.cpp \
    glwidget.cpp \
    mainwindow.cpp

QT += gui opengl

CONFIG += c++11

DISTFILES += \
    shaders/basic.frag \
    shaders/basic.vert

FORMS += \
    mainwindow.ui
