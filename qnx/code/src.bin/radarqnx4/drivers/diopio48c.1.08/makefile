# Makefile for diopio48c
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/radarqnx4
SUDO=1
OUTPUT = $(BINPATH)/diopio48c
OBJS = DIO.o display.o reset.o do_op.o main.o bcd.o tsg.o logger.o \
       watchdog.o out_tsg.o out_tsg_x.o cnv_freq.o load_config.o \
       print_config.o default.o out_tsg_s.o out_tsg_sx.o locate_card.o 
SRC=out_tsg_s.c out_tsg_s.h out_tsg_x.c out_tsg_x.h hlpstr.h display.c display.h DIO.c DIO.h reset.c reset.h task_name.h bcd.c bcd.h tsg.c tsg.h do_op.c do_op.h errstr.h dio_msg.h config.h out_tsg.c out_tsg.h load_config.c print_config.c main.c main.h get_status.h default.c port.h out_tsg_sx.c out_tsg_sx.h cnv_freq.c cnv_freq.h PIO48.h watchdog.c watchdog.h logger.c logger.h locate_card.c locate_card.h
LIBS = -lradar.1 -lfreq.1 -lopt.1
DRIVER=1
LFLAGS= -ltermlib
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

 

