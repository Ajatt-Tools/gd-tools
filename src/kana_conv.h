#pragma once

#include "precompiled.h"

using KanaConvMap = std::unordered_map<std::string_view, std::string_view>;

enum CharByteLen : std::size_t { SKIP = 0, ONE = 1, TWO = 2, THREE = 3, FOUR = 4 };
enum class Direction { kata_to_hira, hira_to_kata };

auto create_map(std::string_view from, std::string_view to) -> KanaConvMap;
auto unicode_char_byte_len(char const& ch) -> CharByteLen;

inline constexpr std::string_view hiragana_chars =
  "ぁあぃいぅうぇえぉおかがか゚きぎき゚くぐく゚けげけ゚こごこ゚さざしじすずせぜそぞただちぢっつづてでとどなにぬねのはばぱひびぴふ"
  "ぶぷへべぺほぼぽまみむめもゃやゅゆょよらりるれろゎわゐゑをんゔゕゖゝゞ";
inline constexpr std::string_view katakana_chars =
  "ァアィイゥウェエォオカガカ゚キギキ゚クグク゚ケゲケ゚コゴコ゚サザシジスズセゼソゾタダチヂッツヅテデトドナニヌネノハバパヒビピフ"
  "ブプヘベペホボポマミムメモャヤュユョヨラリルレロヮワヰヱヲンヴヵヶヽヾ";

struct Utf8CharView
{
  std::size_t idx; // starting position of a char in string, counting by bytes
  std::string_view ch; // the whole character, e.g. "あ" (len=3)
};

struct Utf8IdxLen
{
  std::size_t idx; // starting position of a char in string, counting by bytes
  std::size_t len; // length of the utf8 character in bytes.

  static Utf8IdxLen from_tuple(std::tuple<std::size_t, char const> enumerated)
  {
    return Utf8IdxLen(
      std::get<0>(enumerated), //
      unicode_char_byte_len(std::get<1>(enumerated))
    );
  }
};

constexpr auto enum_unicode_chars(std::string_view str)
{
  // return a sequence of Utf8CharView.
  // e.g. [ (0, "あ"), (3, "い") ]
  return std::views::enumerate(str) //
         | std::views::transform(Utf8IdxLen::from_tuple)
         | std::views::filter([](Utf8IdxLen const ch) { return (ch.len != CharByteLen::SKIP); })
         | std::views::transform([str](Utf8IdxLen const ch) {
             assert(ch.len > 0);
             return Utf8CharView(ch.idx, str.substr(ch.idx, ch.len));
           });
}

constexpr auto iter_unicode_chars(std::string_view str)
{
  return enum_unicode_chars(str) | std::views::transform([](Utf8CharView const v) { return v.ch; });
}

template<Direction D>
auto convert_kana(std::string_view str) -> std::string
{
  static auto const conv_map{ [] {
    if constexpr (D == Direction::kata_to_hira) {
      return create_map(katakana_chars, hiragana_chars);
    } else {
      return create_map(hiragana_chars, katakana_chars);
    }
  }() };

  std::string result{};
  result.reserve(str.length());
  for (auto const [idx, uni_char]: enum_unicode_chars(str)) {
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

auto half_to_full(std::string& str) -> std::string&;

struct KanaInsensitiveMore
{
  bool operator()(std::string const& lhs, std::string const& rhs) const
  {
    if (lhs.length() != rhs.length()) {
      return lhs.length() > rhs.length();
    } else {
      return hiragana_to_katakana(lhs) > hiragana_to_katakana(rhs);
    }
  }
};

using JpSet = std::set<std::string, KanaInsensitiveMore>;
