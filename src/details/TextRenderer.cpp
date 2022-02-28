#include "TextRenderer.h"
#include <glpp/Functions/Texture.h>
#include <glpp/check_errors.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../NamedColor.h"
#include "../Shader.h"
#include "../Transform2D.h"
#include "../math.h"

namespace p6 {
namespace details {

// "⏮⏪⏴⏺⏹⏵⏸⏩⏭♩♪♫♬♭♮♯←↑→↓↔↕↖↗↘↙↺↻★☻🕨🕪!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ αβγδεθλμξπρστφψωΓΔΘΛΠΣΦΨΩ∞ƒ∘∫∂∇√¡¢£¤¥¦§¨©ª«¬ ®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";

#pragma warning(disable : 4244)
// clang-format off
const std::map<char16_t, unsigned char> TextRenderer::char_correspondance = {
    {'⏮',0},{'⏪',1},{'⏴',2},{'⏺',3},{'⏹',4},{'⏵',5},{'⏸',6},{'⏩',7},{'⏭',8},{'♩',9},{'♪',10},{'♫',11},{'♬',12},{'♭',13},{'♮',14},{'♯',15},
    {'←',16},{'↑',17},{'→',18},{'↓',19},{'↔',20},{'↕',21},{'↖',22},{'↗',23},{'↘',24},{'↙',25},{'↺',26},{'↻',27},{'★',28},{'☻',29},{'🕨',30},{'🕪',31},
    {'!',32},{'\\',33},{'"',34},{'#',35},{'$',36},{'%',37},{'&',38},{'\'',39},{'(',40},{')',41},{'*',42},{'+',43},{',',44},{'-',45},{'.',46},{'/',47},
    {'0',48},{'1',49},{'2',50},{'3',51},{'4',52},{'5',53},{'6',54},{'7',55},{'8',56},{'9',57},{':',58},{';',59},{'<',60},{'=',61},{'>',62},{'?',63},
    {'@',64},{'A',65},{'B',66},{'C',67},{'D',68},{'E',69},{'F',70},{'G',71},{'H',72},{'I',73},{'J',74},{'K',75},{'L',76},{'M',77},{'N',78},{'O',79},
    {'P',80},{'Q',81},{'R',82},{'S',83},{'T',84},{'U',85},{'V',86},{'W',87},{'X',88},{'Y',89},{'Z',90},{'[',91},{'\\',92},{']',93},{'^',94},{'_',95},
    {'`',96},{'a',97},{'b',98},{'c',99},{'d',100},{'e',101},{'f',102},{'g',103},{'h',104},{'i',105},{'j',106},{'k',107},{'l',108},{'m',109},{'n',110},{'o',112},
    {'p',111},{'q',113},{'r',114},{'s',115},{'t',116},{'u',117},{'v',118},{'w',119},{'x',120},{'y',121},{'z',122},{'{',123},{'|',124},{'}',125},{'~',126},{' ',127},
    {'α',128},{'β',129},{'γ',130},{'δ',131},{'ε',132},{'θ',133},{'λ',134},{'μ',135},{'ξ',136},{'π',137},{'ρ',138},{'σ',139},{'τ',140},{'φ',141},{'ψ',142},{'ω',143},
    {'Γ',144},{'Δ',145},{'Θ',146},{'Λ',147},{'Π',148},{'Σ',150},{'Φ',149},{'Ψ',151},{'Ω',152},{'∞',153},{'ƒ',154},{'∘',155},{'∫',156},{'∂',157},{'∇',158},{'√',159},
    {'¡',160},{'¢',161},{'£',162},{'¤',163},{'¥',164},{'¦',165},{'§',166},{'¨',167},{'©',168},{'ª',169},{'«',171},{'¬',170},{' ',172},{'®',173},{'¯',174},{'°',175},
    {'±',176},{'²',177},{'³',178},{'´',179},{'µ',180},{'¶',181},{'·',183},{'¸',182},{'¹',184},{'º',185},{'»',186},{'¼',187},{'½',188},{'¾',189},{'¿',190},{'À',191},
    {'Á',192},{'Â',193},{'Ã',195},{'Ä',194},{'Å',196},{'Æ',197},{'Ç',198},{'È',199},{'É',200},{'Ê',201},{'Ë',202},{'Ì',203},{'Í',204},{'Î',205},{'Ï',206},{'Ð',207},
    {'Ñ',208},{'Ò',209},{'Ó',210},{'Ô',211},{'Õ',212},{'Ö',213},{'×',214},{'Ø',215},{'Ù',216},{'Ú',217},{'Û',218},{'Ü',219},{'Ý',220},{'Þ',221},{'ß',222},{'à',223},
    {'á',224},{'â',225},{'ã',226},{'ä',227},{'å',228},{'æ',229},{'ç',230},{'è',231},{'é',232},{'ê',233},{'ë',234},{'ì',235},{'í',236},{'î',237},{'ï',238},{'ð',239},
    {'ñ',240},{'ò',241},{'ó',242},{'ô',243},{'õ',244},{'ö',245},{'÷',246},{'ø',247},{'ù',248},{'ú',249},{'û',250},{'ü',251},{'ý',252},{'þ',253},{'ÿ',254}};
// clang-format on

TextRenderer::TextRenderer()
    : _fontImage(load_image("ressources/otaviogoodFontMap.png"))
    , _textBuffer(glpp::Interpolation::NearestNeighbour,
                  glpp::Interpolation::NearestNeighbour)
{
}

void TextRenderer::Update_buffer_from_str(const std::u16string& str)
{
    std::transform(str.begin(), str.end(), _buffer.begin(),
                   [this](char16_t c) -> unsigned char {
                       auto search = char_correspondance.find(c);
                       return search != char_correspondance.end() ? search->second : static_cast<unsigned char>(63); // for '?';
                   });
}

void TextRenderer::Update_data(const std::u16string& str)
{
    Update_buffer_from_str(str);

    _textBuffer.upload_data(
        static_cast<GLsizei>(Compute_sentence_size(str)),
        &_buffer[0],
        {glpp::InternalFormat::R8UI, glpp::Channels::R_Integer, glpp::TexelDataType::UnsignedByte});
};

size_t TextRenderer::Compute_sentence_size(const std::u16string& str)
{
    if (str.length() > 1024)
        throw new std::runtime_error("[p6::TextRenderer] string to long to be printed.");
    return str.length();
}

void TextRenderer::render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, float inflating, Transform2D transform, Color color)
{
    Update_data(str);

    _fontImage.texture().bind_to_texture_unit(0);
    _textBuffer.bind_to_texture_unit(1);

    _shader.set("_fontImage", 0);
    _shader.set("_textBuffer", 1);
    p6::internal::set_vertex_shader_uniforms(_shader, transform, aspect_ratio);
    _shader.set("_sentence_size", static_cast<int>(Compute_sentence_size(str)));
    _shader.set("_inflating", inflating);
    _shader.set("_color", color.as_premultiplied_vec3());

    rectRenderer.render();
}

void TextRenderer::render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, float font_size, float inflating, Center center, Rotation rotation, Color color)
{
    Radii radii{font_size * Compute_sentence_size(str), font_size};
    render(rectRenderer, str, aspect_ratio, inflating, p6::make_transform_2D(center, radii, rotation), color);
}

void TextRenderer::render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, const float font_size, const float inflating, TopLeftCorner corner, Rotation rotation, Color color)
{
    Radii radii{font_size * Compute_sentence_size(str), font_size};
    render(rectRenderer, str, aspect_ratio, font_size, inflating, internal::compute_new_center({1, -1}, corner.value, radii, rotation), rotation, color);
}

void TextRenderer::render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, const float font_size, const float inflating, TopRightCorner corner, Rotation rotation, Color color)
{
    Radii radii{font_size * Compute_sentence_size(str), font_size};
    render(rectRenderer, str, aspect_ratio, font_size, inflating, internal::compute_new_center({-1, -1}, corner.value, radii, rotation), rotation, color);
}

void TextRenderer::render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, const float font_size, const float inflating, BottomLeftCorner corner, Rotation rotation, Color color)
{
    Radii radii{font_size * Compute_sentence_size(str), font_size};
    render(rectRenderer, str, aspect_ratio, font_size, inflating, internal::compute_new_center({1, 1}, corner.value, radii, rotation), rotation, color);
}

void TextRenderer::render(const RectRenderer& rectRenderer, const std::u16string& str, float aspect_ratio, const float font_size, const float inflating, BottomRightCorner corner, Rotation rotation, Color color)
{
    Radii radii{font_size * Compute_sentence_size(str), font_size};
    render(rectRenderer, str, aspect_ratio, font_size, inflating, internal::compute_new_center({-1, 1}, corner.value, radii, rotation), rotation, color);
}

namespace internal {
Center compute_new_center(glm::vec2 offset_to_center, glm::vec2 corner_position, Radii radii, Rotation rotation)
{
    return {corner_position + p6::rotated_by(rotation, radii.value * offset_to_center)};
}
} // namespace internal

} // namespace details
} // namespace p6