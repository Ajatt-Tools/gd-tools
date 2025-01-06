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

using namespace std::string_literals;
using namespace ajt::rdricpp;
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
  .gd-marisa .alternatives {
    --size: 1rem;
    display: grid;
    font-size: var(--size);
    gap: calc( var(--size) / 4);
    max-width: 100%;
    margin: 0 auto;
    grid-template-columns: repeat(auto-fill, minmax(150px, 1fr));
    align-content: start;
    justify-content: space-around;
    text-align: left;
    padding: 5px 0px;
  }
  .gd-marisa .alternatives > ul {
    list-style-type: none;
    margin: 0;
    padding: calc( var(--size) / 4);
    background-color: hsl(150deg 30% 60% / 10%);
  }
</style>
)EOF";
static constexpr std::size_t max_forward_search_len_bytes{ CharByteLen::THREE * 20UL };

auto find_dic_file() -> std::filesystem::path
{
  static auto const locations = {
    // possible .dic locations
    std::filesystem::path("/usr/share/gd-tools/marisa_words.dic"),
    user_home() / ".local/share/gd-tools/marisa_words.dic",
  };
  for (auto const& location: locations) {
    if (std::filesystem::exists(location) and std::filesystem::is_regular_file(location)) {
      return location;
    }
  }
  throw gd::runtime_error("Couldn't find the word list.");
}

struct marisa_params
{
  std::string gd_word{};
  std::string gd_sentence{};
  std::string path_to_dic{ find_dic_file() };

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

auto cmp_len(std::string_view a, std::string_view b) -> bool
{
  return a.length() < b.length();
}

struct Deinflected
{
  std::string_view from;
  std::vector<Deinflection> to;
};

auto find_deinflections_starting_with(std::string_view const search_str) -> std::vector<Deinflected>
{
  std::vector<Deinflected> hits;
  // loop from larger towards shorter substrings
  for (std::string_view substr: enum_unicode_chars(search_str) //
                                  | std::views::reverse
                                  | std::views::transform( //
                                    [&search_str](Utf8CharView const ch) { //
                                      return search_str.substr(0UL, ch.idx + ch.ch.size());
                                    }
                                  )) {
    hits.emplace_back(substr, deinflect(substr));
  }
  return hits;
}

auto find_keywords_starting_with(marisa::Agent& agent, marisa::Trie const& trie, std::string const& search_str) -> JpSet
{
  JpSet results{};
  auto const variants = { search_str, hiragana_to_katakana(search_str), katakana_to_hiragana(search_str) };

  auto deinflections = std::views::all(variants) //
                       | std::views::transform(find_deinflections_starting_with) //
                       | std::views::join //
                       | std::views::transform([](Deinflected const& group) { return group.to; }) //
                       | std::views::join;

  for (auto const& deinflection: deinflections) {
    agent.set_query(deinflection.term.c_str());
    while (trie.common_prefix_search(agent)) { //
      results.emplace(agent.key().ptr(), agent.key().length());
    }
  }

  return results;
}

void lookup_words(marisa_params params)
{
  half_to_full(params.gd_word);
  std::erase_if(params.gd_word, is_space);
  std::erase_if(params.gd_sentence, is_space);

  if (params.gd_sentence.empty()) {
    params.gd_sentence = params.gd_word;
  } else {
    half_to_full(params.gd_sentence);
  }

  marisa::Trie trie;
  marisa::Agent agent;

  std::ifstream file{ params.path_to_dic };
  raise_if(not file.good(), std::format(R"(Error. The dictionary file "{}" does not exist.)", params.path_to_dic));
  trie.load(params.path_to_dic.c_str());

  std::println(R"(<div class="gd-marisa">)");
  std::ptrdiff_t pos_in_gd_word{ 0 };
  std::vector<JpSet> alternatives{};
  alternatives.reserve(20);

  // Link longest words starting with each position in sentence.
  for (auto const [idx, uni_char]: enum_unicode_chars(params.gd_sentence)) {
    auto const headwords{ find_keywords_starting_with(
      agent,
      trie, //
      params.gd_sentence.substr(idx, max_forward_search_len_bytes)
    ) };

    // set bword to the longest found key in the trie.
    std::string const bword{ headwords.empty() ? std::string{ uni_char } : std::ranges::max(headwords, cmp_len) };
    if (params.gd_word == bword) {
      pos_in_gd_word = static_cast<std::ptrdiff_t>(bword.length());
    } else {
      pos_in_gd_word -= static_cast<std::ptrdiff_t>(uni_char.length());
    }

    std::print(
      R"(<a class="{}" href="bword:{}">{}</a>)",
      (pos_in_gd_word > 0 ? "gd-headword" : "gd-word"),
      bword,
      uni_char
    );
    alternatives.push_back(headwords);
  }

  // Show available entries for other substrings.
  std::println(R"(<div class="alternatives">)");
  for (auto const& group: alternatives | std::views::filter(&JpSet::size)) {
    std::println("<ul>");
    for (auto const& word: group) {
      std::println(
        R"(<li><a class="{}" href="bword:{}">{}</a></li>)",
        (word == params.gd_word ? "gd-headword" : ""),
        word,
        word
      );
    }
    std::println("</ul>"); // close ul
  }
  std::println("</div>"); // close div.alternatives

  std::println("</div>"); // close div.gd-marisa
  std::println("{}", css_style);
}

void marisa_split(std::span<std::string_view const> const args)
{
  try {
    lookup_words(fill_args<marisa_params>(args));
  } catch (gd::help_requested const& ex) {
    std::println(help_text);
  } catch (gd::runtime_error const& ex) {
    std::println("{}", ex.what());
  }
}
