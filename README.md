```
  _______ .______          ___   ____    ____  ____     ___      __   
 /  _____||   _  \        /   \  \   \  /   / |___ \   / _ \    / /   
|  |  __  |  |_)  |      /  ^  \  \   \/   /    __) | | (_) |  / /_   
|  | |_ | |      /      /  /_\  \  \_    _/    |__ <   > _ <  | '_ \  
|  |__| | |  |\  \----./  _____  \   |  |      ___) | | (_) | | (_) | 
 \______| | _| `._____/__/     \__\  |__|     |____/   \___/   \___/  
Linux for i386 machines
```

How to build gray386 linux
==========================

Tested build environment:
Fedora 34 with Nix installed.

**10.** Install Nix 
-------------------

https://nixos.org/manual/nix/stable/#sect-multi-user-installation

**11.** start `nix-shell`
-------------------------

`nix-shell`

**20.** Get num of cpus
-----------------------

`export GR_CPUS=$(nproc --all)`

**30.** Go to linux kernel directory
------------------------------------

In kernel directory:

(optional)

`make -j"$GR_CPUS" ARCH=i386 CFLAGS=-m32 CFLAGS_KERNEL=-m32 nconfig`

`make -j"$GR_CPUS" ARCH=i386 CFLAGS=-m32 CFLAGS_KERNEL="-march=i386 -mtune=i386 -m32" bzImage`

**70.** Cleanup build
---------------------

`git clean -f -d -X`
