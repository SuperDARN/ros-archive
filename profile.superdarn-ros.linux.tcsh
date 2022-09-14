# default profile.linux.tcsh

setenv OSTYPE "linux"
setenv SYSTEM "linux"
setenv EDITOR "emacs"

#####################################################################
#                                                                   #
# Base Installation directory                                       #
#                                                                   #
#####################################################################

# Note. Uncomment the following line and set it to the directory
# that the software is RST software is installed in.

  setenv HOMEPATH $HOME

#####################################################################
#                                                                   #
# Compilation directives                                            #
#                                                                   #
#####################################################################

# Libraries for TCPIP

#setenv TCPIPLIBS " "

# Path of the X11 packages

#setenv XPATH "/usr/X11R6"

# Compile netCDF software

#setenv NETCDF_PATH "/usr"

# pathname for the CDF software

#setenv CDF_PATH "/usr/local/cdf"

# IDL header directory

#setenv IDL_IPATH "/usr/local/itt/idl/external/include"

#####################################################################
#                                                                   #
# IDL Configuration                                                 #
#                                                                   #
#####################################################################

setenv IDL_PATH "+/usr/local/itt:+/${HOMEPATH}/idl/lib"
setenv IDL_STARTUP "${HOMEPATH}/idl/startup.pro"

#####################################################################
#                                                                   #
# Directory Paths                                                   #
#                                                                   #
#####################################################################

setenv BUILD "${HOMEPATH}/build"
setenv CODEBASE "${HOMEPATH}/codebase"
setenv LOGPATH "${HOMEPATH}/log"
setenv DOCPATH "${HOMEPATH}/doc"

setenv WWWPATH "/www/htdocs/doc"
setenv URLBASE "/doc"

setenv LIBPATH "${HOMEPATH}/lib"
setenv BINPATH "${HOMEPATH}/bin"
setenv IPATH "${HOMEPATH}/include"

setenv USR_CODEBASE "${HOMEPATH}/usr/codebase"
setenv USR_LIBPATH "${HOMEPATH}/usr/lib"
setenv USR_BINPATH "${HOMEPATH}/usr/bin"
setenv USR_IPATH "${HOMEPATH}/usr/include"

setenv PATH ${PATH}:${BUILD}/base/bin:${HOMEPATH}/bin:${HOMEPATH}/usr/bin:${HOMEPATH}/script

#####################################################################
#                                                                   #
# Location of pnmtopng                                              #
#                                                                   #
#####################################################################

setenv PNMTOPNG "/usr/bin/pnmtopng"

#####################################################################
#                                                                   #
# Base Data Tables                                                  #
#                                                                   #
#####################################################################

setenv FONTPATH "$HOMEPATH/tables/base/fonts"
setenv FONTDB "$HOMEPATH/tables/base/fonts/fontdb.xml"

#####################################################################
#                                                                   #
# Location of makefiles                                             #
#                                                                   #
#####################################################################

setenv MAKECFG ${BUILD}/base/make/makecfg
setenv MAKEBIN ${BUILD}/base/make/makebin
setenv MAKELIB ${BUILD}/base/make/makelib
setenv MAKELIBIDL ${BUILD}/base/make/makelibidl
