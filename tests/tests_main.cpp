#define CATCH_CONFIG_MAIN
#include "../src/kana_conv.h"
#include "../src/util.h"
#include <catch2/catch.hpp>

using SVec = std::vector<std::string_view>;
using namespace std::string_view_literals;

TEST_CASE("Hiragana to katakana", "[hiragana_to_katakana]")
{
  REQUIRE(hiragana_to_katakana("あいうえお") == "アイウエオ");
  REQUIRE(hiragana_to_katakana("お前はもう死んでいる。") == "オ前ハモウ死ンデイル。");
  REQUIRE(katakana_to_hiragana("イマリ") == "いまり");
  REQUIRE(katakana_to_hiragana("ニャ〜") == "にゃ〜");
  REQUIRE(
    katakana_to_hiragana("<div>オープンソース形態素解析エンジンです。Test 😀")
    == "<div>おーぷんそーす形態素解析えんじんです。Test 😀"
  );
}

TEST_CASE("Trim string", "[strtrim]")
{
  REQUIRE(strtrim("  あいうえお ") == "あいうえお");
}

TEST_CASE("Iterate chars", "[iterate]")
{
  auto const test_case = "あいうえおabcdeабвгд"sv;
  auto const ref_vec = SVec{ "あ", "い", "う", "え", "お", "a", "b", "c", "d", "e", "а", "б", "в", "г", "д" };
  auto test_vec = SVec{};
  std::ranges::copy(iter_unicode_chars(test_case), std::back_inserter(test_vec));

  REQUIRE(test_vec == ref_vec);
}
