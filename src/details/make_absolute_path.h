#pragma once

#include <filesystem>

namespace p6::details {

std::filesystem::path make_absolute_path(std::filesystem::path path);

} // namespace p6::details
