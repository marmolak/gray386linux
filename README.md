```
  _______ .______          ___   ____    ____  ____     ___      __   
 /  _____||   _  \        /   \  \   \  /   / |___ \   / _ \    / /   
|  |  __  |  |_)  |      /  ^  \  \   \/   /    __) | | (_) |  / /_   
|  | |_ | |      /      /  /_\  \  \_    _/    |__ <   > _ <  | '_ \  
|  |__| | |  |\  \----./  _____  \   |  |      ___) | | (_) | | (_) | 
 \______| | _| `._____/__/     \__\  |__|     |____/   \___/   \___/  
Linux for i386 machines
```

Are you interested in Linux on i486 machine? Take a look at [gray486linux](https://github.com/marmolak/gray486linux). Currently, it's not possible to run gray386 on i486 machine. However i486
machine is still able to run actual Linux kernel (2021).


How to get binary build?
========================
It's easy. Just take a look at `bin` directory. There is 2 folders named:

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

**59.** Update `.config` in kernel directory
--------------------------------------------

There is mapping hidden in `.config` and you need to change it
to UID of user under you compile a kernel.

`CONFIG_INITRAMFS_ROOT_UID=1001`

`CONFIG_INITRAMFS_ROOT_GID=1001`

Then you can do some other changes with:

`make -j"$GR_CPUS" ARCH=i386 nconfig`


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
