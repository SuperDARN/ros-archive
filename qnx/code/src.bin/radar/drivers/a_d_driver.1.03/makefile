# Makefile for a_d_driver
# =======================
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/radar
OUTPUT = $(BINPATH)/a_d_drive
SUDO=1
OBJS = dma-alloc.o dt2828.o dma_init.o main.o dt_reset.o set_clock.o \
       dt_int_handler.o dt_etrig.o dt_strig.o dt_int.o 
SRC= dma-alloc.c dt2828.c dma_init.c main.c dt_reset.c set_clock.c \
       dt_int_handler.c dt_etrig.c dt_strig.c dt_int.c
VERSION=1 
LIBS=-lgen.1
CFLAGS=-T1
include $(MAKEBIN)
 