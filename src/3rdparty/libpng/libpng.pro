TARGET = qtlibpng

CONFIG += \
    static \
    hide_symbols \
    exceptions_off rtti_off warn_off \
    installed

MODULE_INCLUDEPATH = $$PWD
MODULE_EXT_HEADERS = png.h pngconf.h

load(qt_helper_lib)

DEFINES += PNG_ARM_NEON_OPT=0 \
           PNG_POWERPC_VSX_OPT=0 \
           PNG_IMPEXP= \
           _CRT_SECURE_NO_DEPRECATE

SOURCES += \
    png.c \
    pngerror.c \
    pngget.c \
    pngmem.c \
    pngpread.c \
    pngread.c \
    pngrio.c \
    pngrtran.c \
    pngrutil.c \
    pngset.c \
    pngtrans.c \
    pngwio.c \
    pngwrite.c \
    pngwtran.c \
    pngwutil.c

TR_EXCLUDE += $$PWD/*

include(../zlib_dependency.pri)
