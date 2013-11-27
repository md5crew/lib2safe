QT       += network
CONFIG += c++11

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

SOURCES += $$PWD/safeapi.cpp \
    $$PWD/safeworker.cpp \
    $$PWD/safeapi_user.cpp \
    $$PWD/safeapi_filesystem.cpp \
    $$PWD/safeapi_locks.cpp \
    $$PWD/safeapi_share.cpp \
    $$PWD/safeapi_versions.cpp
HEADERS += $$PWD/safeapi.h \
    $$PWD/safeworker.h \
    $$PWD/safecalls.h \
    $$PWD/safeerrors.h \
    $$PWD/safetypes.h
