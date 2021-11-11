#include "UniqueShaderModule.h"
#include <stdexcept>
#include <string>

namespace p6 {
namespace details {

static GLenum opengl_shader_kind(ShaderKind kind)
{
    switch (kind) {
    case ShaderKind::Vertex:
        return GL_VERTEX_SHADER;
    case ShaderKind::Fragment:
        return GL_FRAGMENT_SHADER;
    default:
        throw std::runtime_error("[p6 internal error] opengl_shader_kind unknown value: " + std::to_string(static_cast<int>(kind)));
    }
}

UniqueShaderModule::UniqueShaderModule(ShaderKind shader_kind)
    : _id{glCreateShader(opengl_shader_kind(shader_kind))}
{
}

UniqueShaderModule::~UniqueShaderModule()
{
    glDeleteShader(_id);
}

UniqueShaderModule::UniqueShaderModule(UniqueShaderModule&& rhs) noexcept
    : _id{rhs._id}
{
    rhs._id = decltype(rhs._id){};
}

UniqueShaderModule& UniqueShaderModule::operator=(UniqueShaderModule&& rhs) noexcept
{
    if (&rhs != this) {
        _id     = rhs._id;
        rhs._id = decltype(rhs._id){};
    }
    return *this;
}

} // namespace details
} // namespace p6