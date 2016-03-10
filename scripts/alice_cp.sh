#!/bin/bash

trap '{ echo "Hey, you pressed Ctrl-C.  Time to quit." ; exit 1; }' INT

OUT_EOS=/eos/alike.saske.sk
OUT_ARCHIVE=${2}
OUT_ARCHIVE_REPLACE="AliESDs.root"


function help() {
	echo "Usage : $0 <filename>"
}

[ -n "$1" ] || { help; exit 1; }
[ -f "$1" ] || { echo "Error : file $1 doesn't exist" ; exit 2; }

alien-token-info
[ $? -eq 0 ] || { echo "Error : AliEn token is not valid!!! Do 'alien-token-init' first ..." ; exit 2;}

while read -r line; do
    name="$line"
    [[ $name == /alice* ]] || continue
    base_dir=$name
    [ -n "$OUT_ARCHIVE" ] && name=${name/$OUT_ARCHIVE_REPLACE/$OUT_ARCHIVE/}
    base_dir=$(dirname $name)
    [ -f $OUT_EOS$name ] && { echo "Skipping '$OUT_EOS$name' ..."; continue; }
    mkdir -p $OUT_EOS$base_dir || { echo "Cannot create directory '$OUT_EOS$base_dir' !!!"; exit 4; }
    echo "Downloading '$OUT_EOS$name' ..."
    echo "$OUT_EOS$name" > alien_cp_last.txt
    alien_cp alien://$name $OUT_EOS$base_dir
    
    alien-token-info > /dev/null 2>&1
	[ $? -eq 0 ] || { alien-token-init;}
done < "$1"
