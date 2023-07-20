/*
 *  gd-tools - a set of programs to enhance goldendict for immersion learning.
 *  Copyright (C) 2023 Ajatt-Tools
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "kana_conv.h"
#include "precompiled.h"
#include "util.h"

auto unicode_char_byte_len(auto const& ch) -> CharByteLen
{
  // See `man charsets` -> Unicode for explanation of below
  if ((ch & 0xC0) == 0x80) {
    // intermediate unicode char
    return CharByteLen::SKIP;
  } else if ((ch & 0x80) == 0x00) {
    // Start of ASCII character
    return CharByteLen::ONE;
  } else if ((ch & 0xE0) == 0xC0) {
    // Cyrillic, etc.
    return CharByteLen::TWO;
  } else if ((ch & 0xF0) == 0xE0) {
    // Start of 3 byte sequence (CJK)
    return CharByteLen::THREE;
  } else if ((ch & 0xF8) == 0xF0) {
    // Other Unicode
    return CharByteLen::FOUR;
  }
  throw runtime_error{ fmt::format("Can't recognize byte: '{:x}'.", ch) };
}

auto create_map(std::string_view from, std::string_view to) -> std::map<std::string_view, std::string_view>
{
  std::map<std::string_view, std::string_view> result{};
  for (auto const [idx, uni_char]: iter_unicode_chars(from)) {
    result.emplace(uni_char, to.substr(idx, uni_char.length()));
  }
  return result;
}
