#include "Shader.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string_view>
#include "internal/string_utils.h"
#include "make_absolute_path.h"

namespace p6 {

GLenum Shader::s_available_texture_slot{0};

Shader::Shader(std::string_view fragment_source_code)
    : Shader{ShaderSources{
        /*. vertex = */ R"(
#version 410

layout(location = 0) in vec2 _vertex_position;
layout(location = 1) in vec2 _texture_coordinates;
out vec2 _raw_uv;
out vec2 _uniform_uv;
out vec2 _canvas_uv;

uniform mat3 _transform;
uniform float _window_inverse_aspect_ratio;
uniform vec2 _size;
uniform float _aspect_ratio;

void main()
{
    vec2 pos = _vertex_position;
    vec3 pos3 = _transform * vec3(pos, 1.);
    pos = pos3.xy / pos3.z;
    pos.x *= _window_inverse_aspect_ratio;
    gl_Position = vec4(pos, 0., 1.);
    _raw_uv = _texture_coordinates;
    _uniform_uv = (_texture_coordinates - 0.5) * vec2(_aspect_ratio, 1.) * 2.;
    _canvas_uv = (_texture_coordinates - 0.5) * _size * 2.;
}
    )",
        /* .fragment = */ std::string{fragment_source_code},
    }}
{}

Shader::Shader(std::string_view vertex_source_code, std::string_view fragment_source_code)
    : Shader{ShaderSources{
        /* .vertex   = */ std::string{vertex_source_code},
        /* .fragment = */ std::string{fragment_source_code},
    }}
{}

#if !defined(NDEBUG)
static auto find_exact_word(std::string_view text, std::string_view keyword, size_t start = 0) -> size_t
{
    auto const is_delimiter = [](char c) {
        return internal::default_word_delimiters.find(c) != std::string_view::npos;
    };
    auto const offset = text.find(keyword, start);
    if (offset == std::string_view::npos)
        return std::string_view::npos;
    if ((offset > 0 && !is_delimiter(text[offset - 1]))
        || (offset + keyword.size() < text.size() && !is_delimiter(text[offset + keyword.size()])))
    {
        return find_exact_word(text, keyword, offset + keyword.size());
    }
    return offset;
}

static void append_uniforms_names(std::string source_code, std::set<std::string>& _uniforms_names)
{
    source_code        = internal::remove_comments(source_code);
    auto const keyword = std::string_view{"uniform"};

    size_t offset = find_exact_word(source_code, keyword);
    while (offset != std::string::npos)
    {
        offset += keyword.length();

        auto const type_pos = internal::find_next_word_position(source_code, offset);
        if (!type_pos)
            break;
        auto const name = internal::next_word(source_code, type_pos->second);
        if (!name)
            break;

        _uniforms_names.insert(std::string{*name});

        offset = find_exact_word(source_code, keyword, type_pos->second);
    }
}
#endif

template<glpp::ShaderType Type>
static auto gen_shader_module(std::optional<std::string> const& source_code
#if !defined(NDEBUG)
                              ,
                              std::string const&     stage_name,
                              std::set<std::string>& uniforms_names
#endif
                              ) -> std::optional<glpp::internal::Shader<Type>>
{
    if (!source_code)
        return std::nullopt;
    auto module = glpp::internal::Shader<Type>{source_code->data()};
#if !defined(NDEBUG)
    {
        append_uniforms_names(*source_code, uniforms_names);
        auto const err = module.check_compilation_errors();
        if (err)
        {
            auto const msg = stage_name + " shader compilation failed:\n" + err.message();
            std::cerr << msg << '\n';
            throw std::runtime_error{msg};
        }
    }
#endif
    return module;
}

Shader::Shader(ShaderSources const& sources)
{
    auto const vert = gen_shader_module<glpp::ShaderType::Vertex>(sources.vertex
#if !defined(NDEBUG)
                                                                  ,
                                                                  "Vertex",
                                                                  _uniforms_names
#endif
    );
    auto const frag = gen_shader_module<glpp::ShaderType::Fragment>(sources.fragment
#if !defined(NDEBUG)
                                                                    ,
                                                                    "Fragment",
                                                                    _uniforms_names
#endif
    );
    auto const geom = gen_shader_module<glpp::ShaderType::Geometry>(sources.geometry
#if !defined(NDEBUG)
                                                                    ,
                                                                    "Geometry",
                                                                    _uniforms_names
#endif
    );
    auto const tess_ctrl = gen_shader_module<glpp::ShaderType::TessellationControl>(sources.tessellation_control
#if !defined(NDEBUG)
                                                                                    ,
                                                                                    "Tessellation Control",
                                                                                    _uniforms_names
#endif
    );
    auto const tess_eval = gen_shader_module<glpp::ShaderType::TessellationEvaluation>(sources.tessellation_evaluation
#if !defined(NDEBUG)
                                                                                       ,
                                                                                       "Tessellation Evaluation",
                                                                                       _uniforms_names
#endif
    );
    if (vert)
        _program.attach_shader(**vert);
    if (frag)
        _program.attach_shader(**frag);
    if (geom)
        _program.attach_shader(**geom);
    if (tess_ctrl)
        _program.attach_shader(**tess_ctrl);
    if (tess_eval)
        _program.attach_shader(**tess_eval);
    _program.link();
#if !defined(NDEBUG)
    {
        const auto err = _program.check_linking_errors();
        if (err)
        {
            const auto msg = "Shader linking failed:\n" + err.message();
            std::cerr << msg << '\n';
            throw std::runtime_error{msg};
        }
    }
#endif
}

void Shader::check_for_errors_before_rendering() const
{
#if !defined(NDEBUG)
    const auto err = _program.check_for_state_errors();
    if (err)
    {
        const auto msg = "Shader is not ready for rendering:\n" + err.message();
        std::cerr << msg << '\n';
        throw std::runtime_error{msg};
    }
#endif
}

void Shader::use() const
{
    _program.use();
}

template<typename T>
static void set_uniform(const glpp::ext::Program& program, std::string_view uniform_name, T&& value
#if !defined(NDEBUG)
                        ,
                        std::set<std::string> const& uniforms_names
#endif
)
{
#if !defined(NDEBUG)
    assert(uniforms_names.find(std::string{uniform_name}) != uniforms_names.end()
           && "This uniform name does not exist in the shader.");
#endif
    program.use();
    program.set(std::string{uniform_name}, value);
}
void Shader::set(std::string_view uniform_name, int value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, unsigned int value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, bool value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, float value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, const glm::vec2& value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, const glm::vec3& value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, const glm::vec4& value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, const glm::mat2& value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, const glm::mat3& value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, const glm::mat4& value) const
{
    set_uniform(_program, uniform_name, value
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
}
void Shader::set(std::string_view uniform_name, const ImageOrCanvas& image) const
{
    image.texture().bind_to_texture_unit(s_available_texture_slot);
    set_uniform(_program, uniform_name, s_available_texture_slot
#if !defined(NDEBUG)
                ,
                _uniforms_names
#endif
    );
    s_available_texture_slot = (s_available_texture_slot + 1) % 8;
}

static auto file_content(std::filesystem::path const& path) -> std::string
{
    auto ifs = std::ifstream{make_absolute_path(path)};
    return std::string{std::istreambuf_iterator<char>{ifs}, {}};
}

static auto maybe_file_content(std::optional<std::filesystem::path> const& path) -> std::optional<std::string>
{
    if (!path)
        return std::nullopt;

    return file_content(*path);
}

Shader load_shader(std::filesystem::path const& fragment_shader_path)
{
    return Shader{file_content(fragment_shader_path)};
}

Shader load_shader(std::filesystem::path const& vertex_shader_path, std::filesystem::path const& fragment_shader_path)
{
    return Shader{file_content(vertex_shader_path),
                  file_content(fragment_shader_path)};
}

Shader load_shader(ShaderPaths const& paths)
{
    return Shader{{
        maybe_file_content(paths.vertex),
        maybe_file_content(paths.fragment),
        maybe_file_content(paths.geometry),
        maybe_file_content(paths.tessellation_control),
        maybe_file_content(paths.tessellation_evaluation),
    }};
}

namespace internal {

static glm::vec2 get_scale(const glm::mat3& transform)
{
    // Get the length of the first two columns of the 2x2 sub-matrix
    return glm::vec2{
        glm::length(glm::vec2{transform[0][0], transform[0][1]}),
        glm::length(glm::vec2{transform[1][0], transform[1][1]}),
    };
}

void set_vertex_shader_uniforms(Shader const& shader, glm::mat3 const& transform, float framebuffer_aspect_ratio)
{
    glm::vec2 const scale = get_scale(transform);
    shader.set("_window_inverse_aspect_ratio", 1.0f / framebuffer_aspect_ratio);
    shader.set("_transform", transform);
    shader.set("_size", scale);
    if (scale.x == 0.f || scale.y == 0.f) // Avoid crash when aspect ratio implies a division by 0
        return;
    shader.set("_aspect_ratio", scale.x / scale.y);
}

} // namespace internal

} // namespace p6
