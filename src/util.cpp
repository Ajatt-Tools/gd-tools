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

#include "util.h"
#include "precompiled.h"

auto determine_card_class(int64_t const card_queue, int64_t const card_type) noexcept -> std::string_view
{
  // determine card class to be used in CSS when printing
  // https://github.com/ankidroid/Anki-Android/wiki/Database-Structure
  switch (card_queue) {
  case -3:
  case -2:
    return "buried";
  case -1:
    return "suspended";
  case 0:
    return "new";
  case 1:
  case 3:
    return "learning";
  case 2:
    return "review";
  default:
    break;
  }
  switch (card_type) {
  case 0:
    return "new";
  case 1:
  case 3:
    return "learning";
  case 2:
    return "review";
  default:
    break;
  }
  return "unknown";
}

auto is_space(char const ch) noexcept -> bool
{
  return static_cast<bool>(std::isspace(static_cast<unsigned char>(ch)));
}

auto strtrim(std::string_view str) noexcept -> std::string
{
  auto v = std::views::all(str) //
           | std::views::reverse //
           | std::views::drop_while(is_space) //
           | std::views::reverse //
           | std::views::drop_while(is_space);
  return std::string{ v.begin(), v.end() };
}

void str_replace(std::string& str, std::string_view from, std::string_view to) noexcept
{
  if (auto idx = str.find(from); idx != std::string::npos) {
    str.replace(idx, from.length(), to);
  }
}
