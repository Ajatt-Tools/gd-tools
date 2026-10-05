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

#include "mecab_split.h"
#include "kana_conv.h"
#include "precompiled.h"
#include "split_html.h"
#include "util.h"

static constexpr std::string_view help_text = R"EOF(usage: gd-mecab [OPTIONS]

Echo input back to GoldenDict as HTML with sentence split into parts.

OPTIONS
  --word %GDWORD%        required word
  --sentence %GDSEARCH%  required sentence
  --user-dict PATH       path to the user dictionary.
  --font-size SIZE       simple CSS size, e.g. 30px or 2rem (default: 2rem).
)EOF";

auto find_file_recursive(
  std::vector<std::filesystem::path> possible_dirs, //
  std::string_view const file_name
) -> std::filesystem::path
{
  for (size_t idx = 0; idx < possible_dirs.size(); ++idx) {
    if (!std::filesystem::is_directory(possible_dirs[idx])) {
      continue;
    }
    for (auto const& dir_entry: std::filesystem::directory_iterator(possible_dirs.at(idx))) {
      if (dir_entry.is_regular_file() and dir_entry.path().filename() == file_name) {
        return dir_entry.path();
      } else if (dir_entry.is_directory()) {
        possible_dirs.push_back(dir_entry);
      }
    }
  }
  return "";
}

auto find_user_dict_file() -> std::filesystem::path
{
  static std::vector<std::filesystem::path> const possible_dirs = {
    "/usr/share/gd-tools",
    user_home() / ".local/share/gd-tools",
    user_home() / ".local/share/Anki2/addons21",
  };
  return find_file_recursive(possible_dirs, "user_dic.dic");
}

/** Locate the first available MeCab dictionary in the supported system and user paths. */
auto find_dic_dir() -> std::filesystem::path
{
  static std::vector<std::filesystem::path> const possible_dirs = {
    "/usr/lib/mecab/dic/mecab-ipadic-neologd", // neologd is preferred if available
    "/var/lib/mecab/dic/debian", // active dictionary on Debian-based distributions
    "/usr/lib/mecab/dic",
    "/usr/lib64/mecab/dic",
    user_home() / ".local/share/Anki2/addons21",
  };
  // parent path of an empty entry returns itself.
  return find_file_recursive(possible_dirs, "dicrc").parent_path();
}

struct mecab_params
{
  std::string gd_word{};
  std::string gd_sentence{};
  std::filesystem::path user_dict{ find_user_dict_file() };
  std::filesystem::path dic_dir{ find_dic_dir() };
  std::string font_size{ "2rem" };

  auto assign(std::string_view const key, std::string_view const value) -> void
  {
    if (key == "--word") {
      gd_word = value;
    } else if (key == "--sentence") {
      gd_sentence = value;
    } else if (key == "--user-dict") {
      user_dict = value;
    } else if (key == "--font-size") {
      font_size = value;
    } else {
      throw gd::runtime_error(std::string(std::format("Unknown argument name: {}", key)));
    }
  }
};

auto replace_all(std::string str, std::string_view const from, std::string_view const to) -> std::string
{
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return str;
}

auto create_mecab_tagger(std::vector<std::string> arguments) -> std::unique_ptr<MeCab::Tagger>
{
  std::vector<char*> argv;
  argv.reserve(arguments.size());
  for (auto& argument: arguments) { argv.push_back(argument.data()); }
  std::unique_ptr<MeCab::Tagger> tagger{ MeCab::createTagger(static_cast<int>(argv.size()), argv.data()) };
  raise_if(tagger == nullptr, "Failed to initialize MeCab tagger.");
  return tagger;
}

static auto format_mecab_debug_info(mecab_params const& params, std::vector<std::string> const& args) -> std::string
{
  static constexpr std::string_view debug_template = R"EOF(
<div style="display: none;">
dicdir: {}
userdic: {}
mecab args: [{}]
</div>
)EOF";
  return std::format(debug_template, params.dic_dir.string(), params.user_dict.string(), join_with(args, ", "));
}

auto lookup_words(mecab_params params) -> std::string
{
  validate_font_size(params.font_size);
  half_to_full(params.gd_word);
  std::erase_if(params.gd_word, is_space);
  std::erase_if(params.gd_sentence, is_space);

  if (params.gd_sentence.empty()) {
    params.gd_sentence = params.gd_word;
  } else {
    half_to_full(params.gd_sentence);
  }

  raise_if((not std::filesystem::is_directory(params.dic_dir)), "Couldn't find dictionary directory.");

  auto dicdir = "--dicdir=" + params.dic_dir.string();

  std::vector<std::string> args = {
    "arg0", // unused
    dicdir, // dicdir
    R"EOF(--node-format=<a href="bword:%f[6]" title="%f[6]">%m</a>)EOF", //
    R"EOF(--unk-format=<a href="bword:%m" title="%m">%m</a>)EOF", //
    "--eos-format=<br>",
  };

  auto userdic = params.user_dict.string();
  if (std::filesystem::is_regular_file(params.user_dict)) {
    // user dict can be omitted from params.
    userdic = "--userdic=" + userdic;
    args.push_back(userdic);
  }

  auto const tagger = create_mecab_tagger(args);
  // parse() returns null on failure.
  auto const* parsed = tagger->parse(params.gd_sentence.c_str());
  raise_if(parsed == nullptr, "Failed to parse MeCab sentence.");
  std::string result = parsed;
  result = replace_all(result, std::format(">{}<", params.gd_word), std::format("><b>{}</b><", params.gd_word));
  // Debug info is not shown in GoldenDict.
  return render_split_html(result, params.font_size) + format_mecab_debug_info(params, args);
}

auto render_mecab(std::span<std::string_view const> const args) -> std::string
{ return lookup_words(fill_args<mecab_params>(args)); }

void mecab_split(std::span<std::string_view const> const args)
{
  try {
    std::println("{}", render_mecab(args));
  } catch (gd::help_requested const& ex) {
    std::println(help_text);
  }
}
