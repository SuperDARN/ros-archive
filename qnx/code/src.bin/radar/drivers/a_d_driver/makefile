# Makefile for a_d_driver
# =======================
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/radar
OUTPUT = a_d_drive
SUDO=1
OBJS = dma-alloc.o dt2828.c dma_init.o main.o dt_reset.o set_clock.o \
       dt_int_handler.o dt_etrig.o dt_strig.o dt_int.o 
LIBS=-lgen.1
CFLAGS=-T1
include $(MAKEBIN)
 