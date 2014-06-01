#!/bin/bash

if (( $# == 0 )); then
    echo "Usage: cross-toolchain.sh <target> [prefix]"
    echo "Possible values for <target> could be"
    echo "  i686-elf"
    echo "  x86_64-elf"
    exit -1
fi

if (( $# >= 1 )); then
    export TARGET=$1
fi

if (( $# >= 2 )); then
    export PREFIX=$2
else
    export PREFIX="$HOME/opt/cross"
fi

export PATH="$PREFIX/bin:$PATH"


BINUTILS_NAME="binutils-2.24"
GCC_NAME="gcc-4.9.0"

if [ ! -f "$BINUTILS_NAME.tar.gz" ]; then
    wget http://ftp.gnu.org/gnu/binutils/$BINUTILS_NAME.tar.gz
fi
if [ ! -f "$GCC_NAME.tar.gz" ]; then
    wget http://gcc.cybermirror.org/releases/$GCC_NAME/$GCC_NAME.tar.gz
fi

if [ ! -d "$BINUTILS_NAME" ]; then
    tar xzvf "$BINUTILS_NAME.tar.gz"
fi
if [ ! -d "$GCC_NAME" ]; then
    tar xzvf "$GCC_NAME.tar.gz"
fi


if [ ! -f "$PREFIX/bin/$TARGET-as" ]; then
    if [ -d build-binutils ]; then
	rm -r build-binutils
    fi
    mkdir -p build-binutils
    pushd build-binutils
    ../$BINUTILS_NAME/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror
    make -j4
    make install
    popd
fi

if [ ! -f "$PREFIX/bin/$TARGET-gcc" ]; then
    if [ -d build-gcc ]; then
	rm -r build-gcc
    fi
    mkdir -p build-gcc
    pushd build-gcc
    ../$GCC_NAME/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
    make -j4 all-gcc
    make -j4 all-target-libgcc
    make install-gcc
    make install-target-libgcc
    popd
fi
