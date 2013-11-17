QT       += network
QT       -= gui
CONFIG += c++11

TARGET = safe
TEMPLATE = lib

DEFINES += SAFE_LIBRARY
SOURCES += safeapi.cpp \
    safeworker.cpp \
    safeapi_user.cpp \
    safeapi_filesystem.cpp \
    safeapi_locks.cpp \
    safeapi_share.cpp \
    safeapi_versions.cpp
HEADERS += safeapi.h \
    safeworker.h \
    safecalls.h \
    safeerrors.h \
    safetypes.h

include(test/test.pro)
