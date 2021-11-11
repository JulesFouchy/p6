#include "UniqueVAO.h"

namespace p6 {
namespace details {

UniqueVAO::UniqueVAO()
{
    glGenVertexArrays(1, &_id);
}

UniqueVAO::~UniqueVAO()
{
    glDeleteVertexArrays(1, &_id);
}

UniqueVAO::UniqueVAO(UniqueVAO&& rhs) noexcept
    : _id{rhs._id}
{
    rhs._id = 0;
}

UniqueVAO& UniqueVAO::operator=(UniqueVAO&& rhs) noexcept
{
    if (this != &rhs) {
        _id     = rhs._id;
        rhs._id = 0;
    }
    return *this;
}

} // namespace details
} // namespace p6