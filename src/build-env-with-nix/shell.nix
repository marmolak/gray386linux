with import <nixpkgs> {};
with pkgsi686Linux;

let
    gccNoCet = gcc11.cc.overrideAttrs (oldAttrs : rec {
        configureFlags = [ "--disable-cet" ] ++ oldAttrs.configureFlags;
    });

    gccNoCetWrap = wrapCCWith rec {
        cc = gccNoCet;
    };

in (overrideCC stdenv gccNoCetWrap).mkDerivation
{
    name = "gccnocet";
    hardeningDisable = [ "all" ];

    buildInputs = [
	ncurses
	ncurses.dev
	pkg-config
	less
    ];

    shellHook = ''
	export CFLAGS="-m32 -march=i386 -mtune=i386 -fcf-protection=none -fno-stack-protector -fomit-frame-pointer -mno-mmx -mno-sse -fno-pic -Os"
	export CC="gcc"
	cd ..
    '';
}
