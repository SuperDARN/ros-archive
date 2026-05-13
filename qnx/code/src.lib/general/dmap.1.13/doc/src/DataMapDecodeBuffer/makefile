# Makefile for DataMapDecodeBuffer
# ================================
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general 
IGNVER=1
OBJS = DataMapDecodeBuffer.o
OUTPUT = DataMapDecodeBuffer
LIBS=-lcnx.1 -ldmap.1 -lrcnv.1 

ifeq ($(OSTYPE),solaris)
SLIB =-lsocket -lnsl
endif

NET=$(shell find /usr/include/sys -name "socket.h")
ifeq ($(NET),/usr/include/sys/socket.h)
  include $(MAKEBIN)
else
  nonet:
        echo "TCP/IP development libraries not available."
endif
 
