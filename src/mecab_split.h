#pragma once

#include "precompiled.h"
#include "util.h"

/** Shared options with dictionary defaults supplied by each command. */
struct mecab_params {
  std::string gd_word{};
  std::string gd_sentence{};
  std::filesystem::path user_dict{};
  std::filesystem::path dic_dir{};
  std::string font_size{ "2rem" };

  auto assign(std::string_view const key, std::string_view const value) -> void {
    if (key == "--word") {
      gd_word = value;
    } else if (key == "--sentence") {
      gd_sentence = value;
    } else if (key == "--user-dict") {
      user_dict = value;
    } else if (key == "--dic-dir") {
      dic_dir = value;
    } else if (key == "--font-size") {
      font_size = value;
    } else {
      throw gd::runtime_error(std::format("Unknown argument name: {}", key));
    }
  }
};
auto mecab_split(std::span<std::string_view const> const args) -> void;
/** Locate an installed Japanese MeCab dictionary directory. */
auto find_dic_dir() -> std::filesystem::path;
/** Return the MeCab command's HTML without printing it. */
auto render_mecab(std::span<std::string_view const> args) -> std::string;
/** Create a tagger using owned mutable argument buffers, without casting away constness. */
auto create_mecab_tagger(std::vector<std::string> arguments) -> std::unique_ptr<MeCab::Tagger>;
auto replace_all(std::string str, std::string_view const from, std::string_view const to) -> std::string;
auto format_mecab_debug_info(mecab_params const& params, std::vector<std::string> const& args) -> std::string;
