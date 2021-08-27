# makefile for fit_lib
#
#



INCLUDE= -I$(IPATH)/general  -I$(IPATH)/superdarn/fit -I$(IPATH)/superdarn

OBJS = acf_preproc.o dbl_cmp.o elevation.o elev_goose.o \
       omega_guess.o ground_scatter.o more_badlags.o badlags.o \
       remove_noise.o calc_phi_res.o \
       rang_badlags.o noise_stat.o noise_acf.o fit_acf.o fit_noise.o \
       do_fit.o fitacf.o
OUTPUT=fitacf
LINK="1"
include $(MAKELIB)



