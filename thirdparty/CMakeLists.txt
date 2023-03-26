# GLEW - http://glew.sourceforge.net/
set(INCLUDES ${INCLUDES} glew-2.1.0/include)
set(SOURCES ${SOURCES} glew-2.1.0/src/glew.c)
# GLEW on windows gets angry if not built with this definition for a static lib...
set(DEFINITIONS ${DEFINITIONS} GLEW_STATIC)

# GLFW - https://www.glfw.org/
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw-3.3.2)
set(LIBRARIES ${LIBRARIES} glfw ${GLFW_LIBRARIES})
set(INCLUDES ${INCLUDES} glfw-3.3.2/include)
# Prevent GLFW from trying to load its own OpenGL headers
set(DEFINITIONS ${DEFINITIONS} GLFW_INCLUDE_NONE)

# vivid - https://github.com/gurki/vivid/releases/tag/v2.2.1
set(INCLUDES ${INCLUDES} vivid-2.2.1/include vivid-2.2.1/dependencies/glm)

# glm - https://glm.g-truc.net/0.9.9/index.html
set(DEFINITIONS ${DEFINITIONS} GLM_FORCE_SWIZZLE)
set(DEFINITIONS ${DEFINITIONS} GLM_ENABLE_EXPERIMENTAL)
set(INCLUDES ${INCLUDES} glm-0.9.9.7/)

# FMT - https://fmt.dev/latest/index.html
set(FMT_TES OFF CACHE BOOL "" FORCE)
add_subdirectory(fmt-7.0.3)
set(LIBRARIES ${LIBRARIES} fmt::fmt)
set(INCLUDES ${INCLUDES} fmt-7.0.3/include)

# OpenGL
find_package(OpenGL REQUIRED)
set(LIBRARIES ${LIBRARIES} ${OPENGL_gl_LIBRARY})

# OS-dependent options
if(UNIX)
	set(LIBRARIES ${LIBRARIES} pthread GL dl)
endif(UNIX)
if (WIN32)
endif()

# -------------------------
# Assimp
# -------------------------

FetchContent_Declare(
	assimp
	GIT_REPOSITORY https://github.com/assimp/assimp.git
	GIT_TAG 9ace81a3d22fbb25039983eae27a818f09ef18a9 # Master as of 01/07/2023
)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_UBSAN ON CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

# Disable all importers except for the ones manually enabled below
# Assimp importer names can be found by searching for uses of the `ADD_ASSIMP_IMPORTER` macro in assimp code
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER 		ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER 		ON CACHE BOOL "" FORCE)
# May want to enable these at some point again
set(ASSIMP_BUILD_BLEND_IMPORTER 	OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_COLLADA_IMPORTER 	OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER 		OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)
set(LIBRARIES ${LIBRARIES} assimp)

# -------------------------
# yaml-cpp
# -------------------------

# FetchContent_Declare(
# 	yamlcpp
# 	GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
# 	GIT_TAG 1b50109f7bea60bd382d8ea7befce3d2bd67da5f # Master as of 01/07/2023
# )
# FetchContent_MakeAvailable(yamlcpp)
# set(INCLUDES ${INCLUDES} fetched/yamlcpp-src/include)
# set(LIBRARIES ${LIBRARIES} yamlcpp)
# set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
# set(LIBRARIES ${LIBRARIES} yaml-cpp::yaml-cpp)

# -------------------------
# OpenAL
# -------------------------

FetchContent_Declare(
	openAL	
	GIT_REPOSITORY https://github.com/kcat/openal-soft
	GIT_TAG fde74453a62a1ce4b5efaac0ec1835b9f5731e25
)
FetchContent_MakeAvailable(openAL)
set(INCLUDES ${INCLUDES} ${openAL_SOURCE_DIR}/include/AL)
set(LIBRARIES ${LIBRARIES} OpenAL::OpenAL)

# -------------------------
# object_ptr - https://github.com/anthonywilliams/object_ptr
# -------------------------
set(INCLUDES ${INCLUDES} object_ptr/)

# -------------------------
# Other includes
# RapidJSON, STB
# -------------------------
set(INCLUDES ${INCLUDES} includes)
set(DEFINITIONS ${DEFINITIONS} RAPIDJSON_PARSE_DEFAULT_FLAGS=kParseTrailingCommasFlag|kParseCommentsFlag)
set(DEFINITIONS ${DEFINITIONS} RAPIDJSON_HAS_STDSTRING=1)

# -------------------------
# Thirdparty combination library
# -------------------------
add_library(thirdparty STATIC)
target_sources(thirdparty PUBLIC ${SOURCES})
target_link_libraries(thirdparty PUBLIC ${LIBRARIES})
target_include_directories(thirdparty PUBLIC ${INCLUDES})
target_compile_definitions(thirdparty PUBLIC ${DEFINITIONS})

include(imgui.cmake)
include(physx.cmake)