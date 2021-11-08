with import <nixpkgs> {};

let oldPkgs =
    import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/aeaa79dc82980869a88a5955ea3cd3e1944b7d80.tar.gz") {};

    oldPerlPkg = oldPkgs.perl520;

in gcc49Stdenv.mkDerivation
{
    name = "old-kernel-dev-environment";
    buildInputs = [
        ncurses
        ncurses.dev
        pkg-config
        syslinux
        oldPerlPkg
        flex
        bison
        rsync
	less
    ];

    shellHook = ''
        cd ../linux-3.7.10/
    '';
}
