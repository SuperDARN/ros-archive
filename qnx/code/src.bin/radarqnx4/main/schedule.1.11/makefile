# Makefile for scheduler
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/radarqnx4
OBJS = log_info.o load_schedule.o print_schedule.o set_schedule.o \
       test_schedule.o check_program.o refresh.o execute.o main.o 
SRC=hlpstr.h errstr.h log_info.c log_info.h execute.c execute.h main.c check_program.c check_program.h print_schedule.c taskname.h load_schedule.c refresh.c refresh.h schedule.h set_schedule.c test_schedule.c
SUDO=1
OUTPUT = $(BINPATH)/schedule 
LIBS=-lrtime.1 -lopt.1
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

  

 
