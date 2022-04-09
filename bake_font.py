
with open("src/details/font_atlas",'wb') as result_file:
  result_file.write(b'#pragma once\nstatic constexpr unsigned char font_atlas[] = {')
  for b in open("font_atlas.png", 'rb').read():
    result_file.write(b'0x%02X,' % b)
  result_file.write(b'};')