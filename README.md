```
  _______ .______          ___   ____    ____  ____     ___      __   
 /  _____||   _  \        /   \  \   \  /   / |___ \   / _ \    / /   
|  |  __  |  |_)  |      /  ^  \  \   \/   /    __) | | (_) |  / /_   
|  | |_ | |      /      /  /_\  \  \_    _/    |__ <   > _ <  | '_ \  
|  |__| | |  |\  \----./  _____  \   |  |      ___) | | (_) | | (_) | 
 \______| | _| `._____/__/     \__\  |__|     |____/   \___/   \___/  
Linux for i386 machines
```

How to get binary build?
========================
It's easy. Just take a look at bin directory. There is 2 folders named:

(not available - waiting on FPU) `fpu` - version for machines with FPU coprocessor installed.

`no_fpu` - build with software FPU enabled.

NOTE: only AMD cpus are enabled by default now.


How to build gray386 linux
==========================

Tested build environment:
Fedora 35 with Nix installed.

**10.** [Install Nix](https://nixos.org/manual/nix/stable/#sect-multi-user-installation)
-------------------

**11.** Build user env with `nix-shell` (in progress now)
---------------------------------------

`cd src/build-env-with-nix/`

`nix-shell --pure`

**20.** Get num of cpus
-----------------------

`export GR_CPUS=$(nproc --all)`

**21.** Set `gray386/bin`
-------------------------

`mkdir -p gray386/bin`

`which ar`

`ln -s /path/to/ar ./musl-ar`

`which strip`

`ln -s /pat/to/strip musl-strip`


**30.** Build musl libc
-----------------------

`./configure --target=i386 --prefix=../gray386/`

`make -j"$GR_CPUS"`

`make install`

**40.** Build busybox
---------------------

(optional) `make menuconfig`

`make -j"$GR_CPUS"`

`make install`

**41.** Leave nix-shell
-----------------------

`exit`

**50.** Build kernel with `nix-shell`
-------------------------------------

`cd src/build-kernel-env-with-nix/`

`nix-shell --pure`

**60.** In Linux kernel directory
---------------------------------

(optional) `make -j"$GR_CPUS" ARCH=i386 nconfig`

`make -j"$GR_CPUS" ARCH=i386 bzImage`

Results are in:

`arch/x86/boot/bzImage`
`usr/initramfs_data.cpio.gz`

**61.** Exit nix-shell
----------------------

`exit`

**62.** (optional) Test build
-----------------------------

You need to have qemu installed.

In kernel directory just run

`./test-build.sh`


**70.** Cleanup build
---------------------

`git clean -f -d -X`
