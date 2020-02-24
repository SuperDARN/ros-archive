# Makefile for normal_scan_1200
# =============================
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/radar \
	-I$(USR_IPATH)/ops -I$(USR_IPATH)/site.$(SD_RADARCODE)

BASE=normal_scan_1200
EXEBASE=$(BASE)
#EXEBASE=$(BASE)ORG

OBJS = $(BASE).o
SRC=$(BASE).c
SUDO=1
VERSION=1
OUTPUT = $(USR_BINPATH)/$(EXEBASE)
LIBS= -l$(SD_RADARCODE).site.1 -lops.1 -lrs.1 -lfclr.1 -lint.1 -lfreq.1 -lmsg.1 -lsys.1 -lad.1 -ldio.1 -ltsg.1 -lraw.1 -lradar.1 -lfitacf.1 -lgen.1 
include $(MAKEBIN)


