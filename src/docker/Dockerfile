FROM nixos/nix
MAINTAINER Robin Hack <hack.robin@gmail.com>

RUN nix-channel --update

WORKDIR /build

RUN ./graybuild.sh
ENTRYPOINT ["/bin/sh", "-i"]
