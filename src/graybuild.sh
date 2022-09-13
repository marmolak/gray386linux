#!/usr/bin/env bash

set -e

# Install kernel headers
pushd ./build-kernel-env-with-nix/
	nix-shell --command "make headers_install ARCH=i386 INSTALL_HDR_PATH=../gray386/"
popd

# Build musl libc 
pushd ./build-env-with-nix/
	nix-shell --command "pushd ./musl-1.2.2/ && ./graybuild.sh && popd"
popd

# Build busybox
pushd ./build-env-with-nix/
	nix-shell --command "pushd ./busybox-1.34.1/ && ./graybuild.sh && popd"
popd

# Build dropbear SSH client
pushd ./build-env-with-nix/
	nix-shell --command "pushd ./dropbear-2020.81/ && ./graybuild.sh && popd"
popd

# Build Linus kernel
pushd ./build-kernel-env-with-nix/
	nix-shell --command "make -j"$GR_CPUS" ARCH=i386 bzImage"
popd

rm -rf ./results
mkdir ./results
pushd linux-3.7.10/
	cp ./usr/initramfs_data.cpio.gz ../results
	cp ./arch/i386/boot/bzImage ../results/
popd
