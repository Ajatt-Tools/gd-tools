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
static constexpr std::string_view help_text = R"EOF(usage: gd-images [OPTIONS]

Get images from Bing.

OPTIONS
  --max-time SECONDS  maximum time in seconds to wait for response.
  --word WORD         search term.

EXAMPLES
  gd-images --max-time 6 --word "犬"
  gd-images --word 猫
)EOF";
static constexpr std::string_view css_style = R"EOF(<style>
    .gallery {
        display: grid;
        gap: 10px;
        margin: 0;
        justify-items: center;
        align-items: start;
        align-content: start;
        justify-content: space-between;
        grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
    }
    .gallery img {
        margin: 0 auto;
        max-width: 100%;
        width: 100%;
        border-radius: 5px;
        display: block;
        max-height: 95vh;
        object-fit: contain;
    }
</style>)EOF";

struct images_params
{
  std::chrono::seconds max_time{ default_max_time_s };
  std::string gd_word{};

  void assign(std::string_view const key, std::string_view const value)
  {
    if (key == "--max-time") {
      max_time = std::chrono::seconds{ parse_number<std::size_t>(value).value_or(default_max_time_s) };
    } else if (key == "--word") {
      gd_word = value;
    }
  }
};

void fetch_images(images_params const& params)
{
  cpr::Response const r = cpr::Get(
    cpr::Url{ "https://www.bing.com/images/search"sv },
    cpr::Parameters{ { "q", params.gd_word }, { "mkt", "ja-JP" } },
    cpr::Header{ { "User-Agent", "Mozilla/5.0" } },
    cpr::VerifySsl{ false },
    cpr::Timeout{ params.max_time }
  );
  raise_if(r.status_code != cpr::status::HTTP_OK, "Couldn't connect to Bing.");
  static std::regex const img_re("<img[^<>]*class=\"mimg[^<>]*>");
  auto images_begin = std::sregex_iterator(std::begin(r.text), std::end(r.text), img_re);
  auto images_end = std::sregex_iterator();
  std::println("<div class=\"gallery\">");
  for (auto const& match: std::ranges::subrange(images_begin, images_end) | std::views::take(5)) {
    std::println("{}", match.str());
  }
  std::println("</div>");
  std::println("{}", css_style);
}

void images(std::span<std::string_view const> const args)
{
  try {
    fetch_images(fill_args<images_params>(args));
  } catch (gd::help_requested const& ex) {
    std::print(help_text);
  } catch (gd::runtime_error const& ex) {
    std::println("{}", ex.what());
  }
}
