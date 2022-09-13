#!/usr/bin/env bash

set -e

CFLAGS="$CFLAGS -I$(realpath "${PWD}/../gray386/include")"
./configure --target=i386 --prefix=$(realpath "${PWD}/../gray386/")
make -j"$GR_CPUS"
make install
