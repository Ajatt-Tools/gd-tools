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
#include "images.h"
#include "marisa.h"
#include "massif.h"
#include "precompiled.h"
#include "stroke_order.h"

static constexpr std::string_view help_text = R"EOF(usage: {} ACTION [OPTIONS]
A set of helpful programs to enhance GoldenDict for immersion learning.

ACTIONS
  ankisearch  Search word in Anki.
  massif      Search word on Massif.
  images      Search images on Bing.
  marisa      Split search string using MARISA.
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
  return fmt::format(help_text, program_name);
}

auto print_help(std::string_view const program_name) -> void
{
  fmt::print("{}", get_help_str(program_name));
}

auto base_name(auto file_path) -> std::string
{
  return std::filesystem::path(file_path).filename();
}

auto take_action(std::span<std::string_view const> const args) -> void
{
  auto const program_name = base_name(args[0]);

  // Command passed as program name (first arg).
  if (program_name == "gd-ankisearch") {
    return search_anki_cards(args | std::views::drop(1));
  }
  if (program_name == "gd-echo") {
    return stroke_order(args | std::views::drop(1));
  }
  if (program_name == "gd-massif") {
    return massif(args | std::views::drop(1));
  }
  if (program_name == "gd-images") {
    return images(args | std::views::drop(1));
  }
  if (program_name == "gd-marisa") {
    return marisa_split(args | std::views::drop(1));
  }

  // Help requested explicitly.
  if (std::size(args) < 2 or args[1] == "-h" or args[1] == "--help") {
    return print_help(program_name);
  }

  // Command passed as second arg (first is "gd-tools").
  if (args[1] == "ankisearch") {
    return search_anki_cards(args | std::views::drop(2));
  }
  if (args[1] == "echo") {
    return stroke_order(args | std::views::drop(2));
  }
  if (args[1] == "massif") {
    return massif(args | std::views::drop(2));
  }
  if (args[1] == "images") {
    return images(args | std::views::drop(2));
  }
  if (args[1] == "marisa") {
    return marisa_split(args | std::views::drop(2));
  }

  // Couldn't determine command.
  return print_help(program_name);
}

auto main(int const argc, char const* const* const argv) -> int
{
  take_action(std::vector<std::string_view>{ argv, std::next(argv, argc) });
  return 0;
}
