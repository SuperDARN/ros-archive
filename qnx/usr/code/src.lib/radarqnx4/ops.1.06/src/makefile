# Makefile for ops library
# ========================
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
	 -I$(IPATH)/radarqnx4 -I$(USR_IPATH)/radarqnx4/ops
#VERSION=1
LINK="1"
OBJS = setup.o global.o raw.o reopen.o shell.o sync.o
SRC = setup.c global.c raw.c reopen.c shell.c sync.c default.h

OUTPUT = ops
DSTPATH=$(USR_LIBPATH)
include $(MAKELIB)
