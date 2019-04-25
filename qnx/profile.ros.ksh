# default .profile.ros.ksh


#####################################################################
#                                                                   #
# Base Installation directory                                       #
#                                                                   #
#####################################################################

# Note. Uncomment the following line and set it to the directory
# that the software is RST software is installed in.

#export HOMEPATH=/home/radar


#####################################################################
#                                                                   #
# Directory Paths                                                   #
#                                                                   #
#####################################################################

export CODEBASE=${HOMEPATH}/code
export LOGPATH=${HOMEPATH}/log
export DOCPATH=${HOMEPATH}/doc

export LIBPATH=${HOMEPATH}/lib
export BINPATH=${HOMEPATH}/bin
export IPATH=${HOMEPATH}/include

export USR_CODEBASE=${HOMEPATH}/usr/code

export USR_LIBPATH=${HOMEPATH}/usr/lib
export USR_BINPATH=${HOMEPATH}/usr/bin
export USR_IPATH=${HOMEPATH}/usr/include
export PATH=${PATH}:${HOMEPATH}/bin:${HOMEPATH}/script:${HOMEPATH}/usr/bin

#####################################################################
#                                                                   #
# Base Data Tables                                                  #
#                                                                   #
#####################################################################

export FONTPATH="$HOMEPATH/tables/base/fonts"
export FONTDB="$HOMEPATH/tables/base/fonts/fontdb.xml"

#####################################################################
#                                                                   #
# ROS specific environment variables                                #
#                                                                   #
#####################################################################

export OSTYPE="qnx4"
export TCPIPLIBS=" "
export SD_HDWPATH="$HOMEPATH/tables/superdarn/hdw/"
export SD_RADAR="$HOMEPATH/tables/superdarn/radar.dat"

export SD_ERRLOG_PATH="/data/errlogs"
export SD_CMP_PATH="/data/cmp"
export SD_SMR_PATH="/data/smr"
export SD_GRD_PATH="/data/grd"
export SD_FIT_PATH="/data/fit"
export SD_RAW_PATH="/data/raw"
export SD_IQ_PATH="/data/iq"

#####################################################################
#                                                                   #
# Location of makefiles                                             #
#                                                                   #
#####################################################################
export MAKEBIN=${CODEBASE}/make/makebin.qnx4
export MAKELIB=${CODEBASE}/make/makelib.qnx4
export MAKELIBIDL=${CODEBASE}/make/makelibidl.qnx4
export SITELIB=${CODEBASE}/make/sitelib
