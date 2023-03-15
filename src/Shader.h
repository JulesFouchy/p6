#pragma once

#include <filesystem>
#include <glpp/extended.hpp>
#include <string_view>
#include "ImageCommon.h"
#include "Transform2D.h"

namespace p6 {

struct ShaderPaths {
    std::optional<std::filesystem::path> vertex{};
    std::optional<std::filesystem::path> fragment{};
    std::optional<std::filesystem::path> geometry{};
    std::optional<std::filesystem::path> tessellation_control{};
    std::optional<std::filesystem::path> tessellation_evaluation{};
};

struct ShaderSources {
    std::optional<std::string> vertex{};
    std::optional<std::string> fragment{};
    std::optional<std::string> geometry{};
    std::optional<std::string> tessellation_control{};
    std::optional<std::string> tessellation_evaluation{};
};

class Shader {
public:
    /// Throws std::runtime_error if there is an error while compiling the shader source code
    explicit Shader(std::string_view fragment_source_code);
    /// Throws std::runtime_error if there is an error while compiling the shader source code
    Shader(std::string_view vertex_source_code, std::string_view fragment_source_code);
    /// Throws std::runtime_error if there is an error while compiling the shader source code
    explicit Shader(ShaderSources const& sources);

    void set(std::string_view uniform_name, int value) const;
    void set(std::string_view uniform_name, unsigned int value) const;
    void set(std::string_view uniform_name, bool value) const;
    void set(std::string_view uniform_name, float value) const;
    void set(std::string_view uniform_name, const glm::vec2& value) const;
    void set(std::string_view uniform_name, const glm::vec3& value) const;
    void set(std::string_view uniform_name, const glm::vec4& value) const;
    void set(std::string_view uniform_name, const glm::mat2& value) const;
    void set(std::string_view uniform_name, const glm::mat3& value) const;
    void set(std::string_view uniform_name, const glm::mat4& value) const;
    /// :warning: You can have at most 8 images set at once. This is a limitation of the GPUs.
    void set(std::string_view uniform_name, const ImageOrCanvas& image) const;

    /// Sets this as the current shader that will be used for rendering.
    void use() const;

    /// Returns the OpenGL id of the program.
    /// This is for advanced uses only.
    GLuint id() const { return *_program; }

    /// You can call this just before a draw call (e.g. `glDrawArrays`) to check if your shader is set up properly (all textures have been set, etc.)
    void check_for_errors_before_rendering() const;

private:
    glpp::ext::Program _program;
    static GLenum      s_available_texture_slot;
};

/// Loads a Shader from a file containing the fragment shader's source code.
/// If the path is relative, it will be relative to the directory containing your executable.
/// Throws std::runtime_error if there is an error while compiling the shader source code.
[[nodiscard]] Shader load_shader(std::filesystem::path const& fragment_shader_path);

/// Loads a Shader from two files containing the vertex and fragment shader's source code.
/// If the path is relative, it will be relative to the directory containing your executable.
/// Throws std::runtime_error if there is an error while compiling the shader source code.
[[nodiscard]] Shader load_shader(std::filesystem::path const& vertex_shader_path, std::filesystem::path const& fragment_shader_path);

/// Loads a Shader from two files containing the vertex and fragment shader's source code.
/// If the path is relative, it will be relative to the directory containing your executable.
/// Throws std::runtime_error if there is an error while compiling the shader source code.
[[nodiscard]] Shader load_shader(ShaderPaths const& paths);

namespace internal {
/// Set all needed uniforms for the p6 default vertex shader.
void set_vertex_shader_uniforms(Shader const& shader, glm::mat3 const& transform, float framebuffer_aspect_ratio);

} // namespace internal

} // namespace p6
