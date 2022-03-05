#include "TextRenderer.h"
#include <algorithm>
#include <glpp/glpp.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../NamedColor.h"
#include "../Shader.h"
#include "../Transform2D.h"
#include "../math.h"

namespace p6::details {

// "⏮⏪⏴⏺⏹⏵⏸⏩⏭♩♪♫♬♭♮♯←↑→↓↔↕↖↗↘↙↺↻★☻🕨🕪!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ αβγδεθλμξπρστφψωΓΔΘΛΠΣΦΨΩ∞ƒ∘∫∂∇√¡¢£¤¥¦§¨©ª«¬ ®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";

#pragma warning(disable : 4244)
// clang-format off
static const std::map<char16_t, unsigned char> char_correspondance = {
    {'⏮',0},{'⏪',1},{'⏴',2},{'⏺',3},{'⏹',4},{'⏵',5},{'⏸',6},{'⏩',7},{'⏭',8},{'♩',9},{'♪',10},{'♫',11},{'♬',12},{'♭',13},{'♮',14},{'♯',15},
    {'←',16},{'↑',17},{'→',18},{'↓',19},{'↔',20},{'↕',21},{'↖',22},{'↗',23},{'↘',24},{'↙',25},{'↺',26},{'↻',27},{'★',28},{'☻',29},{'🕨',30},{'🕪',31},
    {'!',32},{'\\',33},{'"',34},{'#',35},{'$',36},{'%',37},{'&',38},{'\'',39},{'(',40},{')',41},{'*',42},{'+',43},{',',44},{'-',45},{'.',46},{'/',47},
    {'0',48},{'1',49},{'2',50},{'3',51},{'4',52},{'5',53},{'6',54},{'7',55},{'8',56},{'9',57},{':',58},{';',59},{'<',60},{'=',61},{'>',62},{'?',63},
    {'@',64},{'A',65},{'B',66},{'C',67},{'D',68},{'E',69},{'F',70},{'G',71},{'H',72},{'I',73},{'J',74},{'K',75},{'L',76},{'M',77},{'N',78},{'O',79},
    {'P',80},{'Q',81},{'R',82},{'S',83},{'T',84},{'U',85},{'V',86},{'W',87},{'X',88},{'Y',89},{'Z',90},{'[',91},{'\\',92},{']',93},{'^',94},{'_',95},
    {'`',96},{'a',97},{'b',98},{'c',99},{'d',100},{'e',101},{'f',102},{'g',103},{'h',104},{'i',105},{'j',106},{'k',107},{'l',108},{'m',109},{'n',110},{'o',111},
    {'p',112},{'q',113},{'r',114},{'s',115},{'t',116},{'u',117},{'v',118},{'w',119},{'x',120},{'y',121},{'z',122},{'{',123},{'|',124},{'}',125},{'~',126},{' ',127},
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
    : _font_image{load_image("ressources/otaviogoodFontMap.png")}
    , _gpu_text_buffer{glpp::Interpolation::NearestNeighbour,
                       glpp::Interpolation::NearestNeighbour}
{
}

static void convert_and_copy_text_to_buffer(const std::u16string& text, TextRenderer::ArrayOfChar& cpu_buffer)
{
    std::transform(text.begin(), text.end(), cpu_buffer.begin(),
                   [](char16_t c) -> unsigned char {
                       auto search = char_correspondance.find(c);
                       return search != char_correspondance.end()
                                  ? search->second
                                  : static_cast<unsigned char>(63); // for '?';
                   });
}

static void send_text_buffer_to_gpu(glpp::Texture1D& gpu_buffer, const TextRenderer::ArrayOfChar& cpu_buffer, size_t actual_buffer_size)
{
    gpu_buffer.upload_data(
        static_cast<GLsizei>(actual_buffer_size),
        cpu_buffer.data(),
        {glpp::InternalFormat::R8UI, glpp::Channels::R_Integer, glpp::TexelDataType::UnsignedByte});
}

void TextRenderer::setup_rendering_for(const std::u16string& text, TextParams params)
{
    if (text.length() > 1024) { // TODO do we need to handle arbitrarily long text?
        throw std::runtime_error("[p6::TextRenderer] This text is too long to be rendered");
    }

    convert_and_copy_text_to_buffer(text, _cpu_text_buffer);
    send_text_buffer_to_gpu(_gpu_text_buffer, _cpu_text_buffer, text.length());

    _font_image.texture().bind_to_texture_unit(0);
    _gpu_text_buffer.bind_to_texture_unit(1);

    _shader.set("_font_image", 0);
    _shader.set("_text_buffer", 1);
    _shader.set("_sentence_size", static_cast<int>(text.length()));
    _shader.set("_inflating", params.inflating);
    _shader.set("_color", params.color.as_premultiplied_vec4());
}

namespace TextRendererU {

Radii compute_text_radii(const std::u16string& text, float font_size)
{
    return font_size * glm::vec2{text.length(), 1.f};
}

} // namespace TextRendererU

} // namespace p6::details