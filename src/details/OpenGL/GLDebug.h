#pragma once

#include <glad/glad.h>
#include <cassert>

namespace p6 {
namespace details {

#if !defined(NDEBUG)

/// Wrap all you OpenGL calls in this macro : it will add debug checks. Something like : GLDebug(GLuint programID = glCreateProgram())
/// It is not strictly necessary though because modern OpenGL debugging is enabled too. It's only to help those who don't have the advandced debugging available on their machine.
#define GLDebug(x)                                     \
    p6::details::p6GlDebug::clearFromPreviousErrors(); \
    x;                                                 \
    assert(!p6::details::p6GlDebug::checkForErrors(#x, __FILE__, __LINE__))

namespace p6GlDebug {
void        clearFromPreviousErrors();
bool        checkForErrors(const char* functionName, const char* filename, int line);
char const* glErrorString(const GLenum err);
} // namespace p6GlDebug

#else
#define GLDebug(x) x
#endif

} // namespace details
} // namespace p6