#include "RectRenderer.h"
#include <array>
#include "OpenglStateRAII.h"

namespace p6::internal {

RectRenderer::RectRenderer()
{
    // VAO
    glBindVertexArray(_vao.id());
    // VBO
    const std::array<float, 16> vertices = {
        -1.f, -1.f, 0.f, 0.f,
        -1.f, +1.f, 0.f, 1.f,
        +1.f, +1.f, 1.f, 1.f,
        +1.f, -1.f, 1.f, 0.f};
    const auto vertices_size_in_bytes = vertices.size() * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo.id());
    glBufferData(GL_ARRAY_BUFFER, vertices_size_in_bytes, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_size_in_bytes, vertices.data());
    const auto stride = 4 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(2 * sizeof(float))); // NOLINT
    // IBO
    const std::array<GLuint, 6> indices = {
        0, 1, 2,
        0, 2, 3};
    const auto indices_size_in_bytes = indices.size() * sizeof(GLuint);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo.id());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size_in_bytes, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices_size_in_bytes, indices.data());
}

void RectRenderer::render() const
{
    auto raii = OpenGLStateRAII{};
    glBindVertexArray(_vao.id());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

} // namespace p6::internal