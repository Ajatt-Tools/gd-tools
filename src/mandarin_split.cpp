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

#include "mandarin_split.h"
#include "mecab_html.h"
#include "mecab_split.h"
#include "split_html.h"
#include "util.h"

static constexpr std::string_view help_text = R"EOF(usage: gd-mandarin [OPTIONS]

Echo a Mandarin sentence as clickable traditional Chinese words.

OPTIONS
  --word %GDWORD%        required word to highlight
  --sentence %GDSEARCH%  sentence (defaults to --word)
  --dic-dir PATH        Mandarin dictionary directory (default: ~/.local/gd-mandarin)
  --user-dict PATH      user dictionary (default: user.dic in the dictionary directory)
  --font-size SIZE      simple CSS size, e.g. 30px or 2rem (default: 2rem)
  -h,--help             print this help screen
)EOF";

auto verify_mecab_dictionary_installation(mecab_params const& params) -> void
{
  for (std::string_view const file_name: { "dicrc", "sys.dic", "unk.dic", "char.bin", "matrix.bin" }) {
    raise_if(
      not std::filesystem::is_regular_file(params.dic_dir / file_name),
      "Mandarin dictionary is not installed or is incomplete. Install it with ./scripts/mandarin_installer.sh "
      "from the gd-tools source directory, or use --dic-dir PATH. "
      "Instructions: https://github.com/Ajatt-Tools/gd-tools#gd-mandarin"
    );
  }
}

static auto create_mecab_args(mecab_params params) -> std::vector<std::string>
{
  std::vector<std::string> args{
    "arg0",
    "--dicdir=" + params.dic_dir.string(),
    "--userdic=" + params.user_dict.string(),
  };

  // Feature 5 contains the traditional Chinese form.
  // Extract it through MeCab's formatter, then construct escaped HTML in C++.
  args.append_range(mecab_output_arguments(mecab_dictionary_kind::mandarin));
  return args;
}

static auto lookup_words(mecab_params params) -> std::string
{
  raise_if(params.gd_word.empty(), "Not enough parameters: --word is required.");
  if (params.gd_sentence.empty()) {
    params.gd_sentence = params.gd_word;
  }
  verify_mecab_dictionary_installation(params);
  if (params.user_dict.empty()) {
    params.user_dict = params.dic_dir / "user.dic";
  }
  raise_if(
    not std::filesystem::is_regular_file(params.user_dict),
    "Provided user dictionary doesn't exist or isn't a file."
  );
  validate_font_size(params.font_size);

  std::erase_if(params.gd_word, is_space);
  std::erase_if(params.gd_sentence, is_space);
  validate_mecab_input(params.gd_sentence);

  auto const args = create_mecab_args(params);

  auto const tagger = create_mecab_tagger(args);
  auto const* parsed = tagger->parse(params.gd_sentence.c_str());
  raise_if(parsed == nullptr, "Failed to parse Mandarin sentence.");

  auto const result = render_mecab_output(parsed, params.gd_word, mecab_dictionary_kind::mandarin);
  return render_split_html(result, params.font_size) + format_mecab_debug_info(params, args);
}

auto render_mandarin(std::span<std::string_view const> const args) -> std::string
{
  return lookup_words(fill_args<mecab_params>(args, { .dic_dir = user_home() / ".local/gd-mandarin" }));
}

auto mandarin_split(std::span<std::string_view const> const args) -> void
{
  try {
    std::println("{}", render_mandarin(args));
  } catch (gd::help_requested const&) {
    std::println("{}", help_text);
  }
}
