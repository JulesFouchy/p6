#pragma once

#if !defined(NDEBUG) && !defined(__APPLE__)
#define OPENGL_DEBUG 1 // NOLINT(cppcoreguidelines-macro-usage)
#else
#define OPENGL_DEBUG 0
#endif

#if OPENGL_DEBUG

#include <glpp/glpp.hpp>
#include "glfw.h"

namespace p6 {

void APIENTRY GLDebugCallback(GLenum, GLenum, unsigned int, GLenum, GLsizei, const char*, const void*);

} // namespace p6
#endif