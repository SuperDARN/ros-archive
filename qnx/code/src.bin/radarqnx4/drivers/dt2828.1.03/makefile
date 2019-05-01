# Makefile for dt2828
# ===================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/radar
OUTPUT = $(BINPATH)/dt2828
SUDO=1
OBJS = dma-alloc.o dt2828.o dma_init.o main.o dt_reset.o set_clock.o \
       dt_int_handler.o dt_etrig.o dt_strig.o dt_int.o 
SRC=dt.h hlpstr.h task_name.h dma.h errstr.h set_clock.c dt_reset.c dt_etrig.c main.c dt_ext.h dt_strig.c pragma.h dt_int.c dma_init.c dt_int_handler.c dt2828.c dt2828.h dma-alloc.c
DRIVER=1
LIBS=-lopt.1
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
