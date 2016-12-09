#!/bin/bash

function build {
        ./waf clean
	echo "./waf configure --mode=release --build-static --lightweight= --fft=KISS --prefix=out/$1 $2"
	./waf configure --mode=release --build-static --lightweight= --fft=KISS --prefix=out/$1 $2
	echo "build " $1
	./waf build -j `nproc`
	./waf install
}

function android_build_py {
	echo "create toolchain for" $2
	python $1/build/tools/make_standalone_toolchain.py $3 $4 --install-dir /tmp/$2-android-toolchain --force
	export PATH=/tmp/$2-android-toolchain/bin:$PATH
	build "android/$2" "--cross-compile-android --abi=$2"
}

function android_build_sh {
	echo "create toolchain for" $2
	$1/build/tools/make-standalone-toolchain.sh $3 --install-dir=/tmp/$2-android-toolchain --force
	export PATH=/tmp/$2-android-toolchain/bin:$PATH
	build "android/$2" "--cross-compile-android"
}

if [ "$1" == "" ]; then
	echo "missing argument [win, android, linux]"
else 
	./waf clean
	echo 'configure for '$1' build'
	if [ "$1" == "android" ]; then
		if [ "$2" == "" ]; then
			echo "missing ndk path"
		elif [ "$3" == "python" ]; then
			android_build_py $2 "armeabi-v7a"    "--arch arm"    "--api 18" 
			android_build_py $2 "arm64-v8a"      "--arch arm64"  "--api 21"
			android_build_py $2 "x86"            "--arch x86"    "--api 18"
			android_build_py $2 "x86_64"         "--arch x86_64" "--api 21"
		else
			android_build_sh $2 "armeabi-v7a"    "--arch=arm"
			android_build_sh $2 "arm64-v8a"      "--arch=arm64"
			android_build_sh $2 "x86"            "--arch=x86"
			android_build_sh $2 "x86_64"         "--arch=x86_64"
		fi
	else
		if [ "$1" == "win" ]; then
			build "x64" "--cross-compile-mingw32"
		elif [ "$1" == "linux" ]; then
			build "linux"
		else
			echo "missing argument [win, android, linux]"
		fi
	fi
fi
