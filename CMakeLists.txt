cmake_minimum_required(VERSION 3.20)

add_library(p6)
add_library(p6::p6 ALIAS p6)
target_compile_features(p6 PRIVATE cxx_std_17)

set(P6_RAW_OPENGL_MODE OFF CACHE BOOL "When enabled, disables a few things that p6 does internally (like enabling alpha blending or using custom framebuffers).
                                       This allows you to have a vanilla OpenGL experience. p6 won't get in the way of what you see in OpenGL tutorials.
                                       Only enable this if you want to learn OpenGL and simply use p6 as an easy way to create a window and handle events, without using the drawing features of p6.")

if(P6_RAW_OPENGL_MODE)
    target_compile_definitions(p6 PUBLIC "P6_RAW_OPENGL_MODE")
endif()

if(p6_ENABLE_WARNINGS_AS_ERRORS)
    message("-- [p6] Enabling warnings as errors for p6")

    if(MSVC)
        target_compile_options(p6 PRIVATE /WX /W4)
    else()
        target_compile_options(p6 PRIVATE -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wno-unknown-pragmas)
    endif()

    set(glpp_extended_ENABLE_WARNINGS_AS_ERRORS ON)
    set(img_ENABLE_WARNINGS_AS_ERRORS ON)
endif()

# ---Add glpp-extended---
add_subdirectory(third-party/glpp-extended)
target_compile_definitions(glpp PUBLIC $<$<CONFIG:Debug>:GLPP_SHOULD_CHECK_ERRORS>)
target_link_libraries(p6 PUBLIC glpp::extended)

# ---Add img---
add_subdirectory(third-party/img)
target_link_libraries(p6 PUBLIC img::img)

# ---Add op---
add_subdirectory(third-party/op)
target_link_libraries(p6 PUBLIC op::op)

# ---Add exe_path---
add_subdirectory(third-party/exe_path)
target_link_libraries(p6 PUBLIC exe_path::exe_path)

# ---Add glfw---
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(third-party/glfw)
target_link_libraries(p6 PUBLIC glfw)
target_compile_options(p6 PUBLIC -x objective-c++)

# ---Add Dear ImGui---
add_library(ImGui)
target_compile_features(ImGui PUBLIC cxx_std_11)

if(WIN32 AND NOT MSVC)
    target_link_libraries(ImGui PRIVATE imm32) # Required only when using mingw, not Visual Studio
endif()

target_include_directories(ImGui SYSTEM PUBLIC third-party/imgui)
target_sources(ImGui PRIVATE
    third-party/imgui/imgui_demo.cpp
    third-party/imgui/imgui_draw.cpp
    third-party/imgui/imgui_tables.cpp
    third-party/imgui/imgui_widgets.cpp
    third-party/imgui/imgui.cpp
    third-party/imgui/misc/cpp/imgui_stdlib.cpp
    third-party/imgui/backends/imgui_impl_glfw.cpp # Build ImGui with glfw
    third-party/imgui/backends/imgui_impl_opengl3.cpp # Build ImGui with OpenGL
)
target_link_libraries(ImGui PRIVATE glfw)
target_link_libraries(p6 PUBLIC ImGui)

# ---Add p6 files---
file(GLOB_RECURSE P6_SOURCES CONFIGURE_DEPENDS src/*)
target_sources(p6 PRIVATE ${P6_SOURCES})

if(p6_ENABLE_WARNINGS_AS_ERRORS)
    set(MAYBE_SYSTEM "")
else()
    set(MAYBE_SYSTEM "SYSTEM")
endif()

target_include_directories(p6 ${MAYBE_SYSTEM} PUBLIC
    "include"
    "third-party" # For Dear ImGui
)

# ---Functions to copy files and folders---
include("third-party/CMakeUtils/files_and_folders.cmake")

function(p6_copy_file TARGET_NAME FILENAME)
    Cool__target_copy_file(${TARGET_NAME} ${FILENAME})
endfunction()

function(p6_copy_folder TARGET_NAME FOLDERNAME)
    Cool__target_copy_folder(${TARGET_NAME} ${FOLDERNAME})
endfunction()
