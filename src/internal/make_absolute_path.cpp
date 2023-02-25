#include "make_absolute_path.h"
#include <exe_path/exe_path.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace p6::internal {

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
        auto const msg = "Path \""s + res.string() + "\" does not exist. Make sure you didn't misspell it or made it relative to the wrong folder. All relative paths will be relative to your executable's directory.";
        std::cerr << msg << '\n';
        throw std::runtime_error{msg};
    }

    return res;
}

} // namespace p6::internal
