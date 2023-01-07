# PhysX-specific CMake project setup
set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
set(PHYSX_ROOT_DIR ${CMAKE_SOURCE_DIR}/thirdparty/physx/physx)

message(STATUS "physx root: ${PHYSX_ROOT_DIR}")

set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/physx)
set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/physx)

# Call into PhysX's CMake scripts
add_subdirectory(${PHYSX_ROOT_DIR}/compiler/public)

# set(PHYSX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/physx/physx)

# add_library(physx SHARED IMPORTED)
# set_target_properties(physx
#     PROPERTIES IMPORTED_LOCATION
#         ${PHYSX_ROOT}/bin/win.x86_64.vc142.mt/debug/PhysX_64.dll
# )

# target_link_directories(physx
#     INTERFACE
#         ${PHYSX_ROOT}/bin/win.x86_64.vc142.mt/debug
# )

# target_link_libraries(physx
#     INTERFACE
#         PhysX_64
#         PhysXCommon_64
#         PhysXFoundation_64
# )

# target_include_directories(physx
#     INTERFACE
#         ${PHYSX_ROOT}/include
# )