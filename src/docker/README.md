How to build Gray386linux with podman
======================================

You need to have podman installed on system.

Change working directory to `docker`.

Then just run `containerbuild.sh`. This going to compile whole Gray386Linux and
create image. You can find results of compilation (kernel with userspace) into `src/results`
directory, because `src` directory is propagated to container into `/build` directory.

You can access build environment by using `containerrun.sh`.
