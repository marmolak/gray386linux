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

NOTE2: gray386 linux is source based distribution so binaries can be older than
current configuration.


How to build a gray386 linux
============================

Tested build environment:
Fedora 35 with Nix installed.

**10.** [Install Nix](https://nixos.org/manual/nix/stable/installation/installing-binary.html#multi-user-installation)
----------------------------------------------------------------------------------------


**20.** Get num of cpus
-----------------------

Nix-shell scripts take handle of it.

If you don't want to use `nix-shell`, just type:

`export GR_CPUS=$(nproc --all)`


**28.** Get kernel headers with `nix-shell`
-------------------------------------------

`cd src/build-kernel-env-with-nix/`

`nix-shell --pure`


**28.** Install kernel headers
------------------------------

`make headers_install ARCH=i386 INSTALL_HDR_PATH=../gray386/`


**29.** Leave nix-shell
-----------------------

`exit`


**30.** Build user env with `nix-shell`
---------------------------------------

`cd src/build-env-with-nix/`

`nix-shell --pure`


**40.** Build musl libc
-----------------------

`CFLAGS="$CFLAGS -I$(realpath "${PWD}/../gray386/include")" ./configure --target=i386 --prefix=$(realpath "${PWD}/../gray386/")`

`make -j"$GR_CPUS"`

`make install`


**50.** Build busybox
---------------------

(optional) `make menuconfig`

`make -j"$GR_CPUS"`

`make install`

**51.** (optional) Build dropbear SSH client (+~266 K)
------------------------------------------------------

`CC="$(realpath $PWD/../gray386/bin/musl-gcc)"  ./configure --enable-static --enable-bundled-libtom --disable-syslog  --disable-harden --disable-zlib --disable-shadow --disable-utmp --disable-utmpx --disable-wtmpx --disable-loginfunc --prefix="$(realpath $PWD/../gray386/_install/)"`

`make -j"$GR_CPUS"`

`strip dbclient`

`cp dbclient ../gray386/_install/bin`


**59.** Leave nix-shell
-----------------------

`exit`


**60.** Build kernel with `nix-shell`
-------------------------------------

`cd src/build-kernel-env-with-nix/`

`nix-shell --pure`


**61.** Update `.config` in kernel directory
--------------------------------------------

There is mapping hidden in `.config` and you need to change it
to UID of user under you compile a kernel.

`CONFIG_INITRAMFS_ROOT_UID=1001`

`CONFIG_INITRAMFS_ROOT_GID=1001`

Then you can do some other changes with:

`make -j"$GR_CPUS" ARCH=i386 nconfig`


**70.** In Linux kernel directory
---------------------------------

(optional) `make -j"$GR_CPUS" ARCH=i386 nconfig`

`make -j"$GR_CPUS" ARCH=i386 bzImage`

Results are in:

`arch/x86/boot/bzImage`
`usr/initramfs_data.cpio.gz`


**80.** Exit nix-shell
----------------------

`exit`

**81.** (optional) Test build
-----------------------------

You need to have qemu installed.

In kernel directory just run

`./test-build.sh`


**90.** Cleanup build
---------------------

`git clean -f -d -X`
