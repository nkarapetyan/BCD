#!/bin/bash
enter_to_continue () {
    read -p 'Press ENTER to continue (or ^C to exit).' _
}
base="$(readlink -e "$(dirname "$0")")"
clean_lib () {
    echo "Cleaning ${1}"
    cd "${base}/${1}"
    ./clean.sh
}
compile_lib () {
    echo "Compiling ${1}"
    cd "${base}/${1}"
    ./run.sh "$2"
    s="$?"
    if [ "$s" -ne 0 ]; then
        echo "$0: ERROR"
        exit $s
    fi
}
remove_lib () {
    new_libs=( )
    for lib in "${libs[@]}"; do
        if [ "$lib" != "$1" ]; then
            new_libs+=( "$lib" )
        fi
    done
    libs=( "${new_libs[@]}" )
}
print_usage () {
    echo "usage: $me <clean | init | build | install | initins>"
    echo '          <none | defaults | all> [<+|-><lib> ...]'
    echo
    echo '  Commands:'
    echo '      clean:  clean each provided library'
    echo '      init:   clean and then build each provided library'
    echo '      build:  build each provided library'
    echo '      install: build and install each provided library'
    echo '      initins: clean and then build and install each provided library;'
    echo '              This is the behavior of the old `run_all.sh` script.'
    echo
    echo '  Groups:'
    echo '      none:   just operate on manually added libraries'
    echo '      defaults: operate on all'
    echo '      all:    Leo_libs, BCD_libs, demo'
    exit 1
}
me="$0"
cmd="$1"
grp="$2"
shift 2
if [ "$grp" = 'none' ]; then
    libs=( )
elif [ "$grp" = 'defaults' -o "$grp" = 'all' ]; then
    libs=( Leo_libs BCD_libs demo) 
else
    echo "Invalid group '$grp'"
    print_usage
fi
for arg in "$@"; do
    if [ "${arg:0:1}" = '+' ]; then
        libs+=( "${arg:1}" )
    elif [ "${arg:0:1}" = '-' ]; then
        remove_lib "${arg:1}"
    else
        echo "Invalid argument '$arg'"
        print_usage
    fi
done
if [ "$cmd" = 'clean' ]; then
    echo BUILDER: clean: "${libs[@]}"
    for lib in "${libs[@]}"; do
        clean_lib "$lib"
        if [ "$lib" = 'catkin_ws' ]; then
            clean_lib "$lib"
        fi
    done
    cd "$base"
elif [ "$cmd" = 'init' -o "$cmd" = 'build' -o "$cmd" = 'install' -o "$cmd" = 'initins' ]; then
    echo BUILDER: "$cmd:" "${libs[@]}"
    for lib in "${libs[@]}"; do
        compile_lib "$lib" "$cmd"
    done
else
    echo "Invalid command '$cmd'"
    print_usage
fi
