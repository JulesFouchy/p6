cmake_minimum_required(VERSION 3.8)

add_library(p6)
add_library(p6::p6 ALIAS p6)
target_compile_features(p6 PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(p6 PRIVATE /WX /W4)
else()
    target_compile_options(p6 PRIVATE -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion)
endif()

# ---Add glpp-extended---
add_subdirectory(third-party/glpp-extended)
target_compile_definitions(glpp PRIVATE $<$<CONFIG:Debug>:GLPP_SHOULD_CHECK_ERRORS>)
target_link_libraries(p6 PUBLIC glpp::extended)
# ---Add img---
add_subdirectory(third-party/img)
target_link_libraries(p6 PUBLIC img::img)
# ---Add op---
add_subdirectory(third-party/op)
target_link_libraries(p6 PUBLIC op::op)
# ---Add glfw---
add_subdirectory(third-party/glfw)
target_link_libraries(p6 PUBLIC glfw)

# ---Add p6 files---
target_include_directories(p6 PUBLIC include)
target_sources(p6 PRIVATE
    src/Context.cpp
    src/Shader.cpp
    src/Canvas.cpp
    src/Image.cpp
    src/math.cpp
    src/Transform2D.cpp
    src/details/UniqueGlfwWindow.cpp
    src/details/Time/Clock.cpp
    src/details/Time/Clock_FixedTimestep.cpp
    src/details/Time/Clock_Realtime.cpp
    src/details/RectRenderer.cpp
)

# ---Functions to copy files and folders---
include("third-party/CMakeUtils/files_and_folders.cmake")

function(p6_copy_file TARGET_NAME FILENAME)
    Cool__target_copy_file(${TARGET_NAME} ${FILENAME})
endfunction()

function(p6_copy_folder TARGET_NAME FOLDERNAME)
    Cool__target_copy_folder(${TARGET_NAME} ${FOLDERNAME})
endfunction()