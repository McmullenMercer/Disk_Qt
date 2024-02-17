TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../../TcpServer/TcpServer/opedb.cpp \
        main.c

HEADERS += \
    ../../TcpServer/TcpServer/opedb.h
