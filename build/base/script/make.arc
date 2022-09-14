#!/bin/bash

# make.arc
# ========
# R.J.Barnes
#
# Purpose:
#
# Builds a distribution based on project name and package.
#
# Syntax:
#
# make.arc project package system
#
# Description:
#
# Builds a distribution based on project name and package.


##############################################################################

#  Test for valid command line

##############################################################################

 beta=0
 prj=${1}
 pkg=${2}
 sys=${3}


if test -z "${sys}" 
then
  echo "make.dst [-b] project package system" 
  exit
fi

date=`date`

export=${HOMEPATH}/export
build=${BUILD}

##############################################################################

#  Checkout code from the repository and build a code module

##############################################################################


makemodule() {

  project=${1}
  module=${2}
  type=${3}
 
  echo ${module} ${type}
  gitlist=`find ${HOMEPATH}/${module} -name ".git"`
  for gitname in ${gitlist}
    do
      gitpath=${gitname%%/.git}
      path=${gitpath#${HOMEPATH}/}

      cd ${gitpath}
      
      mkdir -p ${export}/${path}
      cp .git ${export}/${path}/.git
      git archive -v HEAD | (cd ${export}/${path} && tar xf -)
      
      if test "${type}" = "lib"  
      then 
        git tag > ${export}/${path}/src/version.txt 
      elif test "${type}" = "bin"
      then
        git tag > ${export}/${path}/version.txt
      elif test "${type}" = "package"
      then
        git tag > ${export}/${path}/version.txt
      fi

    done

}

##############################################################################

#  Start generating the distribution from the module list

##############################################################################

modlst=`cat ${build}/${prj}/package/${pkg}/list.txt | tr " " "+" `

rm -fr ${export}
mkdir -p ${export}
mkdir -p ${export}/codebase
mkdir -p ${export}/bin
mkdir -p ${export}/lib
mkdir -p ${export}/idl
mkdir -p ${export}/include
mkdir -p ${export}/doc
mkdir -p ${export}/script
mkdir -p ${export}/log
mkdir -p ${export}/usr/codebase
mkdir -p ${export}/usr/bin
mkdir -p ${export}/usr/lib
mkdir -p ${export}/usr/include



for mod in ${modlst}
 do

   if test -z "${mod}"  
   then 
     continue
   fi

   if test "${mod:0:1}" = "#" 
   then 
     continue
   fi

   tmp=${mod//+/ }
   prm=${tmp//\$\{sys\}/${sys}}
   makemodule ${prm}
   s=${?}
   if test ${s} -ne 0
   then
     break
   fi
 done

##############################################################################

#  Set distribution version number

##############################################################################

vtxt="Master Archive"
ver="arc"

##############################################################################

#  Generate Housekeeping

##############################################################################

 cd ${export}

 find . >> log/${prj}-${pkg}.${sys}.${ver}.content


##############################################################################

#  Package up the archive

##############################################################################

tarlist="rpkg.archive.tar"

 tar -cvf rpkg.archive.tar *
 flist=`find "${build}/${prj}/package/${pkg}" -name "rpkg.*.sh" \
        -o -name "rpkg.*.txt"`
 
 for fname in ${flist}
  do
    cat ${fname} | addtext -t "\$version\$" -r "${vtxt}" | \
                   addtext -t "\$sys\$" -r "${sys}" > ${fname##*/}
   
   if test "${fname##*.}" = "sh"
    then
      chmod a+x ${fname##*/}
    fi
    tarlist="${tarlist} ${fname##*/}"
 done

 if test -f "${build}/${prj}/package/${pkg}/README.txt"
 then 
   cat "${build}/${prj}/package/${pkg}/README.txt" | \
      addtext -t "\$version\$" -r "${vtxt}" | \
      addtext -t "\$sys\$" -r "${sys}" > \
      README.${prj}-${pkg}.${ver}.txt
      tarlist="${tarlist} README.${prj}-${pkg}.${ver}.txt"

 fi

 rm -f $HOME/${prj}-${pkg}.tar*
 tar -cvf $HOME/${prj}-${pkg}.tar ${tarlist}

 cd ${HOME}
 rm -fr ${export}
 gzip ${HOME}/${prj}-${pkg}.tar


############################################################################

# Make Self Extracting Archive

############################################################################

let lnecnt="`cat ${BUILD}/base/script/rpkg.se | wc -l`"
let lnecnt=$lnecnt+1

cat ${BUILD}/base/script/rpkg.se | addtext -t "\$lnecnt\$" \
   -r "${lnecnt}" \
    > ${prj}-${pkg}.${sys}.${ver}.sh

cat $HOME/${prj}-${pkg}.tar.gz >> ${HOME}/${prj}-${pkg}.${sys}.${ver}.sh
chmod a+x ${HOME}/${prj}-${pkg}.${sys}.${ver}.sh
rm $HOME/${prj}-${pkg}.tar.gz
