# Makefile for data_feed
# ======================
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/general -I$(IPATH)/superdarn \
		-I$(IPATH)/superdarn/fit -I$(IPATH)/radar \
        -I$(IPATH)/radar/int_lib \
        -I$(IPATH)/radar/fclr_lib \
        -I$(USR_IPATH)/s_ops
LINK="1"
OBJS = do_pulse_s.o do_radar_s.o radar_s.o global_s.o tmseq_s.o setup.o \
       get_status_s.o shell_s.o build_raw_s.o read_data_s.o  \
       calc_skip.o init_proxy.o test_reopen.o wait_boundary.o \
       read_uconts.o fclr_s.o base_fclr_s.o do_fclr_s.o
SRC=   do_pulse_s.c do_radar_s.c radar_s.c global_s.c tmseq_s.c setup.c \
       get_status_s.c shell_s.c build_raw_s.c read_data_s.c  \
       calc_skip.c init_proxy.c test_reopen.c wait_boundary.c \
       read_uconts.c fclr_s.c base_fclr_s.c do_fclr_s.c
OUTPUT = ops_s
DSTPATH=$(USR_LIBPATH)
include $(MAKELIB)








 

 

















