#include "kana_conv.h"
#include "mecab_split.h"
#include "util.h"
#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("half to full", "[convert]")
{
  std::string from =
    "ｱ ｲ ｳ ｴ ｵ ｶ ｷ ｸ ｹ ｺ ｻ ｼ ｽ ｾ ｿ ﾀ ﾁ ﾂ ﾃ ﾄ ﾅ ﾆ ﾇ ﾈ ﾉ ﾊ ﾋ ﾌ ﾍ ﾎ ﾏ ﾐ ﾑ ﾒ ﾓ ﾔ ﾕ ﾖ ﾗ ﾘ ﾙ ﾚ ﾛ ﾜ ｦ ﾝ ｧ ｨ ｩ ｪ ｫ ｯ ｬ ｭ ｮ";
  std::string const to =
    "ア イ ウ エ オ カ キ ク ケ コ サ シ ス セ ソ タ チ ツ テ ト ナ ニ ヌ ネ ノ ハ ヒ フ ヘ ホ マ ミ ム メ モ ヤ ユ ヨ "
    "ラ リ ル レ ロ ワ ヲ ン ァ ィ ゥ ェ ォ ッ ャ ュ ョ";
  half_to_full(from);
  REQUIRE(from == to);
}

TEST_CASE("JpSet", "[JpSet]")
{
  JpSet set = { "キサマ", "きさま" };
  REQUIRE(set.size() == 1);
}

TEST_CASE("replace_all", "[replace_all]")
{
  std::string test = "私私私　家　出ようと思うんだ。";
  test = replace_all(test, "思う", "omou");
  test = replace_all(test, "私私", "");
  REQUIRE(test == "私　家　出ようとomouんだ。");
}
