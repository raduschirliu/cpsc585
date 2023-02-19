set(PHYSX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/physx-build)
set(PHYSX_BIN ${PHYSX_ROOT}/bin/win.x86_64.vc142.md)

# ---------------------------
# Individual PhysX components
# ---------------------------

add_library(PhysX::PhysX64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysX64
    PROPERTIES
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release

        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysX_64.dll
        IMPORTED_IMPLIB   ${PHYSX_BIN}/debug/PhysX_64.lib

        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysX_64.dll
        IMPORTED_IMPLIB_RELEASE   ${PHYSX_BIN}/release/PhysX_64.lib
)

add_library(PhysX::PhysXCommon64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXCommon64
    PROPERTIES
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release

        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXCommon_64.dll
        IMPORTED_IMPLIB   ${PHYSX_BIN}/debug/PhysXCommon_64.lib

        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXCommon_64.dll
        IMPORTED_IMPLIB_RELEASE   ${PHYSX_BIN}/release/PhysXCommon_64.lib
)

add_library(PhysX::PhysXFoundation64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXFoundation64
    PROPERTIES
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release

        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXFoundation_64.dll
        IMPORTED_IMPLIB   ${PHYSX_BIN}/debug/PhysXFoundation_64.lib

        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXFoundation_64.dll
        IMPORTED_IMPLIB_RELEASE   ${PHYSX_BIN}/release/PhysXFoundation_64.lib
)

add_library(PhysX::PhysXCooking64 SHARED IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXCooking64
    PROPERTIES
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release

        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXCooking_64.dll
        IMPORTED_IMPLIB   ${PHYSX_BIN}/debug/PhysXCooking_64.lib

        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXCooking_64.dll
        IMPORTED_IMPLIB_RELEASE   ${PHYSX_BIN}/release/PhysXCooking_64.lib
)

add_library(PhysX::PhysXPvdSDKStatic64 STATIC IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXPvdSDKStatic64
    PROPERTIES
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release

        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXPvdSDK_static_64.lib

        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXPvdSDK_static_64.lib
)

add_library(PhysX::PhysXExtensions64 STATIC IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXExtensions64
    PROPERTIES
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release

        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXExtensions_static_64.lib
        
        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXExtensions_static_64.lib
)

add_library(PhysX::PhysXVehicle264 STATIC IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXVehicle264
    PROPERTIES
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release

        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXVehicle2_static_64.lib
        
        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXVehicle2_static_64.lib
)

add_library(PhysX::PhysXVehicle64 STATIC IMPORTED GLOBAL)
set_target_properties(PhysX::PhysXVehicle64
    PROPERTIES
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release

        IMPORTED_LOCATION ${PHYSX_BIN}/debug/PhysXVehicle_static_64.lib
        
        IMPORTED_LOCATION_RELEASE ${PHYSX_BIN}/release/PhysXVehicle_static_64.lib
)



# ---------------------------
# Full PhysX wrapper
# ---------------------------

set(PHYSX_COMMON ${CMAKE_CURRENT_SOURCE_DIR}/physx_vehicle_common)

add_library(PhysX::PhysX INTERFACE IMPORTED GLOBAL)
set_target_properties(PhysX::PhysX
    PROPERTIES
        INTERFACE_LINK_LIBRARIES
            "PhysX::PhysX64;PhysX::PhysXCommon64;PhysX::PhysXFoundation64;PhysX::PhysXExtensions64;PhysX::PhysXPvdSDKStatic64;PhysX::PhysXVehicle264;PhysX::PhysXVehicle64;PhysX::PhysXCooking64"
)
target_sources(PhysX::PhysX
    INTERFACE
        ${PHYSX_COMMON}/physx/CommonVehicleFiles/base/Base.cpp
        ${PHYSX_COMMON}/physx/CommonVehicleFiles/directdrivetrain/DirectDrivetrain.cpp
        ${PHYSX_COMMON}/physx/CommonVehicleFiles/physxintegration/PhysXIntegration.cpp
        ${PHYSX_COMMON}/physx/CommonVehicleFiles/serialization/BaseSerialization.cpp
        ${PHYSX_COMMON}/physx/CommonVehicleFiles/serialization/DirectDrivetrainSerialization.cpp
        ${PHYSX_COMMON}/physx/CommonVehicleFiles/serialization/SerializationCommon.cpp
        ${PHYSX_COMMON}/physx/CommonVehicleFiles/SnippetVehicleHelpers.cpp
)
target_include_directories(PhysX::PhysX
    INTERFACE
        ${PHYSX_ROOT}/include
        ${PHYSX_COMMON}
)
