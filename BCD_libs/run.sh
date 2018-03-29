#!/bin/bash

cd "$(dirname "$0")"

showlogs () {
    bash_retval=${PIPESTATUS[0]}
    if [ "$bash_retval" = '' -o "$bash_retval" -ne 0 ]; then
        for file in "$@"; do
            echo ".${file}.log:"
            cat -n ".${file}.log"
            echo
        done | less
        rm -f .clean.log
        exit $bash_retval
    fi
}

if [ "$1" = 'init' -o "$1" = 'initins' ]; then
    ./clean.sh 2>&1 | tee .clean.log
else
    echo 'NO CLEAN PERFORMED' | tee .clean.log
fi

cmake . 2>&1 | tee .cmake.log
showlogs clean cmake
make 2>&1 | tee .make.log
showlogs clean cmake make
if [ "$1" = 'install' -o "$1" = 'initins' ]; then
    echo 'Installing and configuring linker. Requires root.'
    sudo make install 2>&1 | tee .install.log
    showlogs clean cmake make install
    sudo ldconfig 2>&1 | tee .ldconfig.log
    showlogs clean cmake make install ldconfig
else
    echo 'NO INSTALL PERFORMED' | tee .install.log
fi
rm -f .clean.log
