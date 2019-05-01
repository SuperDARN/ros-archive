# Makefile for cfit library
# ========================== 
# by R.J.Barnes
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC = cfitopen.c cfitclose.c cfitwrite.c cfitread.c cfitseek.c \
      cfitscan.c cfitcsd.c 
OBJS = cfitopen.o cfitclose.o cfitwrite.o cfitread.o cfitseek.o \
       cfitscan.o cfitcsd.o
LINK="1"
DSTPATH=$(LIBPATH)
OUTPUT = cfit
include $(MAKELIB)

