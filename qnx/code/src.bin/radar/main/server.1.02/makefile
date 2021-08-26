# Makefile for server
# ===================
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/radar
OBJS = msg_io.o msg_mem.o connect.o socket.o cmp_fit.o \
			  fork.o transmit.o server.o
SRC= msg_io.c msg_mem.c connect.c socket.c cmp_fit.c \
			  fork.c transmit.c server.c
VERSION=1
OUTPUT = $(BINPATH)/server
LIBS=-lecho.1 -lmsg.1 -lsys.1 -lradar.1 -lgen.1
SLIB=-lsocket
NET=$(shell find /usr/include/sys -name "socket.h")
ifeq ($(NET),/usr/include/sys/socket.h)
  include $(MAKEBIN) 
else 
  nonet:
	echo "TCP/IP development libraries not available."
endif

 









 

