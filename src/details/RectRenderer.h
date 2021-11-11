#pragma once

#include "OpenGL/UniqueBuffer.h"
#include "OpenGL/UniqueVAO.h"


namespace p6 {
namespace details {

class RectRenderer {
public:
    RectRenderer();
    void render() const;

private:
    UniqueVAO    _vao;
    UniqueBuffer _vbo;
    UniqueBuffer _ibo;
};

} // namespace details
} // namespace p6