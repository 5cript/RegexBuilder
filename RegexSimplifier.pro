#-------------------------------------------------
#
# Project created by QtCreator 2015-02-04T01:55:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++1y

TARGET = RegexSimplifier
TEMPLATE = app

#DEFINES += BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS=1 BOOST_WAVE_SUPPORT_CPP0X=1 BOOST_WAVE_SUPPORT_LONGLONG_INTEGER_LITERALS=1
INCLUDEPATH += "D:/Development/lib/5eeLib"
#LIBS += -L"D:/msys2/home/Tim/boost_1_57_0/stage/lib/" -lboost_system -lboost_wave -lboost_filesystem -lboost_thread
LIBS += -lboost_system-mt -lboost_wave-mt -lboost_filesystem-mt -lboost_thread-mt

SOURCES += main.cpp\
    regexbuilder/keywords.cpp \
    regexbuilder/regex.cpp \
    regexeditor.cpp \
    regexbuilder/group.cpp \
    regexbuilder/regexclass.cpp \
    regexbuilder/expander.cpp \
    regexbuilder/parserutil.cpp \
    regexbuilder/set.cpp \
    rexclasshighlighter.cpp \
    testbedhighlighter.cpp \
    regexbuilder/preprocessor.cpp \
    regexbuilder/tokencompat.cpp \
    regexbuilder/builder.cpp

HEADERS  += regexeditor.h \
    regexbuilder/regex.h \
    regexbuilder/token.h \
    tokencompat.h \
    regexbuilder/keywords.h \
    regexbuilder/parserutil.h \
    regexbuilder/group.h \
    regexbuilder/regexclass.h \
    regexbuilder/expander.h \
    regexbuilder/set.h \
    rexclasshighlighter.h \
    testbedhighlighter.h \
    regexbuilder/wave.h \
    regexbuilder/preprocessor.h \
    regexbuilder/tokencompat.h \
    regexbuilder/attribute_preprocessor.h \
    regexbuilder/builder.h \
    regexbuilder/builder_fwd.h

FORMS    += regexeditor.ui

DISTFILES += \
    ToDo_and_Ideas.txt

RESOURCES += \
    resource.qrc
