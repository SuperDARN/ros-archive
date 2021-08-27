# default .profile

# Environment variables for compiling the software  

export CODEBASE=$HOME/code

export MAKEBIN=$CODEBASE/make/makebin.qnx
export MAKELIB=$CODEBASE/make/makelib.qnx
export IPATH=$HOME/include
export LIBPATH=$HOME/lib
export BINPATH=$HOME/bin

# Environment variables for the general software 

export FONTPATH=$HOME/tables/general/fonts
export MAPDATA=$HOME/tables/general/map_data
export MAPOVERLAY=$HOME/tables/general/overlay.dat
export AACGM_DAT_PREFIX=$HOME/aacgm/aacgm_coeffs

# Environment variables for the superdarn software

export SD_LOGODATA=$HOME/tables/superdarn/logo.dat
export SD_HARDWARE=$HOME/tables/superdarn/hdw/hdw.dat
export SD_RADARNAME=$HOME/tables/superdarn/radar.dat
export SD_MODEL_TABLE=$HOME/tables/superdarn/model

# Environment variables for the radar software

export SD_ERRLOG_PATH="/data/errlogs"
export SD_CMP_PATH="/data/cmp"
export SD_SMR_PATH="/data/smr"
export SD_GRD_PATH="/data/grd"
export SD_FIT_PATH="/data/fit"
export SD_DAT_PATH="/data/dat"

export SD_RADARID=3
export SD_FREQ_TABLE=$HOME/tables/radar/restrict.dat

# environment variable for the operations code

export USR_CODEBASE=$HOME/usr/code
export USR_LIBPATH=$HOME/usr/lib
export USR_BINPATH=$HOME/usr/bin
export USR_IPATH=$HOME/usr/include


export PATH=$PATH:$BINPATH:$USR_BINPATH:$HOME/script     








