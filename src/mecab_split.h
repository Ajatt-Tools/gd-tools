#pragma once

#include "precompiled.h"

auto mecab_split(std::span<std::string_view const> const args) -> void;
/** Create a tagger using owned mutable argument buffers, without casting away constness. */
auto create_mecab_tagger(std::vector<std::string> arguments) -> std::unique_ptr<MeCab::Tagger>;
auto replace_all(std::string str, std::string_view const from, std::string_view const to) -> std::string;
