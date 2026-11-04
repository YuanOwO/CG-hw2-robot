cmake_minimum_required(VERSION 3.16)
project(MyProject)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Find packages
# -------------------------

if(APPLE)
    # macOS uses the system GLUT and OpenGL frameworks (see Link libraries),
    # with Objective-C++ for the Cocoa parts of the platform layer.
    enable_language(OBJCXX)
else()
    # Windows / Linux: FreeGLUT
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
endif()

# -------------------------
# Source files
# -------------------------

file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS
    src/*.c
)

file(GLOB_RECURSE CXX_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

if(APPLE)
    file(GLOB_RECURSE OBJCXX_SOURCES CONFIGURE_DEPENDS
        src/*.mm
    )
endif()

# -------------------------
# Executable
# -------------------------

add_executable(main
    ${C_SOURCES}
    ${CXX_SOURCES}
)

if(APPLE)
    target_sources(main PRIVATE ${OBJCXX_SOURCES})
    set_source_files_properties(${OBJCXX_SOURCES} PROPERTIES COMPILE_OPTIONS -fobjc-arc)
endif()

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf "$<TARGET_FILE_DIR:main>/assets"
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:main>/assets"
    COMMENT "Linking assets"
)

# -------------------------
# Include directories
# -------------------------

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# -------------------------
# Link libraries
# -------------------------

if(APPLE)
    # The system GLUT framework (Apple OpenGL, hardware accelerated).
    # The OpenGL framework also provides GLU.
    target_link_libraries(main PRIVATE
        "-framework GLUT"
        "-framework OpenGL"
        "-framework Cocoa"   # platform/apple/*.mm
        "-framework Carbon"  # platform/apple/keyboard.cpp: key codes and keyboard layouts
    )

    # OpenGL and GLUT are deprecated on macOS since 10.14.
    target_compile_definitions(main PRIVATE GL_SILENCE_DEPRECATION)
else()
    target_link_libraries(main PRIVATE
        OpenGL::GL
        OpenGL::GLU
        GLUT::GLUT
    )
endif()

if(MINGW)
    target_link_options(main PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()

# -------------------------
# Compile options
# -------------------------

target_compile_options(main PRIVATE
    $<$<CONFIG:Debug>:-g -O1 -Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -fsanitize=undefined>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

target_link_options(main PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=undefined>
)
