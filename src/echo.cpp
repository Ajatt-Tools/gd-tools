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

#include "kana_conv.h"
#include "precompiled.h"
#include "util.h"

using namespace std::string_view_literals;

static constexpr std::size_t default_len{ CharByteLen::THREE * 5UL };
static constexpr std::string_view help_text = R"EOF(usage: gd-echo [OPTIONS]

Echo input back to GoldenDict as HTML with the KanjiStrokeOrders font applied.

OPTIONS
  --max-len NUMBER  maximum length of the input string (bytes).
  --font-size SIZE  font size (with unit).
  --font-family     font family (name).
  --word WORD       text to print.

EXAMPLES
  gd-echo --max-len 5 --font-size 10rem --word "書"
  gd-echo --max-len 3 --font-size 120px --word "薔薇"
)EOF";

struct stroke_order_params
{
  std::string_view gd_word{};
  std::string_view font_size{ "10rem" };
  std::string_view font_family{ "KanjiStrokeOrders" };
  std::size_t max_len{ default_len };

  void assign(std::string_view const key, std::string_view const value)
  {
    if (key == "--max-len") {
      max_len = parse_number<std::size_t>(value).value_or(default_len);
    } else if (key == "--font-size") {
      font_size = value;
    } else if (key == "--font-family") {
      font_family = value;
    } else if (key == "--word") {
      gd_word = value;
    }
  }
};

void print_css(stroke_order_params const& params)
{
  static constexpr std::string_view css = R"EOF(
  <style>
  .gd_echo_{} {{
      font-size: {};
      font-family: "{}";
  }}
  </style>
  )EOF";
  std::print(css, this_pid, params.font_size, params.font_family);
}

void print_with_stroke_order(stroke_order_params const& params)
{
  if (params.gd_word.length() <= params.max_len) {
    std::println("<div class=\"gd_echo_{}\">{}</div>", this_pid, params.gd_word);
    print_css(params);
  }
}

void stroke_order(std::span<std::string_view const> const args)
{
  try {
    print_with_stroke_order(fill_args<stroke_order_params>(args));
  } catch (gd::help_requested const& ex) {
    std::print(help_text);
  } catch (gd::runtime_error const& ex) {
    std::println("{}", ex.what());
  }
}
