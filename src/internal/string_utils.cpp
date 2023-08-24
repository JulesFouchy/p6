#include "string_utils.h"
#include <cassert>
#include <optional>

namespace p6::internal {

template<typename String>
static auto substring_impl(
    const String& text,
    size_t        begin,
    size_t        end) -> String
{
    assert(begin != String::npos);
    assert(end != String::npos);
    assert(begin <= end);
    return text.substr(begin, end - begin);
}

static auto substring(
    std::string_view          text,
    std::pair<size_t, size_t> begin_end) -> std::string_view
{
    return substring_impl(text, begin_end.first, begin_end.second);
}

static auto substring(
    const std::string& text,
    size_t             begin,
    size_t             end) -> std::string
{
    return substring_impl(text, begin, end);
}

enum class CommentParsingState {
    Idle,
    JustSawASlash,
    InsideOneLineComment,
    InsideMultilineComment,
    JustSawAStarInsideMultilineComment,
};

static auto is_not_inside_comment(CommentParsingState state) -> bool
{
    return state == CommentParsingState::Idle
           || state == CommentParsingState::JustSawASlash;
}

auto remove_comments(std::string const& str) -> std::string
{
    std::string res{};
    res.reserve(str.length());

    auto   state               = CommentParsingState::Idle;
    size_t previous_char_index = 0;

    auto const enter_comment = [&](size_t i) {
        res += substring(str, previous_char_index, i - 1);
    };
    auto const exit_comment = [&](size_t i) {
        previous_char_index = i + 1u;
    };

    for (size_t i = 0; i < str.length(); ++i) // NOLINT(modernize-loop-convert)
    {
        char const c = str[i];
        switch (state)
        {
        case CommentParsingState::Idle:
        {
            if (c == '/')
                state = CommentParsingState::JustSawASlash;
            break;
        }
        case CommentParsingState::JustSawASlash:
        {
            if (c == '/')
            {
                state = CommentParsingState::InsideOneLineComment;
                enter_comment(i);
            }
            else if (c == '*')
            {
                state = CommentParsingState::InsideMultilineComment;
                enter_comment(i);
            }
            else
            {
                state = CommentParsingState::Idle;
            }
            break;
        }
        case CommentParsingState::InsideOneLineComment:
        {
            if (c == '\n')
            {
                state = CommentParsingState::Idle;
                exit_comment(i);
                res += '\n';
            }
            break;
        }
        case CommentParsingState::InsideMultilineComment:
        {
            if (c == '*')
                state = CommentParsingState::JustSawAStarInsideMultilineComment;
            break;
        }
        case CommentParsingState::JustSawAStarInsideMultilineComment:
        {
            if (c == '/')
            {
                state = CommentParsingState::Idle;
                exit_comment(i);
            }
            else
            {
                state = CommentParsingState::InsideMultilineComment;
            }
            break;
        }
        }
    }

    if (is_not_inside_comment(state))
        res += substring(str, previous_char_index, str.length());

    return res;
}

auto find_next_word_position(
    std::string_view text,
    size_t           starting_pos,
    std::string_view delimiters)
    -> std::optional<std::pair<size_t, size_t>>
{
    const auto idx1 = text.find_first_not_of(delimiters, starting_pos);
    if (idx1 == std::string_view::npos)
    {
        return std::nullopt;
    }
    auto idx2 = text.find_first_of(delimiters, idx1);
    if (idx2 == std::string_view::npos)
    {
        idx2 = text.size();
    }
    return std::make_pair(idx1, idx2);
}

auto next_word(
    std::string_view text,
    size_t           starting_pos,
    std::string_view delimiters) -> std::optional<std::string_view>
{
    auto const position = find_next_word_position(text, starting_pos, delimiters);
    if (!position)
        return std::nullopt;

    return substring(text, *position);
}

} // namespace p6::internal