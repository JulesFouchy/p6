#pragma once

#include <glad/glad.h>

namespace p6 {
namespace details {

class UniqueVAO {
public:
    UniqueVAO();
    ~UniqueVAO();
    UniqueVAO(const UniqueVAO&) = delete;
    UniqueVAO& operator=(const UniqueVAO&) = delete;
    UniqueVAO(UniqueVAO&& rhs) noexcept;
    UniqueVAO& operator=(UniqueVAO&& rhs) noexcept;

    GLuint operator*() const { return _id; }

private:
    GLuint _id;
};

} // namespace details
} // namespace p6