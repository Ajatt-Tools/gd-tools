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

#include "mecab_split.h"
#include "util.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE("Shared MeCab parameters have no language-specific dictionary defaults", "[mecab-params]")
{
  mecab_params const params;
  REQUIRE(params.dic_dir.empty());
  REQUIRE(params.user_dict.empty());
  REQUIRE(params.font_size == "2rem");
}

TEST_CASE("MeCab argument parsing preserves command dictionary defaults", "[mecab-params]")
{
  auto const directory = GENERATE("japanese-dictionary", "mandarin-dictionary");
  std::vector<std::string_view> const args{ "--word", "word" };
  auto const params = fill_args<mecab_params>(args, { .user_dict = "user.dic", .dic_dir = directory });
  REQUIRE(params.gd_word == "word");
  REQUIRE(params.dic_dir == directory);
  REQUIRE(params.user_dict == "user.dic");
  REQUIRE(params.font_size == "2rem");
}

TEST_CASE("Shared MeCab parameters accept option overrides", "[mecab-params]")
{
  std::vector<std::string_view> const args{ "--word",      "word",         "--sentence",
                                            "sentence",    "--dic-dir",    "override-dictionary",
                                            "--user-dict", "override.dic", "--font-size",
                                            "30px" };
  auto const params = fill_args<mecab_params>(args, { .user_dict = "default.dic", .dic_dir = "default-dictionary" });
  REQUIRE(params.gd_word == "word");
  REQUIRE(params.gd_sentence == "sentence");
  REQUIRE(params.dic_dir == "override-dictionary");
  REQUIRE(params.user_dict == "override.dic");
  REQUIRE(params.font_size == "30px");
}

TEST_CASE("Shared MeCab parameters reject unknown or incomplete options", "[mecab-params]")
{
  auto const option = GENERATE("--unknown", "--dic-dir", "--word");
  std::vector<std::string_view> const args{ option };
  REQUIRE_THROWS_AS(fill_args<mecab_params>(args), gd::runtime_error);
}

TEST_CASE("Shared MeCab parameters reject unknown option names with values", "[mecab-params]")
{
  std::vector<std::string_view> const args{ "--word", "word", "--unknown", "value" };
  REQUIRE_THROWS_WITH(fill_args<mecab_params>(args), "Unknown argument name: --unknown");
}

TEST_CASE("Shared MeCab arguments preserve language formats and optional user dictionaries", "[mecab-params]")
{
  auto const kind = GENERATE(mecab_dictionary_kind::japanese, mecab_dictionary_kind::mandarin);
  auto const user_dict = GENERATE(
    "",
    "res/mandarin_dict/user.dic",
    "res/no-such-user-dictionary.dic",
    "res/mandarin_dict/dicrc/not-a-directory"
  );
  mecab_params const params{ .user_dict = user_dict, .dic_dir = "res/mandarin_dict" };
  std::vector<std::string> expected{ "arg0", "--dicdir=res/mandarin_dict", "--output-format-type=" };
  auto const field = kind == mecab_dictionary_kind::mandarin ? "5" : "6";
  expected.push_back(std::format("--node-format=%m<gd-tools-component>%f[{}]<gd-tools-node>", field));
  expected.insert(
    expected.end(), { "--unk-format=%m<gd-tools-node>", "--bos-format=", "--eos-format=<gd-tools-footer>" }
  );
  if (std::string_view(user_dict) == "res/mandarin_dict/user.dic") {
    expected.push_back("--userdic=" + std::string(user_dict));
  }
  REQUIRE(create_mecab_args(params, kind) == expected);
  REQUIRE(params.user_dict == user_dict);
}

TEST_CASE("Regular-file checks preserve absent and non-file results", "[filesystem]")
{
  auto const path = GENERATE(
    "res/mandarin_dict/dicrc",
    "res/mandarin_dict",
    "",
    "res/no-such-user-dictionary.dic",
    "res/mandarin_dict/dicrc/not-a-directory"
  );
  REQUIRE(gd::is_regular_file_or_throw(path) == (std::string_view(path) == "res/mandarin_dict/dicrc"));
}

TEST_CASE("Regular-file checks translate filesystem exceptions", "[filesystem]")
{
  std::string const invalid_path(256, 'x');
  REQUIRE_THROWS_AS(gd::is_regular_file_or_throw(invalid_path), gd::runtime_error);
  REQUIRE_THROWS_WITH(
    gd::is_regular_file_or_throw(invalid_path),
    Catch::Matchers::StartsWith("Couldn't inspect file: ")
  );
}

TEST_CASE("Directory checks preserve absent and non-directory results", "[filesystem]")
{
  auto const path = GENERATE(
    "res/mandarin_dict",
    "res/mandarin_dict/dicrc",
    "",
    "res/no-such-dictionary",
    "res/mandarin_dict/dicrc/not-a-directory"
  );
  REQUIRE(gd::is_directory_or_throw(path) == (std::string_view(path) == "res/mandarin_dict"));
}

TEST_CASE("Directory checks translate filesystem exceptions", "[filesystem]")
{
  std::string const invalid_path(256, 'x');
  REQUIRE_THROWS_AS(gd::is_directory_or_throw(invalid_path), gd::runtime_error);
  REQUIRE_THROWS_WITH(
    gd::is_directory_or_throw(invalid_path),
    Catch::Matchers::StartsWith("Couldn't inspect directory: ")
  );
}
