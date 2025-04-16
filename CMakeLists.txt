cmake_minimum_required(VERSION 3.10)
project(Snow C)

file(GLOB SRC_FILES "*.c" "include/glad.c")
add_executable(snow ${SRC_FILES})

target_include_directories(snow PRIVATE include)

target_compile_definitions(snow PRIVATE DEBUG NO_SOUND WIN32)

if(MSVC)
	target_compile_options(snow PRIVATE 
		$<$<CONFIG:Debug>:/W4 /Zi>
		$<$<CONFIG:Release>:/W4 /O2 /DNDEBUG>
	)

	target_link_libraries(snow user32 gdi32 opengl32 lzma)

elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(snow PRIVATE 
		-Wall -Wextra -Wno-unused-function -fdiagnostics-color=always -msse
	)

	set(RELEASE_FLAGS "-Os -s -ffast-math -ffreestanding -fno-builtin -mno-accumulate-outgoing-args -fmerge-constants -fmerge-all-constants")
	set(RELEASE_FLAGS "${RELEASE_FLAGS} -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-ident -fno-stack-protector -fomit-frame-pointer")
	set(RELEASE_FLAGS "${RELEASE_FLAGS} -fshort-enums -fshort-wchar -fno-align-functions -fno-align-jumps -fno-align-labels -fno-align-loops")
	set(RELEASE_FLAGS "${RELEASE_FLAGS} -freorder-blocks -freorder-functions -ffunction-sections -fdata-sections -flto")
	set(CMAKE_C_FLAGS_RELEASE "${RELEASE_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections,--strip-all")

	if(WIN32)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
		target_link_libraries(snow user32 gdi32 opengl32 lzma)
	elseif(UNIX)
		target_link_libraries(snow m X11 GL lzma)
	endif()
endif()

set_target_properties(snow PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET snow POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/resources"
		"$<TARGET_FILE_DIR:snow>/resources"
	COMMENT "Copying resource files next to executable"
)
