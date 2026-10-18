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

#pragma once

#include "precompiled.h"

/** Dictionary feature layout and displayed-word convention. */
enum class mecab_dictionary_kind {
  japanese,
  mandarin,
};

namespace mecab_separators {
inline constexpr std::string_view component = "<gd-tools-component>";
inline constexpr std::string_view node = "<gd-tools-node>";
inline constexpr std::string_view footer = "<gd-tools-footer>";
} // namespace mecab_separators

/** Reject reserved protocol markers before passing input to MeCab. */
auto validate_mecab_input(std::string_view sentence) -> void;

/** Build output formats that separate nodes and their components. */
auto mecab_output_arguments(mecab_dictionary_kind kind) -> std::vector<std::string>;

/** Convert structured MeCab output into escaped, highlighted links. */
auto render_mecab_output(std::string_view output, std::string_view highlighted_word, mecab_dictionary_kind kind)
  -> std::string;
/** Validate protocol framing and return a view borrowing output, with its final separators removed. */
auto validate_mecab_output(std::string_view output) -> std::string_view;
