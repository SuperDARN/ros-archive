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
LIBS = -lradar.1 -lfreq.1 -lgen.1
LFLAGS= -T1 -ltermlib
include $(MAKEBIN)

 


