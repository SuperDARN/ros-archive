# Makefile for aacgm library
# ==========================
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general

SRC = math.c math.h aacgm.c altitude_to_cgm.h altitude_to_cgm.c \
      cgm_to_altitude.c cgm_to_altitude.h coeff.c \
       convert_geo_coord.c convert_geo_coord.h eqn_of_time.c eqn_of_time.h \
       mlt.c mlt.h mlt1.c mlt1.h rylm.c rylm.h\
       solar_loc.c solar_loc.h default.h   
OBJS = math.o aacgm.o altitude_to_cgm.o cgm_to_altitude.o coeff.o \
       convert_geo_coord.o eqn_of_time.o mlt.o mlt1.o rylm.o \
       solar_loc.o
DSTPATH=$(LIBPATH)
LINK="1"
OUTPUT = aacgm
include $(MAKELIB)

 

 





