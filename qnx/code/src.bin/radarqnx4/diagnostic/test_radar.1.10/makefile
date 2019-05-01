# Makefile for test_radar
# =======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
        -I$(IPATH)/radar
OBJS = test_radar.o help.o do_op.o read_tsg.o do_tsg.o
SRC=hlpstr.h task_name.h do_op.c do_op.h errstr.h read_tsg.c read_tsg.h help.c help.h do_tsg.c do_tsg.h test_radar.c
OUTPUT = $(BINPATH)/test_radar
LIBS=-lad.1 -ldio.1 -lrtimer.1 -lopt.1
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

