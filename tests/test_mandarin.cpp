/*
 *  gd-tools - a set of programs to enhance goldendict for immersion learning.
 *  Copyright (C) 2023 Ajatt-Tools
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY, without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "anki_search.h"
#include "echo.h"
#include "helpers.h"
#include "images.h"
#include "mandarin_split.h"
#include "marisa_split.h"
#include "massif.h"
#include "mecab_html.h"
#include "mecab_split.h"
#include "split_html.h"
#include "translate.h"
#include "util.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE("Simple CSS font sizes", "[font-size]")
{
  auto const size = GENERATE("2rem", "30px", "1.25em", "100%", "12pt", "3vw", "4vh", "0px");
  REQUIRE_NOTHROW(validate_font_size(size));
}

TEST_CASE("Reject unsafe or unsupported CSS font sizes", "[font-size]")
{
  auto const size = GENERATE("", "30", "-1px", "large", "calc(1rem + 2px)", "2rem; color:red", "</style>", "2rem\n");
  REQUIRE_THROWS_AS(validate_font_size(size), gd::runtime_error);
}

TEST_CASE("Commands propagate application errors to the caller", "[commands]")
{
  auto const command = GENERATE(
    &search_anki_cards,
    &stroke_order,
    &images,
    &marisa_split,
    &massif,
    &mecab_split,
    &mandarin_split,
    &translate
  );
  std::vector<std::string_view> const args{ "--word" };
  REQUIRE_THROWS_AS(command(args), gd::runtime_error);
}

TEST_CASE("Mandarin requires its own complete dictionary", "[mandarin]")
{
  auto const path = GENERATE("res/no-such-mandarin-dictionary", "res");
  std::vector<std::string_view> const args{ "--dic-dir", path, "--word", "你好", "--sentence", "你好" };
  REQUIRE_THROWS_WITH(
    render_mandarin(args),
    Catch::Matchers::ContainsSubstring("Mandarin dictionary is not installed")
      and Catch::Matchers::ContainsSubstring("mandarin_installer.sh")
      and Catch::Matchers::ContainsSubstring("https://github.com/Ajatt-Tools/gd-tools#gd-mandarin")
  );
}

TEST_CASE("Mandarin renders traditional clickable words", "[mandarin]")
{
  auto const sentence = GENERATE("你好中国", "你好 中国");
  std::vector<std::string_view> const args{ "--dic-dir", "res/mandarin_dict", "--word", "你好", "--sentence",
                                            sentence,    "--font-size",       "30px" };
  auto const result = render_mandarin(args);
  REQUIRE(result.find(R"(<div class="gd-split" style="font-size: 30px">)") != std::string::npos);
  REQUIRE(
    split_html_words(result)
    == R"(<a href="bword:你好" title="你好"><b>你好</b></a><a href="bword:中國" title="中國">中國</a>)"
  );
  REQUIRE(result.find("font-size: 30px") != std::string::npos);
}

TEST_CASE("Mandarin removes whitespace without removing Latin text", "[mandarin]")
{
  auto const sentence = GENERATE("你 好 abc 中国", "你好\n中国", "你好 ABC123 中国");
  std::vector<std::string_view> const args{
    "--dic-dir", "res/mandarin_dict", "--word", "你好", "--sentence", sentence
  };
  std::string normalized_sentence = sentence;
  std::erase_if(normalized_sentence, is_space);
  std::vector<std::string_view> const normalized_args{ "--dic-dir", "res/mandarin_dict", "--word",
                                                       "你好",      "--sentence",        normalized_sentence };
  auto const result = render_mandarin(args);
  REQUIRE(result == render_mandarin(normalized_args));
  if (normalized_sentence.contains("abc")) {
    REQUIRE(result.find(R"(<a href="bword:abc" title="abc">abc</a>)") != std::string::npos);
  }
  if (normalized_sentence.contains("ABC123")) {
    REQUIRE(result.find(R"(<a href="bword:A" title="A">A</a>)") != std::string::npos);
    REQUIRE(result.find(R"(<a href="bword:123" title="123">123</a>)") != std::string::npos);
  }
}

TEST_CASE("Mandarin default font size", "[mandarin]")
{
  std::vector<std::string_view> const args{ "--dic-dir", "res/mandarin_dict", "--word", "你好", "--sentence", "你好" };
  REQUIRE(render_mandarin(args).find("font-size: 2rem") != std::string::npos);
}

TEST_CASE("Mandarin rejects invalid options without HTML output", "[mandarin]")
{
  auto const option = GENERATE("--font-size", "--user-dict", "--unknown");
  std::vector<std::string_view> const args{
    "--dic-dir", "res/mandarin_dict", "--word", "你好", "--sentence", "你好", option, "invalid"
  };
  REQUIRE_THROWS_AS(render_mandarin(args), gd::runtime_error);
}

TEST_CASE("Mandarin uses the word when the sentence is omitted", "[mandarin]")
{
  std::vector<std::string_view> const args{ "--dic-dir", "res/mandarin_dict", "--word", "你好" };
  std::vector<std::string_view> const explicit_args{ "--dic-dir", "res/mandarin_dict", "--word",
                                                     "你好",      "--sentence",        "你好" };
  REQUIRE(render_mandarin(args) == render_mandarin(explicit_args));
}

TEST_CASE("MeCab validates its configurable font size", "[mecab]")
{
  auto const size = GENERATE("2rem; color:red", R"(2rem"><script>)");
  std::vector<std::string_view> const args{ "--word", "猫", "--font-size", size };
  REQUIRE_THROWS_WITH(render_mecab(args), Catch::Matchers::ContainsSubstring("Invalid font size"));
}

TEST_CASE("Mandarin validates its configurable font size", "[mandarin]")
{
  auto const size = GENERATE("2rem; color:red", R"(2rem"><script>)");
  std::vector<std::string_view> const args{ "--dic-dir", "res/mandarin_dict", "--word", "你好", "--sentence",
                                            "你好",      "--font-size",       size };
  REQUIRE_THROWS_WITH(render_mandarin(args), Catch::Matchers::ContainsSubstring("Invalid font size"));
}

TEST_CASE("MeCab renders default and overridden font sizes", "[mecab]")
{
  if (find_dic_dir().empty()) {
    SKIP("Japanese dictionary is not installed.");
  }
  auto const size = GENERATE("", "2rem", "30px");
  std::vector<std::string_view> args{ "--word", "猫" };
  if (std::string_view(size) != "") {
    args.insert(args.end(), { "--font-size", size });
  }
  auto const result = render_mecab(args);
  REQUIRE(
    result.find(std::format("font-size: {}", std::string_view(size).empty() ? "2rem" : size)) != std::string::npos
  );
  REQUIRE(result.find("<!--\ndicdir: ") != std::string::npos);
  REQUIRE(result.find("\nuserdic: ") != std::string::npos);
  REQUIRE(result.find("\nmecab args: [arg0, --dicdir=") != std::string::npos);
}

TEST_CASE("Mandarin rendering signals help requests", "[mandarin]")
{
  auto const option = GENERATE("--help", "-h");
  std::vector<std::string_view> const args{ option };
  REQUIRE_THROWS_AS(render_mandarin(args), gd::help_requested);
}

TEST_CASE("Split HTML shares scoped CSS and keeps font sizes local", "[split-html]")
{
  auto const words = R"(<a href="bword:你好"><b>你好</b></a>)";
  auto const first = render_split_html(words, "2rem");
  auto const second = render_split_html(words, "30px");
  REQUIRE(first.starts_with("\n<style>"));
  REQUIRE(first.find("</style>") < first.find("<div"));
  REQUIRE(first.ends_with(std::format("{}{}", words, "</div>")));
  REQUIRE(first.find(R"(class="gd-split" style="font-size: 2rem")") != std::string::npos);
  REQUIRE(second.find(R"(class="gd-split" style="font-size: 30px")") != std::string::npos);
  auto const first_css = first.substr(0, first.find("</style>") + std::string_view("</style>").size());
  auto const second_css = second.substr(0, second.find("</style>") + std::string_view("</style>").size());
  REQUIRE(first_css == second_css);
  REQUIRE(first.find(".gd-split, .gd-split *") != std::string::npos);
  REQUIRE(first.find(".gd-split a:not(:last-of-type)::after") != std::string::npos);
  REQUIRE(first.find(".gd-split a b") != std::string::npos);
  REQUIRE(first.find(words) != std::string::npos);
  REQUIRE(first_css.find("font-size:") == std::string::npos);
}

TEST_CASE("MeCab tagger helper owns mutable arguments", "[mecab]")
{
  std::vector<std::string> args{ "test", "--dicdir=res/mandarin_dict" };
  std::ranges::copy(mecab_output_arguments(mecab_dictionary_kind::mandarin), std::back_inserter(args));
  auto const tagger = create_mecab_tagger(std::move(args));
  REQUIRE(tagger != nullptr);
  auto const* parsed = tagger->parse("中国");
  REQUIRE(parsed != nullptr);
  REQUIRE(
    render_mecab_output(parsed, "", mecab_dictionary_kind::mandarin) == R"(<a href="bword:中國" title="中國">中國</a>)"
  );
  REQUIRE_THROWS_AS(create_mecab_tagger({ "test", "--dicdir=res/no-such-dictionary" }), gd::runtime_error);
}

TEST_CASE("Japanese inflected words link to their dictionary headword", "[mecab]")
{
  if (find_dic_dir().empty()) {
    SKIP("Japanese dictionary is not installed.");
  }
  std::vector<std::string_view> const args{ "--word", "食べ", "--sentence", "食べた" };
  auto const result = render_mecab(args);
  REQUIRE(
    split_html_words(result)
    == R"(<a href="bword:食べる" title="食べる"><b>食べ</b></a><a href="bword:た" title="た">た</a>)"
  );
}

TEST_CASE("Both commands escape real MeCab output before inserting HTML", "[mecab-html]")
{
  auto const render = GENERATE(&render_mecab, &render_mandarin);
  auto const word = GENERATE("<", ">", "&", "\"", "'");
  // Keep the fixture's system and user dictionaries compatible instead of discovering a Japanese user dictionary.
  std::vector<std::string_view> const args{
    "--dic-dir", "res/mandarin_dict", "--user-dict", "res/mandarin_dict/user.dic", "--word", word, "--sentence", word
  };
  auto const result = render(args);
  auto const escaped = escape_html(word);
  REQUIRE(split_html_words(result) == std::format(R"(<a href="bword:{0}" title="{0}"><b>{0}</b></a>)", escaped));
}

TEST_CASE("Dictionary status errors become application errors instead of aborting", "[mecab][mandarin]")
{
  auto const render = GENERATE(&render_mecab, &render_mandarin);
  auto const option = GENERATE("--dic-dir", "--user-dict");
  // Linux rejects a filename component longer than 255 bytes without creating any files.
  std::string const invalid_path(256, 'x');
  std::vector<std::string_view> const args{ "--dic-dir",   "res/mandarin_dict",
                                            "--user-dict", "res/mandarin_dict/user.dic",
                                            "--word",      "你好",
                                            option,        invalid_path };
  REQUIRE_THROWS_AS(render(args), gd::runtime_error);
}
