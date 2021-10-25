with import <nixpkgs> {};

let oldPkgs =
    import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/aeaa79dc82980869a88a5955ea3cd3e1944b7d80.tar.gz") {};

    oldPerlPkg = oldPkgs.perl520;
in
{
    oldKernelEnv = stdenvNoCC.mkDerivation
    {
        name = "old-kernel-dev-environment";
        buildInputs = [
            gcc49
            ncurses5
            syslinux
            oldPerlPkg
        ];
    };
}
