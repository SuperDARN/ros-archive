# Makefile for radops_dio
# =======================
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/radar
SUDO=1
OUTPUT = dio_drive
OBJS = DIO.o display.o reset.o do_op.o main.o bcd.o tsg.o logger.o \
       watchdog.o out_tsg.o out_tsg_x.o cnv_freq.o load_config.o \
       print_config.o default.o out_tsg_s.o out_tsg_sx.o 
SRC=  DIO.c display.c reset.c do_op.c main.c bcd.c tsg.c logger.c \
       watchdog.c out_tsg.c out_tsg_x.c cnv_freq.c load_config.c \
       print_config.c default.c out_tsg_s.c out_tsg_sx.c
VERSION=1 
LIBS = -lradar.1 -lfreq.1 -lgen.1
LFLAGS= -T1 -ltermlib
include $(MAKEBIN)

 


