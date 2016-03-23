#!/bin/bash

if [ -z "$1" ];then
    echo "./%0 <name>"
    exit 1
fi

cp -f AliRsnOutTaskExample.h AliRsnOutTask${1}.h
cp -f AliRsnOutTaskExample.cxx AliRsnOutTask${1}.cxx

sed -i -e 's/AliRsnOutTaskExample/AliRsnOutTask'$1'/g' AliRsnOutTask${1}.h
sed -i -e 's/AliRsnOutTaskExample/AliRsnOutTask'$1'/g' AliRsnOutTask${1}.cxx

echo "Files 'AliRsnOutTask${1}.h' and 'AliRsnOutTask${1}.cxx' were created."
