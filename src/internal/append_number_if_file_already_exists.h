#pragma once

#include <filesystem>

namespace p6::internal {

std::filesystem::path append_number_if_file_already_exists(std::filesystem::path path);

} // namespace p6::internal
