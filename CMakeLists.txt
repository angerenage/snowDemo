cmake_minimum_required(VERSION 3.10)
project(Snow C)

# Find Python3 to run the resource embedder
find_package(Python3 REQUIRED)

include(${CMAKE_SOURCE_DIR}/tools/ToolDownloader.cmake)

# Paths
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/resources/shaders")
set(SHADER_PACK "${CMAKE_BINARY_DIR}/shaders.pack")
set(CRUSHED_HEADER "${CMAKE_SOURCE_DIR}/glsl_unpacker.h")
set(CRUSHED_SOURCE "${CMAKE_BINARY_DIR}/glsl_unpacker.c")
set(GENERATED_C "${CMAKE_BINARY_DIR}/generated_resources.c")
set(GENERATED_H "${CMAKE_SOURCE_DIR}/resources.h")

# Collect shader files
file(GLOB SHADER_FILES "${SHADER_DIR}/*.glsl")

# Run GLSL-Crusher
add_custom_command(
	OUTPUT ${SHADER_PACK} ${CRUSHED_HEADER} ${CRUSHED_SOURCE}
	COMMAND ${GLSL_CRUSHER_EXECUTABLE}
		${SHADER_FILES}
		-p ${SHADER_PACK}
		-h ${CRUSHED_HEADER}
		-c ${CRUSHED_SOURCE}
		--max-token-size $<IF:$<CONFIG:Release>,500,10>
		--core
		$<$<CONFIG:Release>:--verbose>
	DEPENDS ${SHADER_FILES}
	COMMENT "Crushing shaders..."
	VERBATIM
	COMMAND_EXPAND_LISTS
)

# Find all other resources (excluding shaders)
file(GLOB_RECURSE ALL_RESOURCES "${CMAKE_SOURCE_DIR}/resources/*")
list(FILTER ALL_RESOURCES EXCLUDE REGEX "\\.glsl$")

# Embed resources
add_custom_command(
	OUTPUT ${GENERATED_C} ${GENERATED_H}
	COMMAND echo ${ALL_RESOURCES}
	COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/resources_embedder.py
		${GENERATED_C} ${GENERATED_H}
		${SHADER_PACK} ${ALL_RESOURCES}
	DEPENDS ${CMAKE_SOURCE_DIR}/tools/resources_embedder.py ${ALL_RESOURCES} ${SHADER_PACK}
	COMMENT "Embedding resources..."
)

# Source files + generated resources
file(GLOB SRC_FILES "*.c" "./include/glad.c")
list(APPEND SRC_FILES ${GENERATED_C})
list(APPEND SRC_FILES ${CRUSHED_SOURCE})
add_executable(snow ${SRC_FILES})

# Include project and third-party headers
target_include_directories(snow PRIVATE ${CMAKE_SOURCE_DIR} include)

# Define DEBUG macro in debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(snow PRIVATE DEBUG)
endif()

# Compiler specific flags and linking
if(MSVC)
	target_compile_options(snow PRIVATE 
		$<$<CONFIG:Debug>:/W4 /Zi /DDEBUG /MTd>
		$<$<CONFIG:Release>:/W4 /O2 /MT>
	)

	set_target_properties(snow PROPERTIES
		LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF /RELEASE /DEBUG:NONE /INCREMENTAL:NO"
	)

	target_link_libraries(snow user32 gdi32 opengl32)

elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(snow PRIVATE -Wall -Wextra -Wno-unused-function -fdiagnostics-color=always -msse)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(snow PRIVATE -g "-DDEBUG")  # Debug: enable symbols only
	else()
		if(NOT WIN32)
			# Aggressive release flags (non-Windows only)
			set(RELEASE_FLAGS "-Os -s -ffast-math -ffreestanding -fno-builtin -mno-accumulate-outgoing-args -fmerge-constants -fmerge-all-constants")
			set(RELEASE_FLAGS "${RELEASE_FLAGS} -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-ident -fno-stack-protector -fomit-frame-pointer")
			set(RELEASE_FLAGS "${RELEASE_FLAGS} -fshort-enums -fshort-wchar -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops")
			set(RELEASE_FLAGS "${RELEASE_FLAGS} -freorder-blocks -freorder-functions -ffunction-sections -fdata-sections -flto")
			set(CMAKE_C_FLAGS_RELEASE "${RELEASE_FLAGS}")
			set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections,--strip-all")
		endif()
	endif()

	if(WIN32)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
		target_link_libraries(snow user32 gdi32 opengl32)
	elseif(UNIX)
		target_link_libraries(snow m X11 GL)
	endif()
endif()

# Final binary output directory
if(NOT MSVC)
	set_target_properties(snow PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/RelWithDebInfo"
		RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/MinSizeRel"
	)
else()
	set_target_properties(snow PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)
endif()
