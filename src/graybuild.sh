#!/usr/bin/env bash

set -e

declare -a -r USERSPACE=("musl-1.2.2" "busybox-1.34.1" "dropbear-2020.81")

# Install kernel headers
pushd ./build-kernel-env-with-nix/
	nix-shell --pure --command "make headers_install ARCH=i386 INSTALL_HDR_PATH=../gray386/"
popd

for U in ${USERSPACE[*]}; do
    pushd ./build-env-with-nix/
        nix-shell --pure --command "pushd ./${U}/ && ./graybuild.sh; popd"
    popd
done

# Build Linux kernel
pushd ./build-kernel-env-with-nix/
	nix-shell --pure --command "./graybuild.sh"
popd

rm -rf ./results
mkdir ./results
pushd linux-3.7.10/
	cp ./usr/initramfs_data.cpio.gz ../results
	cp ./arch/i386/boot/bzImage ../results/
popd
