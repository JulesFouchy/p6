cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0079 NEW) # To allow us to add libraries to the ImGui target
cmake_policy(SET CMP0076 NEW) # To allow us to use target_sources() on ImGui

add_library(p6)
add_library(p6::p6 ALIAS p6)
target_compile_features(p6 PRIVATE cxx_std_17)

# if(MSVC)
#     target_compile_options(p6 PRIVATE /WX /W4)
# else()
#     target_compile_options(p6 PRIVATE -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion)
# endif()

# ---Add glpp-extended---
add_subdirectory(third-party/glpp-extended)
target_compile_definitions(glpp PRIVATE $<$<CONFIG:Debug>:GLPP_CHECK_ERRORS>)
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
    src/Image.cpp
    src/math.cpp
    src/ImGui/ImGuiWrapper.cpp
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