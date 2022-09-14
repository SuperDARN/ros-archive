# Makefile for iqdata
# ==================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC=iqread.c iqwrite.c iqseek.c iqinx.c
OBJS = iqread.o iqwrite.o iqseek.o iqinx.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)

OUTPUT = iqdata
LINK="1"


include $(MAKELIB).$(SYSTEM)
