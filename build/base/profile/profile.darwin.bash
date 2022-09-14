# default profile.darwin

export OSTYPE="darwin"
export SYSTEM="darwin"
export EDITOR="emacs"

#####################################################################
#                                                                   #
# Base Installation directory                                       #
#                                                                   #
#####################################################################

# Note. Uncomment the following line and set it to the directory
# that the software is RST software is installed in.

if test -z "${HOMEPATH}"
then
  export HOMEPATH=$HOME
fi


#####################################################################
#                                                                   #
# Compilation directives                                            #
#                                                                   #
#####################################################################

# Libraries for TCPIP

export TCPIPLIBS=" "

# Path of the X11 packages

export XPATH="/usr/X11R6"

# Compile netCDF software

#export NETCDF_PATH="/opt/local/include"

# pathname for the CDF software

export CDF_PATH="/Applications/cdf31-dist"

# IDL header directory

export IDL_IPATH="/Applications/itt/idl/external/include"

#####################################################################     
#                                                                   #
# Library directories                                               #
#                                                                   #
#####################################################################   

export DYLD_LIBRARY_PATH=.:/Applications/cdf31-dist/lib

#####################################################################
#                                                                   #
# IDL Configuration                                                 #
#                                                                   #
#####################################################################

export IDL_PATH="+/Applications/itt:+/${HOMEPATH}/idl/lib"
export IDL_STARTUP="${HOMEPATH}/idl/startup.pro"

#####################################################################
#                                                                   #
# Directory Paths                                                   #
#                                                                   #
#####################################################################

export BUILD="${HOMEPATH}/build"
export CODEBASE="${HOMEPATH}/codebase"
export LOGPATH="${HOMEPATH}/log"
export DOCPATH="${HOMEPATH}/doc"

export WWWPATH="/www/htdocs/doc"
export URLBASE="/doc"

export LIBPATH="${HOMEPATH}/lib"
export BINPATH="${HOMEPATH}/bin"
export IPATH="${HOMEPATH}/include"

export USR_CODEBASE="${HOMEPATH}/usr/codebase"
export USR_LIBPATH="${HOMEPATH}/usr/lib"
export USR_BINPATH="${HOMEPATH}/usr/bin"
export USR_IPATH="${HOMEPATH}/usr/include"

export PATH=${PATH}:/opt/local/bin:${HOMEPATH}/bin:${HOMEPATH}/usr/bin:${HOMEPATH}/script:${BUILD}/base/bin:/Applications/itt/idl/bin:/Applications/cdf31-dist/bin

#####################################################################
#                                                                   #
# Location of pnmtopng                                              #
#                                                                   #
#####################################################################

export PNMTOPNG="/opt/local/bin/pnmtopng"

#####################################################################
#                                                                   #
# Base Data Tables                                                  #
#                                                                   #
#####################################################################

export FONTPATH="$HOMEPATH/tables/base/fonts"
export FONTDB="$HOMEPATH/tables/base/fonts/fontdb.xml"
export MAPDATA="$HOMEPATH/tables/general/map_data"
export BNDDATA="$HOMEPATH/tables/general/bnd_data"
export MAPOVERLAY="$HOMEPATH/tables/general/overlay.dat"

#####################################################################
#                                                                   #
# Location of makefiles                                             #
#                                                                   #
#####################################################################
export MAKECFG=${BUILD}/base/make/makecfg
export MAKEBIN=${BUILD}/base/make/makebin
export MAKELIB=${BUILD}/base/make/makelib
export MAKELIBIDL=${BUILD}/base/make/makelibidl
#####################################################################
#                                                                   #
# SuperDARN Data Tables                                             #
#                                                                   #
#####################################################################

export SD_HDWPATH="$HOMEPATH/tables/superdarn/hdw/"
export SD_RADAR="$HOMEPATH/tables/superdarn/radar.dat"

export AACGM_DAT_PREFIX="$HOMEPATH/tables/analysis/aacgm/aacgm_coeffs"
export IGRF_PATH="$HOMEPATH/tables/analysis/mag/"
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