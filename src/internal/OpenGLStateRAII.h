#pragma once
#include <glpp/glpp.hpp>

namespace p6::internal {

class OpenGLStateRAII {
public:
    OpenGLStateRAII();
    ~OpenGLStateRAII();
    OpenGLStateRAII(OpenGLStateRAII const&)                = delete;
    OpenGLStateRAII& operator=(OpenGLStateRAII const&)     = delete;
    OpenGLStateRAII(OpenGLStateRAII&&) noexcept            = delete;
    OpenGLStateRAII& operator=(OpenGLStateRAII&&) noexcept = delete;

private:
    // Blend
    GLboolean _blend_is_enabled{};
    GLint     _blend_equation{};
    GLint     _src_rgb{};
    GLint     _dst_rgb{};
    GLint     _src_alpha{};
    GLint     _dst_alpha{};

    // Depth test
    GLboolean _depth_test_is_enabled{};

    // Backface culling
    GLboolean _backface_culling_is_enabled{};
};

} // namespace p6::internal