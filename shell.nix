with import <nixpkgs> {}; {
  qpidEnv = stdenvNoCC.mkDerivation {
    name = "old-kernel-dev-environment";
    buildInputs = [
        gcc49
	ncurses5
	syslinux
    ];
   shellHook =
   ''
   nix-env -i perl-5.20.3 -f https://github.com/NixOS/nixpkgs/archive/aeaa79dc82980869a88a5955ea3cd3e1944b7d80.tar.gz
   '';
  };
 
}
