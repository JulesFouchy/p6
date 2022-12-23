import urllib.request
import json
import unicodedata


def format_hex(s: str):
    s = s.replace("#", "").upper()
    if len(s) == 3:
        s = s[0] + s[0] + s[1] + s[1] + s[2] + s[2]
    assert(len(s) == 6)
    return s


def format_name(s: str):
    return unicodedata.normalize('NFKD', s
                                 .replace(" ", "")
                                 .replace("(", "")
                                 .replace(")", "")
                                 .replace("-", "")
                                 .replace("/", "")
                                 .replace("'", "")
                                 .replace("#", "")
                                 .replace("&", "")
                                 .replace(".", "")
    ).encode('ascii', 'ignore').decode('ascii')

# Load colors data
colors_json = json.loads(urllib.request.urlopen(
                "https://raw.githubusercontent.com/mralexgray/color-names/json/output/colors.json"
              ).read())
# Format name and hex
for color in colors_json.values():
    color['name'] = format_name(color['name'])
    color['hex'] = format_hex(color['hex'])

# Add spaces to align all the colors values
max_length = max(map(lambda color: len(color['name']), colors_json.values()))
for color in colors_json.values():
    color['name'] += ' ' * (max_length - len(color['name']))

# Generate colors list
colors_list = ""
for color in colors_json.values():
    colors_list += f"static constexpr Color {color['name']} = hex(0x{color['hex']});\n"

# Generate final C++ code
cpp_code = """// This file was auto-generated by generate_named_colors.py
// Please don't edit the file directly, go to the script!

#pragma once
#include "Color.h"

namespace p6::NamedColor {

%s
} // namespace p6::NamedColor
""" % colors_list

# Write to file
with open('src/NamedColor.h', 'w') as f:
    f.write(cpp_code)