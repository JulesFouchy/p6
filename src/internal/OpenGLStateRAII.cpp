#include "OpenGLStateRAII.h"
#include <glpp/glpp.hpp>

namespace p6::internal {

OpenGLStateRAII::OpenGLStateRAII()
{
    // Blend
    _blend_is_enabled = glIsEnabled(GL_BLEND); // NOLINT(*prefer-member-initializer)
    glGetIntegerv(GL_BLEND_EQUATION, &_blend_equation);
    glGetIntegerv(GL_BLEND_SRC_RGB, &_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &_dst_alpha);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);                // We use premultiplied alpha, which is the only convention that makes actual sense
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // https://apoorvaj.io/alpha-compositing-opengl-blending-and-premultiplied-alpha/

    // Depth test
    _depth_test_is_enabled = glIsEnabled(GL_DEPTH_TEST); // NOLINT(*prefer-member-initializer)
    glDisable(GL_DEPTH_TEST);

    // Backface culling
    _backface_culling_is_enabled = glIsEnabled(GL_CULL_FACE); // NOLINT(*prefer-member-initializer)
    glDisable(GL_CULL_FACE);
}

OpenGLStateRAII::~OpenGLStateRAII()
{
    // Blend
    if (_blend_is_enabled)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    glBlendEquation(_blend_equation);
    glBlendFuncSeparate(_src_rgb, _dst_rgb, _src_alpha, _dst_alpha);

    // Depth test
    if (_depth_test_is_enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    // Backface culling
    if (_backface_culling_is_enabled)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}

} // namespace p6::internal