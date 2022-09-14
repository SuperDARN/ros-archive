# Makefile for CSDClose
# ====================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
IGNVER=1
OBJS = CSDClose.o
OUTPUT = CSDClose
LIBS=-lcsd.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
