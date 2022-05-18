#include "make_absolute_path.h"
#include <exe_path/exe_path.h>

namespace p6::details
{
std::filesystem::path make_absolute_path(std::filesystem::path path)
{
    if (path.is_relative())
    {
        return exe_path::dir() / path;
    }
    else
    {
        return path;
    }
}

} // namespace p6::details
