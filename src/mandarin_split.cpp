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

// Keep Mandarin-specific validation and preparation separate from the shared MeCab API.
namespace {

constexpr std::string_view help_text = R"EOF(usage: gd-mandarin [OPTIONS]

Echo a Mandarin sentence as clickable traditional Chinese words.

OPTIONS
  --word %GDWORD%        required word to highlight
  --sentence %GDSEARCH%  sentence (defaults to --word)
  --dic-dir PATH        Mandarin dictionary directory (default: ~/.local/gd-mandarin)
  --user-dict PATH      user dictionary (default: user.dic in the dictionary directory)
  --font-size SIZE      simple CSS size, e.g. 30px or 2rem (default: 2rem)
  -h,--help             print this help screen
)EOF";

/** Require all Mandarin dictionary files before initializing the tagger. */
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

/** Resolve the mandatory Mandarin user dictionary independently of Japanese defaults. */
auto mandarin_user_dictionary(mecab_params const& params) -> std::filesystem::path
{
  auto const dictionary = params.user_dict.empty() ? params.dic_dir / "user.dic" : params.user_dict;
  raise_if(
    not std::filesystem::is_regular_file(dictionary),
    "Provided user dictionary doesn't exist or isn't a file."
  );
  return dictionary;
}

/** Validate Mandarin dictionaries and normalized input before invoking the shared parsing pipeline. */
auto lookup_words(mecab_params params) -> std::string
{
  raise_if(params.gd_word.empty(), "Not enough parameters: --word is required.");
  if (params.gd_sentence.empty()) {
    params.gd_sentence = params.gd_word;
  }
  verify_mecab_dictionary_installation(params);
  params.user_dict = mandarin_user_dictionary(params);
  validate_font_size(params.font_size);

  std::erase_if(params.gd_word, is_space);
  std::erase_if(params.gd_sentence, is_space);
  validate_mecab_input(params.gd_sentence);

  return render_mecab_sentence(params, mecab_dictionary_kind::mandarin);
}

} // namespace

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
