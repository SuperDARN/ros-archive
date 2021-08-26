# Makefile for radar_shell
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/radarqnx4
OBJS = main.o shell.o
SRC=hlpstr.h errstr.h shell.c shell.h main.c taskname.h
VERSION=1
LIBS=-lopt.1
OUTPUT = $(BINPATH)/radar_shell
ifeq ($(OSTYPE),qnx4)
include $(MAKEBIN)
else
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
	$(BINPATH)/rcsv -h version.info > version.h
clean:
	rm -f *.o
	rm -f $(OUTPUT)
endif
