#pragma once

#include "precompiled.h"

enum CharByteLen : std::size_t { SKIP = 0, ONE = 1, TWO = 2, THREE = 3, FOUR = 4 };
enum class Direction { kata_to_hira, hira_to_kata };

auto create_map(std::string_view from, std::string_view to) -> std::map<std::string_view, std::string_view>;
auto unicode_char_byte_len(auto const& ch) -> CharByteLen;

inline constexpr std::string_view hiragana_chars =
  "ぁあぃいぅうぇえぉおかがか゚きぎき゚くぐく゚けげけ゚こごこ゚さざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふ"
  "ぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖゝゞ";
inline constexpr std::string_view katakana_chars =
  "ァアィイゥウェエォオカガカ゚キギキ゚クグク゚ケゲケ゚コゴコ゚サザシジスズセゼソゾタダチヂッツヅテデトドナニヌネノハバパヒビピフ"
  "ブプヘベペホボポマミムメモャヤュユョヨラリルレロヮワヰヱヲンヴヵヶヽヾ";

inline auto iter_unicode_chars(std::string_view str)
{
  return std::views::enumerate(str) //
         | std::views::transform([](auto const idx_chr) {
             return std::make_pair(std::get<0>(idx_chr), unicode_char_byte_len(std::get<1>(idx_chr)));
           })
         | std::views::filter([](auto const idx_len) { return idx_len.second != CharByteLen::SKIP; })
         | std::views::transform([str](auto const idx_len) {
             assert(idx_len.second > 0);
             return std::make_pair(idx_len.first, str.substr(idx_len.first, idx_len.second));
           });
}

template<Direction D>
auto convert_kana(std::string_view str) -> std::string
{
  static const auto conv_map{ [] {
    if constexpr (D == Direction::kata_to_hira) {
      return create_map(katakana_chars, hiragana_chars);
    } else {
      return create_map(hiragana_chars, katakana_chars);
    }
  }() };

  std::string result{};
  result.reserve(str.length());
  for (auto const [idx, uni_char]: iter_unicode_chars(str)) {
    result.append(conv_map.contains(uni_char) ? conv_map.at(uni_char) : uni_char);
  }
  return result;
}

inline auto katakana_to_hiragana(std::string_view str) -> std::string
{
  return convert_kana<Direction::kata_to_hira>(str);
}

inline auto hiragana_to_katakana(std::string_view str) -> std::string
{
  return convert_kana<Direction::hira_to_kata>(str);
}
