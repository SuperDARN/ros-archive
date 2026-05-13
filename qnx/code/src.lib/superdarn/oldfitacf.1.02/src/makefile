# makefile for fit_lib
#
#

INCLUDE= -I$(IPATH)/base -I$(IPATH)/general  -I$(IPATH)/superdarn/fit -I$(IPATH)/superdarn
SRC = oldacf_preproc.c oldacf_preproc.h olddbl_cmp.c olddbl_cmp.h \
      oldelevation.c oldelevation.h \
      oldelev_goose.c oldelev_goose.h \
      oldomega_guess.c oldomega_guess.h oldground_scatter.c \
      oldground_scatter.h oldbadsmp.h oldbadlags.h olddo_fit.h \
      oldmore_badlags.c oldmore_badlags.h  oldbadlags.c \
      oldremove_noise.c oldremove_noise.h oldcalc_phi_res.c oldcalc_phi_res.h \
      oldrang_badlags.c oldrang_badlags.h oldnoise_stat.c oldnoise_stat.h \
      oldnoise_acf.c oldnoise_acf.h oldfit_acf.c oldfit_acf.h \
      oldfit_noise.c oldfit_noise.h \
      olddo_fit.c oldfitacf.c fitacfversion.h    
OBJS = oldacf_preproc.o olddbl_cmp.o oldelevation.o oldelev_goose.o \
       oldomega_guess.o oldground_scatter.o oldmore_badlags.o oldbadlags.o \
       oldremove_noise.o oldcalc_phi_res.o \
       oldrang_badlags.o oldnoise_stat.o oldnoise_acf.o oldfit_acf.o \
       oldfit_noise.o \
       olddo_fit.o oldfitacf.o 
DSTPATH=$(LIBPATH)
OUTPUT=oldfitacf
LINK="1"
include $(MAKELIB)



