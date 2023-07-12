How to build gray386 linux with podman
======================================

You need to have podman installed on system.

Change working directory to `docker`.

Then just run `graybuild.sh`. This going to compile whole gray386 linux and
create image. You can find results of compilation (kernel with userspace) into `src/results`
directory, because `src` directory is propagated to container into `/build` directory.

You can access build environment by using `grayrun.sh`.
