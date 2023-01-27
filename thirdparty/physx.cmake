set(PHYSX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/physx-build)
set(PHYSX_BIN ${PHYSX_ROOT}/bin/win.x86_64.vc142.md)

# ---------------------------
# Individual PhysX components
# ---------------------------

add_library(PhysX::PhysX64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysX64
    PROPERTIES
        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysX_64.dll
        IMPORTED_IMPLIB   ${PHYSX_BIN}/debug/PhysX_64.lib

        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysX_64.dll
        IMPORTED_IMPLIB_RELEASE   ${PHYSX_BIN}/release/PhysX_64.lib
)

add_library(PhysX::PhysXCommon64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXCommon64
    PROPERTIES
        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXCommon_64.dll
        IMPORTED_IMPLIB   ${PHYSX_BIN}/debug/PhysXCommon_64.lib

        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXCommon_64.dll
        IMPORTED_IMPLIB_RELEASE   ${PHYSX_BIN}/release/PhysXCommon_64.lib
)

add_library(PhysX::PhysXFoundation64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXFoundation64
    PROPERTIES
        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXFoundation_64.dll
        IMPORTED_IMPLIB   ${PHYSX_BIN}/debug/PhysXFoundation_64.lib

        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXFoundation_64.dll
        IMPORTED_IMPLIB_RELEASE   ${PHYSX_BIN}/release/PhysXFoundation_64.lib
)

add_library(PhysX::PhysXPvdSDKStatic64 STATIC IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXPvdSDKStatic64
    PROPERTIES
        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXPvdSDK_static_64.lib

        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXPvdSDK_static_64.lib
)

add_library(PhysX::PhysXExtensions64 STATIC IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXExtensions64
    PROPERTIES
        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXExtensions_static_64.lib
        
        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXExtensions_static_64.lib
)

# ---------------------------
# Full PhysX wrapper
# ---------------------------

add_library(PhysX::PhysX INTERFACE IMPORTED GLOBAL)
set_target_properties(PhysX::PhysX
    PROPERTIES
        INTERFACE_LINK_LIBRARIES "PhysX::PhysX64;PhysX::PhysXCommon64;PhysX::PhysXFoundation64;PhysX::PhysXExtensions64;PhysX::PhysXPvdSDKStatic64"
)
target_include_directories(PhysX::PhysX
    INTERFACE
        ${PHYSX_ROOT}/include
)
