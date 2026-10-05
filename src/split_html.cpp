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

#include "split_html.h"

static constexpr std::string_view css_style = R"EOF(
<style>
  .gd-split, .gd-split * {
    margin: 0;
    padding: 0;
  }
  .gd-split {
    color: #1268c3;
    font-weight: normal;
  }
  .gd-split a {
    display: inline-block;
    font-weight: normal;
    color: royalblue;
    text-decoration: none;
  }
  .gd-split a:not(:last-of-type)::after {
    content: "";
    display: inline-block;
    background-color: #333;
    margin: 4px;
    width: 3px;
    height: 3px;
    border-radius: 100vmax;
    vertical-align: middle;
    cursor: text;
    user-select: text;
  }
  .gd-split a b {
    background-color: #ddeeff;
    border-radius: 0.2rem;
    font-weight: 500;
  }
</style>
)EOF";

auto render_split_html(std::string_view const words, std::string_view const font_size) -> std::string
{ return std::format(R"({}<div class="gd-split" style="font-size: {}">{}</div>)", css_style, font_size, words); }
