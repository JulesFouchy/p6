cmake_minimum_required(VERSION 3.8)

add_library(p6)
add_library(p6::p6 ALIAS p6)
target_compile_features(p6 PRIVATE cxx_std_11)

if(MSVC)
    target_compile_options(p6 PRIVATE /WX /W4)
else()
    target_compile_options(p6 PRIVATE -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion)
endif()

# ---Add glpp-extended---
add_subdirectory(third-party/glpp-extended)
target_link_libraries(p6 PUBLIC glpp::extended)
# ---Add glfw---
add_subdirectory(third-party/glfw)
target_link_libraries(p6 PUBLIC glfw)

# ---Add p6 files---
target_include_directories(p6 PUBLIC include)
target_sources(p6 PRIVATE
    src/Context.cpp
    src/Shader.cpp
    src/details/UniqueGlfwWindow.cpp
    src/details/Time/Clock.cpp
    src/details/Time/Clock_FixedTimestep.cpp
    src/details/Time/Clock_Realtime.cpp
    src/details/RectRenderer.cpp
)