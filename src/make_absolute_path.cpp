#include "make_absolute_path.h"
#include <exe_path/exe_path.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace p6 {

std::filesystem::path make_absolute_path(std::filesystem::path path)
{
    auto res = [&]() {
        if (path.is_relative())
        {
            return exe_path::dir() / path;
        }
        else
        {
            return path;
        }
    }();

    if (!std::filesystem::exists(res))
    {
        using namespace std::literals;
        auto const msg = "Path \""s + res.string() + "\" does not exist. Make sure you didn't misspell it or made it relative to the wrong folder. All relative paths will be relative to your executable's directory.\nAlso, in your CMakeLists.txt you need to call `p6_copy_folder(${PROJECT_NAME} the_folder_containing_your_file)`. See this for an example: https://github.com/JulesFouchy/p6-docs/blob/main/tests/CMakeLists.txt";
        std::cerr << msg << '\n';
        throw std::runtime_error{msg};
    }

    return res;
}

} // namespace p6
