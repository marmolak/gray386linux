#!/usr/bin/env bash

. ./lib/userspace.sh

for U in ${USERSPACE[*]}; do
    pushd ./build-env-with-nix/
        nix-shell --pure --command "pushd ./${U}/ && make clean; popd"
    popd
done

# Build Linux kernel
pushd ./build-kernel-env-with-nix/
        nix-shell --pure --command "make clean"
popd
