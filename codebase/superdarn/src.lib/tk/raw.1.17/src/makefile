# Makefile for radar library
# ========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC=rawread.c rawwrite.c rawinx.c rawseek.c
OBJS = rawread.o rawwrite.o rawinx.o rawseek.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT = raw
LINK="1"



include $(MAKELIB).$(SYSTEM)
