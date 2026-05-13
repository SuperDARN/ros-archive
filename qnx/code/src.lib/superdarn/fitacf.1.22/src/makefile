# makefile for fit_lib
#
#



INCLUDE= -I$(IPATH)/base -I$(IPATH)/general  -I$(IPATH)/superdarn/fit -I$(IPATH)/superdarn
SRC = acf_preproc.c acf_preproc.h dbl_cmp.c dbl_cmp.h elevation.c elevation.h \
      elev_goose.c elev_goose.h \
      omega_guess.c omega_guess.h ground_scatter.c ground_scatter.h \
      more_badlags.c more_badlags.h  badlags.c\
      badlags_s.c remove_noise.c remove_noise.h calc_phi_res.c calc_phi_res.h \
      rang_badlags.c rang_badlags.h noise_stat.c noise_stat.h \
      noise_acf.c noise_acf.h fit_acf.c fit_acf.h fit_noise.c fit_noise.h \
       do_fit.c do_phase_fit.c do_phase_fit.h fitacf.c fitacfversion.h    
OBJS = acf_preproc.o dbl_cmp.o elevation.o elev_goose.o \
       omega_guess.o ground_scatter.o more_badlags.o badlags.o badlags_s.o \
       remove_noise.o calc_phi_res.o \
       rang_badlags.o noise_stat.o noise_acf.o fit_acf.o fit_noise.o \
       do_fit.o do_phase_fit.o fitacf.o 
DSTPATH=$(LIBPATH)
OUTPUT=fitacf
LINK="1"
include $(MAKELIB)



