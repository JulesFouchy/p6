#pragma once

#include <glad/glad.h>
#include "ShaderDescription.h"
#include "UniqueShaderModule.h"

namespace p6 {
namespace details {

class ShaderModule {
public:
    /// Throws std::runtime_error if there is an error while compiling the shader source code
    explicit ShaderModule(const ShaderDescription& desc);

    GLuint operator*() const { return *_shader_module; }

private:
    UniqueShaderModule _shader_module;
};

} // namespace details
} // namespace p6