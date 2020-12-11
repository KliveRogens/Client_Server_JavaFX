#-------------------------------------------------
#
# Project created by CalcElementProjectGenerator
#
#-------------------------------------------------

QT       -= gui

win32:CONFIG(release, debug|release): TARGET = TR_VAPE_09H_AD3749
else:win32:CONFIG(debug, debug|release): TARGET = TR_VAPE_09H_AD3749d
TEMPLATE = lib

SOURCES += TR_VAPE_09H_AD3749.cpp \
    ../TR_VAPE_CALC/tr_vape_calc.cpp

HEADERS += TR_VAPE_09H_AD3749.h \
    ../../include/IPort.h \
    ../../CalcElement/CalcElement.h \
    ../TR_VAPE_CALC/tr_vape_calc.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../target/release/calcLibrary -lPort
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../target/debug/calcLibrary -lPortd

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../target/release/calcLibrary -lCalcElement
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../target/debug/calcLibrary -lCalcElementd

win32:CONFIG(release, debug|release): DESTDIR = $$PWD/../../../target/release/calcLibrary
else:win32:CONFIG(debug, debug|release): DESTDIR = $$PWD/../../../target/debug/calcLibrary