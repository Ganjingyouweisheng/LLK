#-------------------------------------------------
#
# Project created by QtCreator 2024-12-30T10:50:28
#
#-------------------------------------------------

QT       += core gui multimedia sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtLianliankan
TEMPLATE = app


SOURCES += main.cpp\
    auto_conductor.cpp \
    database_test.cpp \
    logindialog.cpp \
        main_game_window.cpp \
    game_model.cpp \
    overdialog.cpp \
    system_database.cpp \
    user_database.cpp

HEADERS  += main_game_window.h \
    auto_conductor.h \
    game_model.h \
    logindialog.h \
    overdialog.h \
    system_database.h \
    user_database.h

FORMS    += main_game_window.ui \
    logindialog.ui \
    overdialog.ui

RESOURCES += \
    res.qrc
