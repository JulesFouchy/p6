#if !defined(NDEBUG)

#include "GLDebug.h"
#include <iostream>

namespace p6 {
namespace details {

void p6GlDebug::clearFromPreviousErrors()
{
    while (glGetError() != GL_NO_ERROR) {
    }
}

bool p6GlDebug::checkForErrors(const char* functionName, const char* filename, int line)
{
    GLenum error; // NOLINT
    bool   bFoundErrors = false;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cerr << "[OpenGL Error] " << glErrorString(error) << " : " << functionName << ' ' << filename << ' ' << line << '\n';
        bFoundErrors = true;
    }
    return bFoundErrors;
}

char const* p6GlDebug::glErrorString(const GLenum err)
{
    switch (err) {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

    default:
        return "UNKNOWN_ERROR_TYPE";
    }
}

} // namespace details
} // namespace p6

#endif