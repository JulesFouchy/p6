cmake_minimum_required(VERSION 3.20)

add_library(p6)
add_library(p6::p6 ALIAS p6)
target_compile_features(p6 PRIVATE cxx_std_17)

if (p6_ENABLE_WARNINGS_AS_ERRORS)
    message("-- [p6] Enabling warnings as errors for p6")
    if(MSVC)
        target_compile_options(p6 PRIVATE /WX /W4)
    else()
        target_compile_options(p6 PRIVATE -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion)
    endif()
    set(glpp_extended_ENABLE_WARNINGS_AS_ERRORS true)
    set(img_ENABLE_WARNINGS_AS_ERRORS true)
else()
    message("-- [p6] Not using warnings as errors for p6")
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
# ---Add exe_path---
add_subdirectory(third-party/exe_path)
target_link_libraries(p6 PUBLIC exe_path::exe_path)
# ---Add glfw---
add_subdirectory(third-party/glfw)
target_link_libraries(p6 PUBLIC glfw)
# ---Add Dear ImGui---
add_subdirectory(third-party/imgui)
# Build ImGui with glfw
target_link_libraries(ImGui PRIVATE glfw)
target_sources(ImGui PRIVATE third-party/imgui/backends/imgui_impl_glfw.cpp)
# Build ImGui with OpenGL
target_sources(ImGui PRIVATE third-party/imgui/backends/imgui_impl_opengl3.cpp)
#
target_link_libraries(p6 PUBLIC ImGui::ImGui)

# ---Add p6 files---
target_include_directories(p6 PUBLIC include
                                     third-party)
target_sources(p6 PRIVATE
    src/Context.cpp
    src/Shader.cpp
    src/Canvas.cpp
    src/Image.cpp
    src/math.cpp
    src/Transform2D.cpp
    src/details/ImGuiWrapper.cpp
    src/details/UniqueGlfwWindow.cpp
    src/details/Time/Clock.cpp
    src/details/Time/Clock_FixedTimestep.cpp
    src/details/Time/Clock_Realtime.cpp
    src/details/RectRenderer.cpp
    src/details/TextRenderer.cpp
)

# ---Functions to copy files and folders---
include("third-party/CMakeUtils/files_and_folders.cmake")

function(p6_copy_file TARGET_NAME FILENAME)
    Cool__target_copy_file(${TARGET_NAME} ${FILENAME})
endfunction()

function(p6_copy_folder TARGET_NAME FOLDERNAME)
    Cool__target_copy_folder(${TARGET_NAME} ${FOLDERNAME})
endfunction()

# ---Functions to setup p6---
cmake_path(RELATIVE_PATH CMAKE_CURRENT_SOURCE_DIR BASE_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE _P6_PATH)
set(P6_PATH ${_P6_PATH} CACHE INTERNAL "") # Makes the variable accessible by parent CMakeLists.txt files; this allows them to call p6_setup_resources() manually if they want to
function(p6_setup_resources TARGET)
    Cool__target_copy_folder(${TARGET} ${P6_PATH}/res p6/res)
endfunction()

#! You **must** call this function to add p6 as a library used by your TARGET
# Instead of calling the usual target_link_libraries(), because we need to do some additional setup
function(target_link_p6_library TARGET)
    target_link_libraries(${TARGET} p6::p6)
    p6_setup_resources(${TARGET})
endfunction()
