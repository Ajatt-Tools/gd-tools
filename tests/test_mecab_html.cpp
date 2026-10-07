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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "mandarin_split.h"
#include "mecab_html.h"
#include "mecab_split.h"
#include "util.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE("HTML escaping preserves text and escapes markup delimiters", "[mecab-html]")
{
  REQUIRE(escape_html("").empty());
  REQUIRE(escape_html("中國 猫 abc") == "中國 猫 abc");
  REQUIRE(escape_html(R"(<&"'>)") == "&lt;&amp;&quot;&#39;&gt;");
  REQUIRE(escape_html("&lt;") == "&amp;lt;");
}

TEST_CASE("MeCab links escape fields before applying highlighting", "[mecab-html]")
{
  auto const kind = GENERATE(mecab_dictionary_kind::japanese, mecab_dictionary_kind::mandarin);
  auto const word = R"(<&"'>)";
  auto const output = std::format(
    "{}{}{}{}{}",
    word,
    mecab_separators::component,
    word,
    mecab_separators::node,
    mecab_separators::footer
  );
  REQUIRE(
    render_mecab_output(output, word, kind)
    == R"(<a href="bword:&lt;&amp;&quot;&#39;&gt;" title="&lt;&amp;&quot;&#39;&gt;"><b>&lt;&amp;&quot;&#39;&gt;</b></a>)"
  );
}

TEST_CASE("Known MeCab words use dictionary-specific displayed text", "[mecab-html]")
{
  auto const kind = GENERATE(mecab_dictionary_kind::japanese, mecab_dictionary_kind::mandarin);
  auto const output =
    std::format("中国{}中國{}{}", mecab_separators::component, mecab_separators::node, mecab_separators::footer);
  auto const word = kind == mecab_dictionary_kind::mandarin ? "中國" : "中国";
  REQUIRE(
    render_mecab_output(output, word, kind) == std::format(R"(<a href="bword:中國" title="中國"><b>{}</b></a>)", word)
  );
}

TEST_CASE("Unknown or missing headwords fall back to the word", "[mecab-html]")
{
  auto const kind = GENERATE(mecab_dictionary_kind::japanese, mecab_dictionary_kind::mandarin);
  auto const record = GENERATE("abc", "abc<gd-tools-component>", "abc<gd-tools-component>*");
  auto const output = std::format("{}{}{}", record, mecab_separators::node, mecab_separators::footer);
  REQUIRE(render_mecab_output(output, "", kind) == R"(<a href="bword:abc" title="abc">abc</a>)");
}

TEST_CASE("MeCab renders multiple nodes without leaking protocol markers", "[mecab-html]")
{
  auto const output = std::format(
    "猫{}猫{}abc{}{}",
    mecab_separators::component,
    mecab_separators::node,
    mecab_separators::node,
    mecab_separators::footer
  );
  REQUIRE(
    render_mecab_output(output, "猫", mecab_dictionary_kind::japanese)
    == R"(<a href="bword:猫" title="猫"><b>猫</b></a><a href="bword:abc" title="abc">abc</a>)"
  );
}

TEST_CASE("MeCab rejects reserved separators in input", "[mecab-html]")
{
  auto const separator = GENERATE(mecab_separators::component, mecab_separators::node, mecab_separators::footer);
  REQUIRE_THROWS_AS(validate_mecab_input(std::format("before{}after", separator)), gd::runtime_error);
  REQUIRE_NOTHROW(validate_mecab_input("<script>& ordinary text"));
}

TEST_CASE("MeCab rejects malformed structured output", "[mecab-html]")
{
  auto const output = GENERATE(
    "",
    "word",
    "word<gd-tools-footer>",
    "<gd-tools-node><gd-tools-footer>",
    "<gd-tools-component>headword<gd-tools-node><gd-tools-footer>",
    "word<gd-tools-component>headword<gd-tools-component>extra<gd-tools-node><gd-tools-footer>",
    "word<gd-tools-node><gd-tools-node><gd-tools-footer>",
    "word<gd-tools-footer><gd-tools-node><gd-tools-footer>"
  );
  REQUIRE_THROWS_AS(render_mecab_output(output, "", mecab_dictionary_kind::japanese), gd::runtime_error);
}

TEST_CASE("MeCab accepts an empty analysis with a footer", "[mecab-html]")
{
  REQUIRE(render_mecab_output(mecab_separators::footer, "", mecab_dictionary_kind::japanese).empty());
}

TEST_CASE("Commands validate separators after whitespace removal", "[mecab-html]")
{
  auto const render = GENERATE(&render_mecab, &render_mandarin);
  std::vector<std::string_view> const args{ "--dic-dir", "res/mandarin_dict", "--word",
                                            "你好",      "--sentence",        "<gd-tools- component>" };
  REQUIRE_THROWS_WITH(render(args), Catch::Matchers::ContainsSubstring("reserved MeCab separator"));
}

TEST_CASE("MeCab debug comments escape user-controlled paths", "[mecab-html]")
{
  mecab_params const params{ .user_dict = R"(user--><script>&"'.dic)", .dic_dir = "dictionary" };
  std::vector<std::string> const args{ "arg0", "--userdic=" + params.user_dict.string() };
  auto const output = format_mecab_debug_info(params, args);
  REQUIRE(output.starts_with("\n<!--\n"));
  REQUIRE(output.find("-->") == output.rfind("-->"));
  REQUIRE(output.find("<script>") == std::string::npos);
  REQUIRE(output.find("&lt;script&gt;") != std::string::npos);
}
