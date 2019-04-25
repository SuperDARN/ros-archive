# Makefile for gc214TShf
# ======================
# Author: R.J.Barnes

#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/radarqnx4
OUTPUT = $(BINPATH)/gc214TSif
SUDO=1

OBJS = dma-alloc.o io.o pci.o dma.o gc214.o gc4016.o build.o filter.o main.o
SRC=io.c io.h hlpstr.h dma.c dma.h pci.c pci.h errstr.h gc4016.c gc4016.h filter.c filter.h gc214.c gc214card.h registers.h main.c taskname.h build.c build.h dma-alloc.c


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
