# Use modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OpenGL_GL_PREFERENCE GLVND)

file(GLOB_RECURSE SOURCES
    src/*
)

# Get transient linker dependencies for thirdparty libs
set(THIRDPARTY_LINK_DEPS "")
get_target_property(THIRDPARTY_LINK_DEPS thirdparty INTERFACE_LINK_LIBRARIES)

set(PHYSX_LINK_DEPS "")
get_target_property(PHYSX_LINK_DEPS PhysX::PhysX INTERFACE_LINK_LIBRARIES)
message(STATUS "====> PHYSX LINK DEPS: ${PHYSX_LINK_DEPS}")

# Create target
add_executable(game ${SOURCES})

target_include_directories(game PRIVATE src)
target_link_libraries(game
	PUBLIC
		thirdparty
		${THIRDPARTY_LINK_DEPS}
		PhysX::PhysX
		${PHYSX_LINK_DEPS}
)
target_compile_definitions(game PRIVATE ${DEFINITIONS})
target_compile_options(game PRIVATE ${_453_CMAKE_CXX_FLAGS})

set(FINAL_LINK_LIBS "")
get_target_property(FINAL_LINK_LIBS game LINK_LIBRARIES)
message(STATUS "====> FINAL LINK LIBS: ${FINAL_LINK_LIBS}")

set_target_properties(game PROPERTIES INSTALL_RPATH "./" BUILD_RPATH "./")

# Copy all text resources
add_custom_command(
	TARGET game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_CURRENT_SOURCE_DIR}/resources
			$<TARGET_FILE_DIR:game>/resources
)

# Copy all DLL dependencies
add_custom_command(
	TARGET game POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_RUNTIME_DLLS:game> $<TARGET_FILE_DIR:game>
	COMMAND_EXPAND_LISTS
)
