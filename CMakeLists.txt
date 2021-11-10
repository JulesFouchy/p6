add_library(P6 
    src/Context.cpp
    src/UniqueGlfwWindow.cpp
    src/Time/Clock.cpp
    src/Time/Clock_FixedTimestep.cpp
    src/Time/Clock_Realtime.cpp
)

# ---Add OpenGL and glad---
find_package(OpenGL REQUIRED)
add_library(GLAD STATIC third-party/glad/src/glad.c)
target_include_directories(GLAD PRIVATE
    third-party/glad/include
)
target_include_directories(P6 PRIVATE
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
target_include_directories(P6 PRIVATE
    ${GLFW_DIR}/include
    ${GLFW_DIR}/deps
)
target_link_libraries(P6 INTERFACE
    glfw
    GLAD
)