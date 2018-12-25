LIBS += -L/usr/lib -lcal3d
INCLUDEPATH += /home/isadora/lib/Cal3D/src/

HEADERS = \
    glwidget.h \
    mainwindow.h \
    particle.h \
    particlespawner.h \
    timer.h \
    collider.h \
    navmesh.h \
    rendermesh.h \
    animatedcharacter.h

SOURCES = \
    main.cpp \
    glwidget.cpp \
    mainwindow.cpp \
    particle.cpp \
    particlespawner.cpp \
    timer.cpp \
    collider.cpp \
    navmesh.cpp \
    rendermesh.cpp \
    animatedcharacter.cpp

QT += gui opengl

CONFIG += c++11
QMAKE_CXXFLAGS += -O0 -g

DISTFILES += \
    shaders/basic.frag \
    shaders/basic.vert \
    shaders/particle.vert \
    shaders/particle.frag

FORMS += \
mainwindow.ui 


