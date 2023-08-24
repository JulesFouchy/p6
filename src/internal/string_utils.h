#pragma once
#include <optional>
#include <string>

namespace p6::internal {

auto remove_comments(std::string const& str) -> std::string;

static constexpr std::string_view default_word_delimiters{" \n\t\r,;{}[]():/+*-=&|^~%!?<>`."};

/// /!\ The returned string_view is only valid as long as the input string_view is valid!
/// Returns the next word after `startingPos`. A word is a block of characters that doesn't contain any of the `delimiters`.
auto next_word(
    std::string_view text,
    size_t           starting_pos = 0,
    std::string_view delimiters   = default_word_delimiters) -> std::optional<std::string_view>;

/// Returns the indices of the beginning and end of the next word in "text" after position "starting_pos".
/// Words are considered to be separated by one or more characters of "delimiters".
auto find_next_word_position(
    std::string_view text,
    size_t           starting_pos,
    std::string_view delimiters = default_word_delimiters)
    -> std::optional<std::pair<size_t, size_t>>;

} // namespace p6::internal