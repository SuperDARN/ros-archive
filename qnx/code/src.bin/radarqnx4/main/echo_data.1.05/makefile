# Makefile for echo_data
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base  -I$(IPATH)/radar
OBJS = echo.o
SRC=hlpstr.h errstr.h echo.c taskname.h echotask.h
OUTPUT = $(BINPATH)/echo_data
LIBS=-ltaskid.1  -lrmsgrcv.1 -lrtimer.1 -lerrlog.1 -lopt.1
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

  

 

