#-------------------------------------------------
#
# Project created by QtCreator 2018-08-24T12:09:16
#
#-------------------------------------------------
include(code_generator_tests/gtest_dependency.pri)

QT       += core gui widgets serialport

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_USE_QSTRINGBUILDER

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++1z no_keywords

INCLUDEPATH += C:/boost_mingw/boost/include/boost-1_67
INCLUDEPATH += include

HEADERS += include/ast.h \
           include/code_generator.h \
           include/command_runner.h \
           include/cpp_file_generator.h \
           include/msg_types_file_generator.h \
           include/oil_file_generator.h \
           include/parser.h \
           include/replacer.h \
           include/util.h

test {
    TARGET = test
    CONFIG += console
    CONFIG -= app_bundle
    CONFIG += thread
    HEADERS += code_generator_tests/util_test.h \
                code_generator_tests/msg_queue_test.h \
                code_generator_tests/target_header_files_shadow/message_queue.h \
                code_generator_tests/target_header_files_shadow/windows_types.h \
                code_generator_tests/target_header_files_shadow/J1939Includes.h \
                code_generator_tests/mocks/mutual_exclusion_handler_mock.h
    SOURCES += code_generator_tests/main_test.cpp
    RESOURCES += code_generator_tests/test_resources.qrc
} else {
    TARGET = code_generator
    HEADERS += code_generator_wizard.h
    SOURCES += main.cpp \
               code_generator_wizard.cpp
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
