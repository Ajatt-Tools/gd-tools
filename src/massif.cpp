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

#include "precompiled.h"
#include "util.h"

using namespace std::literals;

static constexpr std::size_t default_max_time_s{ 6 };
static constexpr std::string_view help_text = R"EOF(usage: gd-massif [OPTIONS]

Get example sentences from Massif.

OPTIONS
  --max-time SECONDS  maximum time in seconds to wait for response.
  --word WORD         search term.

EXAMPLES
  gd-massif --max-time 6 --word "貴様"
  gd-massif --word 貴様
)EOF";
static constexpr std::string_view css_style = R"EOF(<style>
    .gd-massif {
        padding-inline-start: 0;
        margin-inline-start: 0;
    }
    .gd-massif li {
        font-size: 1.5rem;
        list-style: none;
    }
    .gd-massif .result-meta {
        font-size: 12px;
    }
    .gd-massif a {
        color: royalblue;
    }
    .gd-massif em {
        font-style: normal;
        background-color: #ddeeff;
        border-radius: 0.2rem;
    }
</style>)EOF";

struct massif_params
{
  std::chrono::seconds max_time{ default_max_time_s };
  std::string_view gd_word{};

  void assign(std::string_view const key, std::string_view const value)
  {
    if (key == "--max-time") {
      max_time = std::chrono::seconds{ parse_number<std::size_t>(value).value_or(default_max_time_s) };
    } else if (key == "--word") {
      gd_word = value;
    }
  }
};

void fetch_massif_examples(massif_params const& params)
{
  cpr::Response const r = cpr::Get(
    cpr::Url{ std::format("https://massif.la/ja/search?q={}", params.gd_word) },
    cpr::Timeout{ params.max_time },
    cpr::VerifySsl{ false }
  );
  raise_if(r.status_code != 200, "Couldn't connect to Massif.");
  std::println("<ul class=\"gd-massif\">");
  for (auto const& line:
       r.text //
         | std::views::split('\n') //
         | std::views::transform([](auto const& sub_view) { return std::string_view{ sub_view }; })
         | std::views::drop_while([](auto const str_view) {
             return not str_view.contains("<li class=\"text-japanese\">");
           })
         | std::views::take_while([](auto const str_view) { return not str_view.contains("</ul>"); })) {
    std::println("{}", line);
  }
  std::println("</ul>");
  std::println("{}", css_style);
}

void massif(std::span<std::string_view const> const args)
{
  try {
    fetch_massif_examples(fill_args<massif_params>(args));
  } catch (gd::help_requested const& ex) {
    std::print(help_text);
  } catch (gd::runtime_error const& ex) {
    std::println("{}", ex.what());
  }
}
