#include "append_number_if_file_already_exists.h"
#include <string>

namespace p6::internal {

std::filesystem::path append_number_if_file_already_exists(std::filesystem::path path)
{
    auto final_path = path;
    int  i          = 1;
    while (std::filesystem::exists(final_path))
    {
        final_path.replace_filename(
            path.stem().string() + "[" + std::to_string(i) + "]" + path.extension().string());
        i++;
    }
    return final_path;
}

} // namespace p6::internal
