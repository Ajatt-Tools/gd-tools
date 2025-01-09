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

#include "anki_search.h"
#include "echo.h"
#include "images.h"
#include "marisa_split.h"
#include "massif.h"
#include "mecab_split.h"
#include "precompiled.h"
#include "translate.h"
#include "util.h"

static constexpr std::string_view help_text = R"EOF(usage: {} ACTION [OPTIONS]
A set of helpful programs to enhance GoldenDict for immersion learning.

ACTIONS
  ankisearch  Search word in Anki.
  massif      Search word on Massif.
  images      Search images on Bing.
  translate   Translate text using argostranslate.
  marisa      Split search string using MARISA.
  mecab       Split search string using Mecab.
  strokeorder Show stroke order of a word.
  handwritten Display the handwritten form of a word.

OPTIONS
  -h,--help  Print this help screen.

EXAMPLES
gd-tools ankisearch --field-name VocabKanji %GDWORD%
gd-ankisearch --field-name VocabKanji %GDWORD%
gd-ankisearch --deck-name Mining %GDWORD%
)EOF";

auto get_help_str(std::string_view program_name) -> std::string
{
  return std::format(help_text, program_name);
}

auto print_help(std::string_view const program_name) -> void
{
  std::print("{}", get_help_str(program_name));
}

auto base_name(auto file_path) -> std::string
{
  return std::filesystem::path(file_path).filename();
}

template<std::integral Ret = uint64_t>
constexpr auto djbx33a(std::string_view const s) -> Ret
{
  static constexpr Ret init = 5381;
  static constexpr Ret mul = 33;

  Ret acc = init;
  for (auto const ch: s) { acc = (acc * mul) + static_cast<Ret>(ch); }
  return acc;
}

constexpr auto operator""_h(char const* s, [[maybe_unused]] size_t const size)
{
  return djbx33a(std::string_view(s, size));
}

auto take_action(std::span<std::string_view const> const args) -> void
{
  auto const program_name = base_name(args.front());

  // Command passed as program name (first arg).
  std::span rest = args.subspan(1);
  switch (djbx33a(program_name)) {
  case "gd-ankisearch"_h:
    return search_anki_cards(rest);
  case "gd-echo"_h:
    return stroke_order(rest);
  case "gd-massif"_h:
    return massif(rest);
  case "gd-images"_h:
    return images(rest);
  case "gd-translate"_h:
    return translate(rest);
  case "gd-marisa"_h:
    return marisa_split(rest);
  case "gd-mecab"_h:
    return mecab_split(rest);
  }

  // Help requested explicitly.
  if (std::size(args) < 2 or args[1] == "-h" or args[1] == "--help") {
    return print_help(program_name);
  }

  // Command passed as second arg (first is "gd-tools").
  rest = rest.subspan(1);
  switch (djbx33a(args[1])) {
  case "ankisearch"_h:
    return search_anki_cards(rest);
  case "echo"_h:
    return stroke_order(rest);
  case "massif"_h:
    return massif(rest);
  case "images"_h:
    return images(rest);
  case "translate"_h:
    return translate(rest);
  case "marisa"_h:
    return marisa_split(rest);
  case "mecab"_h:
    return mecab_split(rest);
  }

  // Couldn't determine command.
  return print_help(program_name);
}

auto main(int const argc, char const* const* const argv) -> int
{
  take_action(std::vector<std::string_view>{ argv, std::next(argv, argc) });
  return 0;
}
