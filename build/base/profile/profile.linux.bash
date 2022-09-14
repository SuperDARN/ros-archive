# default profile.darwin

export OSTYPE="linux"
export SYSTEM="linux"
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

export NETCDF_PATH="/usr"

# pathname for the CDF software

export CDF_PATH="/usr/local/cdf"

# IDL header directory

export IDL_IPATH="/usr/local/itt/idl/external/include"

#####################################################################
#                                                                   #
# IDL Configuration                                                 #
#                                                                   #
#####################################################################

export IDL_PATH="+/usr/local/itt:+/${HOMEPATH}/idl/lib"
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

export PATH=${PATH}:${BUILD}/base/bin:${HOMEPATH}/bin:${HOMEPATH}/usr/bin:${HOMEPATH}/script

#####################################################################
#                                                                   #
# Location of pnmtopng                                              #
#                                                                   #
#####################################################################

export PNMTOPNG="/usr/bin/pnmtopng"

#####################################################################
#                                                                   #
# Base Data Tables                                                  #
#                                                                   #
#####################################################################

export FONTPATH="$HOMEPATH/tables/base/fonts"
export FONTDB="$HOMEPATH/tables/base/fonts/fontdb.xml"

#####################################################################
#                                                                   #
# Location of makefiles                                             #
#                                                                   #
#####################################################################

export MAKECFG=${BUILD}/base/make/makecfg
export MAKEBIN=${BUILD}/base/make/makebin
export MAKELIB=${BUILD}/base/make/makelib
export MAKELIBIDL=${BUILD}/base/make/makelibidl
