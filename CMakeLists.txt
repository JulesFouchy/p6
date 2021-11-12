cmake_minimum_required(VERSION 3.8)

add_library(p6 
    src/Context.cpp
    src/Shader.cpp
    src/details/UniqueGlfwWindow.cpp
    src/details/Time/Clock.cpp
    src/details/Time/Clock_FixedTimestep.cpp
    src/details/Time/Clock_Realtime.cpp
    src/details/OpenGL/GLDebug.cpp
    src/details/OpenGL/UniqueVAO.cpp
    src/details/OpenGL/UniqueBuffer.cpp
    src/details/OpenGL/UniqueShader.cpp
    src/details/OpenGL/UniqueShaderModule.cpp
    src/details/OpenGL/ShaderModule.cpp
    src/details/RectRenderer.cpp
)
target_include_directories(p6 PUBLIC include)

if(MSVC)
    target_compile_options(p6 PRIVATE /WX /W4)
else()
    target_compile_options(p6 PRIVATE -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wconversion -Wsign-conversion)
endif()

# ---Add OpenGL and glad---
find_package(OpenGL REQUIRED)
add_library(GLAD STATIC third-party/glad/src/glad.c)
target_include_directories(GLAD PRIVATE
    third-party/glad/include
)
target_include_directories(p6 PUBLIC
    ${OPENGL_INCLUDE_DIR}
    third-party/glad/include
)
# ---Add glfw---
set(GLFW_DIR third-party/glfw)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
target_include_directories(p6 PRIVATE
    ${GLFW_DIR}/include
    ${GLFW_DIR}/deps
)
target_link_libraries(p6 INTERFACE
    glfw
    GLAD
)
# ---Add glm---
target_include_directories(p6 PUBLIC
   third-party/glm
)