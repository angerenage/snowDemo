cmake_minimum_required(VERSION 3.10)
project(Snow C)

# Find Python3 to run the resource embedder
find_package(Python3 REQUIRED)

# Output paths for generated C and header
set(GENERATED_C "${CMAKE_BINARY_DIR}/generated_resources.c")
set(GENERATED_H "${CMAKE_SOURCE_DIR}/ressource.h")

# Embed all files from ./resources into generated C and H files
add_custom_command(
	OUTPUT ${GENERATED_C} ${GENERATED_H}
	COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/resources_embedder.py
		${CMAKE_SOURCE_DIR}/resources
		${GENERATED_C}
		${GENERATED_H}
	DEPENDS ${CMAKE_SOURCE_DIR}/tools/resources_embedder.py
	COMMENT "Embedding resources..."
)

# Source files + generated resources
file(GLOB SRC_FILES "*.c" "./include/glad.c")
list(APPEND SRC_FILES ${GENERATED_C})
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
		$<$<CONFIG:Debug>:/W4 /Zi>
		$<$<CONFIG:Release>:/W4 /O2 /DNDEBUG>
	)
	target_link_libraries(snow user32 gdi32 opengl32)

elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(snow PRIVATE -Wall -Wextra -Wno-unused-function -fdiagnostics-color=always -msse)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(snow PRIVATE -g)  # Debug: enable symbols only
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

	# Link Windows system libs or X11/GL on Unix
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
