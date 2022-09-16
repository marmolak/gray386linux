#!/usr/bin/env bash

set -e

export GR_CPUS=$(nproc --all)

make -j"$GR_CPUS" ARCH=i386 bzImage
