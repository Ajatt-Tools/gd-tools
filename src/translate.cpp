/*
 *  gd-tools - a set of programs to enhance goldendict for immersion learning.
 *  Copyright (C) 2025 Ajatt-Tools
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

#include "precompiled.h"
#include "util.h"

using namespace std::literals;
namespace sp = subprocess;

static constexpr std::string_view help_text = R"EOF(usage: gd-translate [OPTIONS]

Translate text from Japanese to target language

OPTIONS
  --to LANG            target language (default: en)
  --sentence SENTENCE  japanese sentence to translate
  --spoiler yes/no     black out the sentence with a spoiler box (default: no)

EXAMPLES
  gd-translate --spoiler yes --sentence %GDSEARCH%
  gd-translate --spoiler yes --to fr --sentence %GDSEARCH%
)EOF";
static constexpr std::string_view css_style = R"EOF(<style>
    .spoiler {
      background-color: black;
      padding: 6px;
      width: fit-content;
    }
    .spoiler:hover {
      background-color: white;
    }
</style>)EOF";

struct translate_params
{
  std::string to{ "en" };
  std::string gd_word;
  bool spoiler{ false };

  void assign(std::string_view const key, std::string_view const value)
  {
    if (key == "--to") {
      to = value;
    } else if (key == "--sentence") {
      gd_word = value;
    } else if (key == "--spoiler" and value == "yes") {
      spoiler = true;
    }
  }
};

void exec_translate(translate_params const& params)
{
  auto cmd_argos = sp::Popen(
    {
      "argos-translate",
      "-f",
      "ja",
      "-t",
      params.to,
      params.gd_word,
    },
    sp::output{ sp::PIPE },
    sp::error{ sp::PIPE }
  );

  auto const [stdout, stderr] = cmd_argos.communicate();

  std::println("<div{}>", params.spoiler ? " class=\"spoiler\"" : "");
  std::println("{}", std::string_view(stdout.buf.data(), stdout.length));
  std::println("</div>");
  std::println("{}", css_style);
}

void translate(std::span<std::string_view const> const args)
{
  try {
    exec_translate(fill_args<translate_params>(args));
  } catch (gd::help_requested const& ex) {
    std::print(help_text);
  } catch (gd::runtime_error const& ex) {
    std::println("{}", ex.what());
  } catch (std::runtime_error const& ex) {
    std::println("subprocess error. {}", ex.what());
  }
}
