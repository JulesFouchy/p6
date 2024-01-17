#include "OpenGL_Debug.h"
#if OPENGL_DEBUG
#include <iostream>
#include <stdexcept>

namespace p6 {

void APIENTRY GLDebugCallback(
    GLenum       source, // NOLINT(bugprone-easily-swappable-parameters)
    GLenum       type,
    unsigned int id,
    GLenum       severity,
    GLsizei /*length*/,
    const char* openGLMessage,
    const void* /*userParam*/
)
{
    // ignore non-significant error / warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    { // Check if we have already seen the message
        static std::vector<unsigned int> already_seen_ids;
        if (std::find(already_seen_ids.begin(), already_seen_ids.end(), id) != already_seen_ids.end())
            return;
        already_seen_ids.push_back(id);
    }

    // Make message
    std::string message;
    message += "OpenGL Debug message (id=" + std::to_string(id) + ")\n" + openGLMessage;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API: message += "\n\nSource: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: message += "\n\nSource: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: message += "\n\nSource: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: message += "\n\nSource: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION: message += "\n\nSource: Application"; break;
    case GL_DEBUG_SOURCE_OTHER: message += "\n\nSource: Other"; break;
    default: message += "\n\nSource: Unknown"; break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR: message += "\nType: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: message += "\nType: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: message += "\nType: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY: message += "\nType: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: message += "\nType: Performance"; break;
    case GL_DEBUG_TYPE_MARKER: message += "\nType: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP: message += "\nType: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP: message += "\nType: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER: message += "\nType: Other"; break;
    default: message += "\nType: Unknown"; break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH: message += "\nSeverity: High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: message += "\nSeverity: Medium"; break;
    case GL_DEBUG_SEVERITY_LOW: message += "\nSeverity: Low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: message += "\nSeverity: Notification"; break;
    default: message += "\nSeverity: Unknown"; break;
    }

    // Log
    if (type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR || severity == GL_DEBUG_SEVERITY_HIGH)
    {
        std::cerr << message << '\n';
        throw std::runtime_error{message};
    }
    else
    {
        std::cout << message << '\n';
    }
}

} // namespace p6

#endif