#!/bin/bash

# TODO: check afterwards

FILE_PATH=$(realpath $(dirname $BASH_SOURCE))

if [[ ! -f main ]]
then
    cd $FILE_PATH
    make all &> /dev/null
fi

$FILE_PATH/main $@
