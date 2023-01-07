: Doesn't seem to work... :[

rmdir /s /q build.physx

call thirdparty\physx\physx\buildtools\packman\packman.cmd init
call thirdparty\physx\physx\buildtools\packman\packman.cmd pull thirdparty\physx\physx\dependencies.xml --platform vc16win64

cd thirdparty\physx\physx

cmake                                           ^
    -S compiler\public                          ^
    -B ..\..\..\build.physx                     ^
    -G "Visual Studio 16 2019"                  ^
    -Ax64                                       ^
    -DTARGET_BUILD_PLATFORM=windows             ^
    -DPX_OUTPUT_ARCH=x86                        ^
    -DCMAKE_BUILD_TYPE=checked                  ^
    -DPHYSX_ROOT_DIR=%CD%                       ^
    -DPX_OUTPUT_LIB_DIR=bin                     ^
    -DPX_OUTPUT_BIN_DIR=bin                     ^
    --no-warn-unused-cli