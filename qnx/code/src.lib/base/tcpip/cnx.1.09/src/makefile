# Makefile for cnx library
# ========================
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -DPOSIX -D__EXTENSIONS__
SRC=connex.c msg.c err.c
OBJS = connex.o msg.o err.o 
DSTPATH=$(LIBPATH)
OUTPUT = cnx
LINK="1"

ifdef TCPIPLIBS
include $(MAKELIB)
else 
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
clean:
	rm -f *.o
	rm -f $(OUTPUT)
endif





