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
