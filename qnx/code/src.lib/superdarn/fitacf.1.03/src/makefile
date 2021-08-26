# makefile for fit_lib
#
#



INCLUDE= -I$(IPATH)/general  -I$(IPATH)/superdarn/fit -I$(IPATH)/superdarn
SRC = acf_preproc.c dbl_cmp.c elevation.c elev_goose.c \
       omega_guess.c ground_scatter.c more_badlags.c badlags.c \
       remove_noise.c calc_phi_res.c \
       rang_badlags.c noise_stat.c noise_acf.c fit_acf.c fit_noise.c \
       do_fit.c fitacf.c    
OBJS = acf_preproc.o dbl_cmp.o elevation.o elev_goose.o \
       omega_guess.o ground_scatter.o more_badlags.o badlags.o \
       remove_noise.o calc_phi_res.o \
       rang_badlags.o noise_stat.o noise_acf.o fit_acf.o fit_noise.o \
       do_fit.o fitacf.o
OUTPUT=fitacf
LINK="1"
include $(MAKELIB)



