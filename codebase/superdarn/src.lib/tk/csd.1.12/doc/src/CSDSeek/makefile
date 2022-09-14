# Makefile for CSDSeek
# ====================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
IGNVER=1
OBJS = CSDSeek.o
OUTPUT = CSDSeek
LIBS=-lcsd.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
