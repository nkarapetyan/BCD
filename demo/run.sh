#!/bin/bash

cd "$(dirname "$0")"

QMAKE=qmake
if ! which $QMAKE ; then
    QMAKE=qmake
fi
echo "QMAKE=$QMAKE"

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

$QMAKE -project 2>&1 | tee .qmake1.log
showlogs clean qmake1
$QMAKE *.pro 2>&1 | tee .qmake2.log
showlogs clean qmake1 qmake2
cat libs.txt >> "$(find -name "*.pro")"
echo "CONFIG+=debug" >> "$(find -name "*.pro")"
$QMAKE -o Makefile *.pro 2>&1 | tee .qmake3.log
showlogs clean qmake1 qmake2 qmake3

#export LD_LIBRARY_PATH=/usr/local/lib/KCPP

sed -i 's|^INCPATH.*$|\0 -Iinclude|' Makefile
#make 2>&1 >/dev/null | less
make 2>&1 | tee .make.log
showlogs clean qmake1 qmake2 qmake3 make

if [ "$1" = 'install' -o "$1" = 'initins' ]; then
    echo 'Installing and configuring linker. Requires root.'
    sudo cp demo /usr/local/bin/bcd-demo 2>&1 | tee .install.log
    showlogs clean qmake1 qmake2 qmake3 make install
    sudo ldconfig 2>&1 | tee .ldconfig.log
    showlogs clean qmake1 qmake2 qmake3 make install ldconfig
else
    echo 'NO INSTALL PERFORMED' | tee .install.log
fi
rm -f .clean.log
