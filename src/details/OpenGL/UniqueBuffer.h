#pragma once

#include <glad/glad.h>

namespace p6 {
namespace details {

class UniqueBuffer {
public:
    UniqueBuffer();
    ~UniqueBuffer();
    UniqueBuffer(const UniqueBuffer&) = delete;
    UniqueBuffer& operator=(const UniqueBuffer&) = delete;
    UniqueBuffer(UniqueBuffer&& rhs) noexcept;
    UniqueBuffer& operator=(UniqueBuffer&& rhs) noexcept;

    GLuint operator*() const { return _id; }

private:
    GLuint _id;
};

} // namespace details
} // namespace p6