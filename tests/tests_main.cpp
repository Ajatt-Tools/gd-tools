#define CATCH_CONFIG_MAIN
#include "../src/kana_conv.h"
#include "../src/util.h"
#include <catch2/catch.hpp>

TEST_CASE("Hiragana to katakana", "[hiragana_to_katakana]")
{
  REQUIRE(hiragana_to_katakana("あいうえお") == "アイウエオ");
  REQUIRE(hiragana_to_katakana("お前はもう死んでいる。") == "オ前ハモウ死ンデイル。");
  REQUIRE(katakana_to_hiragana("イマリ") == "いまり");
  REQUIRE(katakana_to_hiragana("ニャ〜") == "にゃ〜");
  REQUIRE(katakana_to_hiragana("<div>オープンソース形態素解析エンジンです。Test 😀") == "<div>おーぷんそーす形態素解析えんじんです。Test 😀");
}

TEST_CASE("Trim string", "[strtrim]")
{
  REQUIRE(strtrim("  あいうえお ") == "あいうえお");
}
