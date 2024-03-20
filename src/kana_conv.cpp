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

auto unicode_char_byte_len(char const& ch) -> CharByteLen
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
  throw gd::runtime_error{ std::format("Can't recognize byte: '{:x}'.", ch) };
}

auto create_map(std::string_view from, std::string_view to) -> KanaConvMap
{
  KanaConvMap result{};
  for (auto const [idx, uni_char]: enum_unicode_chars(from)) {
    result.emplace(uni_char, to.substr(idx, uni_char.length()));
  }
  return result;
}

auto half_to_full(std::string& str) -> std::string&
{
  static KanaConvMap const conv_map = {
    { "ｱ", "ア" }, { "ｲ", "イ" }, { "ｳ", "ウ" }, { "ｴ", "エ" }, { "ｵ", "オ" }, { "ｶ", "カ" }, { "ｷ", "キ" },
    { "ｸ", "ク" }, { "ｹ", "ケ" }, { "ｺ", "コ" }, { "ｻ", "サ" }, { "ｼ", "シ" }, { "ｽ", "ス" }, { "ｾ", "セ" },
    { "ｿ", "ソ" }, { "ﾀ", "タ" }, { "ﾁ", "チ" }, { "ﾂ", "ツ" }, { "ﾃ", "テ" }, { "ﾄ", "ト" }, { "ﾅ", "ナ" },
    { "ﾆ", "ニ" }, { "ﾇ", "ヌ" }, { "ﾈ", "ネ" }, { "ﾉ", "ノ" }, { "ﾊ", "ハ" }, { "ﾋ", "ヒ" }, { "ﾌ", "フ" },
    { "ﾍ", "ヘ" }, { "ﾎ", "ホ" }, { "ﾏ", "マ" }, { "ﾐ", "ミ" }, { "ﾑ", "ム" }, { "ﾒ", "メ" }, { "ﾓ", "モ" },
    { "ﾔ", "ヤ" }, { "ﾕ", "ユ" }, { "ﾖ", "ヨ" }, { "ﾗ", "ラ" }, { "ﾘ", "リ" }, { "ﾙ", "ル" }, { "ﾚ", "レ" },
    { "ﾛ", "ロ" }, { "ﾜ", "ワ" }, { "ｦ", "ヲ" }, { "ﾝ", "ン" }, { "ｧ", "ァ" }, { "ｨ", "ィ" }, { "ｩ", "ゥ" },
    { "ｪ", "ェ" }, { "ｫ", "ォ" }, { "ｯ", "ッ" }, { "ｬ", "ャ" }, { "ｭ", "ュ" }, { "ｮ", "ョ" }, { "｡", "。" },
    { "､", "、" }, { "･", "・" }, { "゛", "ﾞ" }, { "゜", "ﾟ" }, { "｢", "「" }, { "｣", "」" }, { "ｰ", "ー" }
  };
  for (auto const [idx, uni_char]: enum_unicode_chars(str)) {
    if (conv_map.contains(uni_char)) {
      str.replace(idx, uni_char.length(), conv_map.at(uni_char));
    }
  }
  return str;
}
