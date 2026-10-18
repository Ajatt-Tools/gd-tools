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
#pragma once

#include "util.h"
#include <string_view>

/** Return the words inside the wrapper as a view borrowing the supplied HTML. */
inline auto split_html_words(std::string_view const html) -> std::string_view
{
  auto const wrapper = html.find(R"(<div class="gd-split")");
  raise_if(wrapper == std::string_view::npos, "Missing split HTML wrapper.");
  auto const start = html.find('>', wrapper);
  auto const end = html.find("</div>", start);
  raise_if(start == std::string_view::npos or end == std::string_view::npos, "Malformed split HTML wrapper.");
  return html.substr(start + 1, end - start - 1);
}
