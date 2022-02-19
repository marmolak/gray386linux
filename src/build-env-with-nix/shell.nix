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
	which
	less
    ];

    shellHook = ''
	export CFLAGS="-m32 -march=i386 -mtune=i386 -fcf-protection=none -fno-stack-protector -fomit-frame-pointer -mno-mmx -mno-sse -fno-pic -Os"
	export CC="gcc"
	export GR_CPUS=$(nproc --all)
	cd ..

	# set strip and ar
	mkdir -p ./gray386/bin/
	pushd ./gray386/bin/ &> /dev/null
	rm -rf ./musl-strip
	rm -rf ./musl-ar

	ln -s "$(which strip)" musl-strip
	ln -s "$(which ar)" musl-ar

	popd &> /dev/null
    '';
}
