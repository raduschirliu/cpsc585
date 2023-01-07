set(PHYSX_CONFIG debug)

set(PHYSX_INSTALL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/physx-build)
set(PHYSX_INSTALL_BIN ${PHYSX_INSTALL_ROOT}/bin/win.x86_64.vc142.md/${PHYSX_CONFIG})

# ---------------------------
# Individual PhysX components
# ---------------------------

add_library(PhysX::PhysX64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysX64
    PROPERTIES
        IMPORTED_LOCATION ${PHYSX_INSTALL_BIN}/PhysX_64.dll
        IMPORTED_IMPLIB   ${PHYSX_INSTALL_BIN}/PhysX_64.lib
)

add_library(PhysX::PhysXCommon64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXCommon64
    PROPERTIES
        IMPORTED_LOCATION ${PHYSX_INSTALL_BIN}/PhysXCommon_64.dll
        IMPORTED_IMPLIB   ${PHYSX_INSTALL_BIN}/PhysXCommon_64.lib
)

add_library(PhysX::PhysXFoundation64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXFoundation64
    PROPERTIES
        IMPORTED_LOCATION ${PHYSX_INSTALL_BIN}/PhysXFoundation_64.dll
        IMPORTED_IMPLIB   ${PHYSX_INSTALL_BIN}/PhysXFoundation_64.lib
)

add_library(PhysX::PhysXExtensions64 STATIC IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXExtensions64
    PROPERTIES
        IMPORTED_LOCATION ${PHYSX_INSTALL_BIN}/PhysXExtensions_static_64.lib
)

# ---------------------------
# Full PhysX wrapper
# ---------------------------

add_library(PhysX::PhysX INTERFACE IMPORTED GLOBAL)
set_target_properties(PhysX::PhysX
    PROPERTIES
        INTERFACE_LINK_LIBRARIES "PhysX::PhysX64;PhysX::PhysXCommon64;PhysX::PhysXFoundation64;PhysX::PhysXExtensions64"
)
target_include_directories(PhysX::PhysX
    INTERFACE
        ${PHYSX_INSTALL_ROOT}/include
)
