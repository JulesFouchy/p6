#include "UniqueBuffer.h"

namespace p6 {
namespace details {

UniqueBuffer::UniqueBuffer()
{
    glGenBuffers(1, &_id);
}

UniqueBuffer::~UniqueBuffer()
{
    glDeleteBuffers(1, &_id);
}

UniqueBuffer::UniqueBuffer(UniqueBuffer&& rhs) noexcept
    : _id{rhs._id}
{
    rhs._id = 0;
}

UniqueBuffer& UniqueBuffer::operator=(UniqueBuffer&& rhs) noexcept
{
    if (this != &rhs) {
        _id     = rhs._id;
        rhs._id = 0;
    }
    return *this;
}

} // namespace details
} // namespace p6