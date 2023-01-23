@echo off

: Determine config type to use for PhysX
set CONFIG=%~1
if "%CONFIG%"=="" set CONFIG=debug
echo Setting up PhysX for config: %CONFIG%

: Set up and build PhysX
cd thirdparty\physx\physx
call generate_projects.bat vc16win64

: NV_USE_STATIC_WINCRT needs to be set to 'False' due to the Extensions lib always being built statically
cmake -B compiler/vc16win64 -S compiler/public -DNV_USE_STATIC_WINCRT=False

cmake --build compiler/vc16win64 --config=%CONFIG%
cmake --install compiler/vc16win64 --config=%CONFIG% --prefix="%~dp0/thirdparty/physx-build"
