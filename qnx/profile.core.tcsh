# Environment variables for the Core Libraries - tcsh
# NB. The environment variable HOMEPATH must be set to the directory
# the software is installed in.

setenv CODEBASE ${HOMEPATH}/code
setenv LOGPATH ${HOMEPATH}/log

setenv LIBPATH ${HOMEPATH}/lib
setenv BINPATH ${HOMEPATH}/bin
setenv IPATH ${HOMEPATH}/include

setenv USR_CODEBASE ${HOMEPATH}/usr/code

setenv USR_LIBPATH ${HOMEPATH}/usr/lib
setenv USR_BINPATH ${HOMEPATH}/usr/bin
setenv USR_IPATH ${HOMEPATH}/usr/include
setenv PATH ${PATH}:${HOMEPATH}/bin:${HOMEPATH}/script:${HOMEPATH}/usr/bin











setenv MAKEBIN ${CODEBASE}/make/makebin.qnx4
setenv MAKELIB ${CODEBASE}/make/makelib.qnx4
setenv MAKELIBIDL ${CODEBASE}/make/makelibidl.qnx4
