set(PROJECT_NAME "game")

file(GLOB_RECURSE SOURCES
    src/*
)

# Get transient linker dependencies for thirdparty libs
set(THIRDPARTY_LINK_DEPS "")
get_target_property(THIRDPARTY_LINK_DEPS thirdparty INTERFACE_LINK_LIBRARIES)

# Create target
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PUBLIC
	thirdparty
	${THIRDPARTY_LINK_DEPS}
)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINITIONS})
target_compile_options(${PROJECT_NAME} PRIVATE ${_453_CMAKE_CXX_FLAGS})

set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "./" BUILD_RPATH "./")

# Copy all resources
add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_CURRENT_SOURCE_DIR}/resources
			$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)
