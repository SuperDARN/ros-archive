#####################################################################
#                                                                   #
# SuperDARN Data Tables                                             #
#                                                                   #
#####################################################################

export SD_HDWPATH="$HOMEPATH/tables/superdarn/hdw/"
export SD_RADAR="$HOMEPATH/tables/superdarn/radar.dat"

export AACGM_DAT_PREFIX="$HOMEPATH/tables/aacgm/aacgm_coeffs"
export IGRF_PATH="$HOMEPATH/tables/mag/"
export SD_MODEL_TABLE="$HOMEPATH/tables/superdarn/model"

#####################################################################
#                                                                   #
# IDL Libraries                                                     #
#                                                                   #
#####################################################################

export LIB_FITIDL="${HOMEPATH}/lib/libfitidl.1.so"
export LIB_IQIDL="${HOMEPATH}/lib/libiqidl.1.so"
export LIB_RAWIDL="${HOMEPATH}/lib/librawidl.1.so"
export LIB_OLDFITIDL="${HOMEPATH}/lib/liboldfitidl.1.so"
export LIB_OLDRAWIDL="${HOMEPATH}/lib/liboldrawidl.1.so"
export LIB_RPOSIDL="${HOMEPATH}/lib/librposidl.1.so"
export LIB_AACGMIDL="${HOMEPATH}/lib/libaacgmidl.1.so"
export LIB_IGRFIDL="${HOMEPATH}/lib/libigrfidl.1.so"
export LIB_GEOPIDL="${HOMEPATH}/lib/libgeopidl.1.so"

#####################################################################
#                                                                   #
# Legacy environment variables                                      #
#                                                                   #
#####################################################################

export SD_TABLES="${HOMEPATH}/tables/idl"
export SD_RAWROPEN_PATH=":/data/raw/:."
export SD_FITROPEN_PATH=":/data/fit/:."
export SD_IDL="${HOMEPATH}/idl/"

#####################################################################
#                                                                   #
# Radar Operating System environment variables                      #
#                                                                   #
#####################################################################

export SD_ERRLOG_PATH="/data/ros/errlogs"
export SD_FIT_PATH="/data/ros/fit"
export SD_RAW_PATH="/data/ros/raw"
export SD_IQ_PATH="/data/ros/iq"

export SD_FREQ_TABLE=$HOMEPATH/tables/superdarn/site/site.tst/restrict.dat
