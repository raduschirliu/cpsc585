: git submodule update --init --recursive

call thirdparty\physx\physx\buildtools\packman\packman.cmd init
call thirdparty\physx\physx\buildtools\packman\packman.cmd pull thirdparty\physx\physx\dependencies.xml --platform vc16win64

cmake -S . -B build -G "Visual Studio 16 2019" -Ax64 -DTARGET_BUILD_PLATFORM=windows -DPX_OUTPUT_ARCH=x86 --no-warn-unused-cli