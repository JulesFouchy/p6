#pragma once

#include <string>
#include "ShaderKind.h"

namespace p6 {
namespace details {

struct ShaderDescription {
    std::string source_code;
    ShaderKind  kind;
    std::string name;
};

} // namespace details
} // namespace p6
