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

// TODO search dict.dic in more locations
// TODO convert half width chars to full width chars
// TODO inflections; fix things like グラついた
// 首をなでられた
// 護り手たちがボヤいてたんだよ

using namespace std::string_literals;
static constexpr std::string_view help_text = R"EOF(usage: gd-marisa [OPTIONS]

Split sentence using MARISA and print links to each word.

OPTIONS
  --word WORD          required word
  --sentence SENTENCE  required sentence
  --path-to-dic        optional path to words.dic

EXAMPLES
gd-marisa --word %GDWORD% --sentence %GDSEARCH%
)EOF";
static constexpr std::string_view css_style = R"EOF(
<style>
  .gd-marisa {
    font-size: 2rem;
    margin-bottom: 0.05em;
    margin-top: -0.2em;
    color: #1268c3;
    font-weight: normal;
  }
  .gd-marisa a {
    display: inline-block;
    font-weight: normal;
    color: royalblue;
    text-decoration: none;
    border-bottom: dashed max(1px, calc(1em / 16)) currentColor;
  }
  .gd-marisa a.gd-headword {
    background-color: #ddeeff;
    border-radius: 0.2rem;
    font-weight: 500;
  }
  .gd-marisa > ul {
    --size: 1rem;
    font-size: var(--size);
    padding-inline-start: var(--size);
    margin-block: 2px;
  }
</style>
)EOF";
static constexpr std::size_t max_forward_search_len_bytes{ CharByteLen::THREE * 10UL };

struct marisa_params
{
  std::string gd_word{};
  std::string gd_sentence{};
  std::string path_to_dic{ "/usr/share/gd-tools/marisa_words.dic" };

  auto assign(std::string_view const key, std::string_view const value) -> void
  {
    if (key == "--word") {
      gd_word = value;
    } else if (key == "--sentence") {
      gd_sentence = value;
    } else if (key == "--path-to-dic") {
      path_to_dic = value;
    }
  }
};

auto longest_result(
  marisa::Agent& agent,
  marisa::Trie const& trie,
  std::string const& search_str,
  std::size_t longestlen
) -> std::string
{
  agent.set_query(search_str.c_str());
  while (trie.common_prefix_search(agent)) { longestlen = agent.key().length(); }
  return search_str.substr(0, longestlen);
}

auto longest_variant(
  marisa::Agent& agent,
  marisa::Trie const& trie,
  std::string const& search_str,
  std::size_t const longestlen
)
{
  return std::max(
    { longest_result(agent, trie, search_str, longestlen),
      longest_result(agent, trie, katakana_to_hiragana(search_str), longestlen),
      longest_result(agent, trie, hiragana_to_katakana(search_str), longestlen) },
    [](auto const& a, auto const& b) { return a.length() < b.length(); }
  );
}

void lookup_words(marisa_params params)
{
  std::erase_if(params.gd_word, is_space);
  std::erase_if(params.gd_sentence, is_space);

  if (params.gd_sentence.empty()) {
    params.gd_sentence = params.gd_word;
  }

  marisa::Trie trie;
  marisa::Agent agent;

  std::ifstream file{ params.path_to_dic };
  raise_if(not file.good(), fmt::format("Error. The dictionary file \"{}\" does not exist.", params.path_to_dic));
  trie.load(params.path_to_dic.c_str());

  fmt::print("<div class=\"gd-marisa\">\n");
  std::ptrdiff_t pos_in_gd_word{ 0 };

  for (auto const [idx, uni_char]: iter_unicode_chars(params.gd_sentence)) {
    std::string const bword =
      longest_variant(agent, trie, params.gd_sentence.substr(idx, max_forward_search_len_bytes), uni_char.length());
    pos_in_gd_word = params.gd_word == bword ? bword.length() : pos_in_gd_word - uni_char.length();
    fmt::print("<a{} href=\"bword:{}\">{}</a>", (pos_in_gd_word > 0 ? " class=\"gd-headword\"" : ""), bword, uni_char);
  }

  // Show available entries for smaller substrings
  agent.set_query(params.gd_word.c_str());
  fmt::print("<ul>\n");
  while (trie.common_prefix_search(agent) and agent.key().length() != params.gd_word.length()) {
    auto const word = std::string(agent.key().ptr(), agent.key().length());
    fmt::print("<li><a href=\"bword:{}\">{}</a></li>\n", word, word);
  }
  fmt::print("</ul>\n"); // close ul
  fmt::print("</div>\n"); // close div.gd-marisa
  fmt::print("{}\n", css_style);
}

void marisa_split(std::span<std::string_view const> const args)
{
  try {
    lookup_words(fill_args<marisa_params>(args));
  } catch (help_requested const& ex) {
    fmt::print(help_text);
  } catch (runtime_error const& ex) {
    fmt::print("{}\n", ex.what());
  }
}