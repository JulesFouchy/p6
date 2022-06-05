#include "make_directories_if_necessary.h"

namespace p6::internal {

void make_directories_if_necessary(std::filesystem::path path)
{
    path = path.parent_path(); // Remove the filename part, keep only the directory path
    if (!std::filesystem::is_directory(path)
        || !std::filesystem::exists(path))
    {
        std::filesystem::create_directories(path);
    }
}

} // namespace p6::internal
