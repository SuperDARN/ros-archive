# Makefile for errlog
# ===================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base
OBJS = errlog.o
LIBS =-lopt.1
SRC=hlpstr.h errlog.c errstr.h taskname.h
LIBS =-lopt.1
OUTPUT = $(BINPATH)/errlog
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

  
