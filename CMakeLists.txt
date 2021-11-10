add_library(P6 ../../src/p6.cpp)

find_package(OpenGL REQUIRED)
add_library(GLAD STATIC third-party/glad/src/glad.c)
include_directories(
    ${OPENGL_INCLUDE_DIR}
    third-party/glad/include
)