#!/bin/bash

sys="$sys$"
version="$version$"
shlist="bash tcsh"


echo "Please consult the README for instructions on how to compile the code"
echo "once installation is complete"
echo ""
echo "Do you wish to proceed? (y/n)"

yesno=""
while test "$yesno" != "y" -a "$yesno" != "n"
do
  read yesno

  if test "$yesno" != "y" -a "$yesno" != "n"
  then
    echo "Please answer y for yes or n for no"
  fi
done

if test "$yesno" = "n"
  then
  exit 0
fi

export HOMEPATH=${PWD}

cd ${HOMEPATH}/script
ln -svf ../build/base/script/* ${HOMEPATH}/script
cd ${HOMEPATH}

link.script superdarn

for shell in ${shlist}
do
 if test -f build/base/profile/profile.${sys}.${shell}
  then
  cat build/base/profile/profile.${sys}.${shell} > \
       profile.superdarn-ros.${sys}.${shell}
  fi

  if test -f build/general/profile/profile.${sys}.${shell}
  then
  cat build/general/profile/profile.${sys}.${shell} >> \
       profile.superdarn-ros.${sys}.${shell}
  fi

  if test -f build/superdarn/profile/profile.${sys}.${shell}
  then
  cat build/superdarn/profile/profile.${sys}.${shell} >> \
       profile.superdarn-ros.${sys}.${shell}
  fi

done



