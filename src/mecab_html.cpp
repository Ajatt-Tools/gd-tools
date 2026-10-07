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

#include "mecab_html.h"
#include "precompiled.h"
#include "util.h"
#include <string_view>

auto validate_mecab_input(std::string_view const sentence) -> void
{
  for (std::string_view const separator: {
         mecab_separators::component,
         mecab_separators::node,
         mecab_separators::footer,
       }) {
    raise_if(sentence.contains(separator), "Input contains a reserved MeCab separator.");
  }
}

auto mecab_output_arguments(mecab_dictionary_kind const kind) -> std::vector<std::string>
{
  auto const* const headword = kind == mecab_dictionary_kind::mandarin ? "%f[5]" : "%f[6]";

  return {
    // Disable named output formats from dicrc.
    "--output-format-type=",
    std::format("--node-format=%m{}{}{}", mecab_separators::component, headword, mecab_separators::node),
    std::format("--unk-format=%m{}", mecab_separators::node),
    "--bos-format=",
    std::format("--eos-format={}", mecab_separators::footer),
  };
}

auto validate_mecab_output(std::string_view output) -> std::string_view
{
  raise_if(not output.ends_with(mecab_separators::footer), "Missing MeCab output footer.");
  output.remove_suffix(mecab_separators::footer.size());
  raise_if(output.contains(mecab_separators::footer), "Unexpected MeCab output footer.");
  if (output.empty()) {
    return {};
  }
  raise_if(not output.ends_with(mecab_separators::node), "Missing MeCab node separator.");
  output.remove_suffix(mecab_separators::node.size());
  return output;
}

struct mecab_word
{
  std::string_view word;
  std::string_view headword;
};

auto parse_mecab_word(std::string_view const text, mecab_dictionary_kind const kind) -> mecab_word
{
  auto ret = mecab_word{ .word = text, .headword = text };
  if (auto const position = text.find(mecab_separators::component); position != std::string_view::npos) {
    ret.word = text.substr(0, position);
    ret.headword = text.substr(position + mecab_separators::component.size());
    raise_if(ret.headword.contains(mecab_separators::component), "Unexpected MeCab output components.");
  }
  raise_if(ret.word.empty(), "Empty MeCab word.");

  if (ret.headword.empty() or ret.headword == "*") {
    ret.headword = ret.word;
  }

  if (kind == mecab_dictionary_kind::mandarin) {
    ret.word = ret.headword;
  }

  return ret;
}

auto render_mecab_output(
  std::string_view const output,
  std::string_view const highlighted_word,
  mecab_dictionary_kind const kind
) -> std::string
{
  std::string html;
  auto nodes = // this value can't be const.
    validate_mecab_output(output) //
    | std::views::split(mecab_separators::node) //
    | std::views::transform([](auto const section) { return std::string_view(section.begin(), section.end()); });

  for (std::string_view const text: nodes) {
    raise_if(text.empty(), "Empty MeCab output node.");

    auto const m_word = parse_mecab_word(text, kind);

    auto label = escape_html(m_word.word);
    if (m_word.word == highlighted_word) {
      label = std::format("<b>{}</b>", label);
    }

    auto const escaped_headword = escape_html(m_word.headword);
    html += std::format(R"EOF(<a href="bword:{}" title="{}">{}</a>)EOF", escaped_headword, escaped_headword, label);
  }

  return html;
}
