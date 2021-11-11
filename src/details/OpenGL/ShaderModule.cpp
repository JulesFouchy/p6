#include "ShaderModule.h"
#include <stdexcept>
#include <vector>
#include "GLDebug.h"

namespace p6 {
namespace details {

static void validate_shader_module(GLuint id, const std::string& name)
{
    int result; // NOLINT
    GLDebug(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        GLsizei length; // NOLINT
        GLDebug(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        std::vector<GLchar> error_message;
        error_message.reserve(length);
        GLDebug(glGetShaderInfoLog(id, length, nullptr, error_message.data()));
        throw std::runtime_error(std::string{name + "\nCompilation failed:\n"} + error_message.data());
    }
}

static void compile_shader_module(GLuint id, const ShaderDescription& desc)
{
    const char* src = desc.source_code.c_str();
    GLDebug(glShaderSource(id, 1, &src, nullptr));
    GLDebug(glCompileShader(id));
    validate_shader_module(id, desc.name);
}

ShaderModule::ShaderModule(const ShaderDescription& desc)
    : _shader_module{desc.kind}
{
    compile_shader_module(*_shader_module, desc);
}

} // namespace details
} // namespace p6