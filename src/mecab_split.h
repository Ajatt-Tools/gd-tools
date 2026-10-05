#pragma once

#include "precompiled.h"

auto mecab_split(std::span<std::string_view const> const args) -> void;
/** Locate an installed Japanese MeCab dictionary directory. */
auto find_dic_dir() -> std::filesystem::path;
/** Return the MeCab command's HTML without printing it. */
auto render_mecab(std::span<std::string_view const> args) -> std::string;
/** Create a tagger using owned mutable argument buffers, without casting away constness. */
auto create_mecab_tagger(std::vector<std::string> arguments) -> std::unique_ptr<MeCab::Tagger>;
auto replace_all(std::string str, std::string_view const from, std::string_view const to) -> std::string;
