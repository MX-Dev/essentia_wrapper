Prerequisites:
    Linux
    Android SDK
    MinGW 64 compiler

Build
    download essentia src https://github.com/MTG/essentia/releases
    copy build.sh and wscript from essentia_wrapper/extern/essentia to essentia src dir
    call build.sh with argument "win", "android" or "linux" to build for the specified platform
    copy generated libs to extern/essentia/lib/
