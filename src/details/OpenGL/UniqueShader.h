#pragma once

#include <glad/glad.h>

namespace p6 {
namespace details {

class UniqueShader {
public:
    UniqueShader();
    ~UniqueShader();
    UniqueShader(const UniqueShader&) = delete;
    UniqueShader& operator=(const UniqueShader&) = delete;
    UniqueShader(UniqueShader&& rhs) noexcept;
    UniqueShader& operator=(UniqueShader&& rhs) noexcept;

    GLuint operator*() const { return _id; }

private:
    GLuint _id;
};

} // namespace details
} // namespace p6