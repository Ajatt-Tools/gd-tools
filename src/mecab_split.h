#pragma once

#include "precompiled.h"

auto mecab_split(std::span<std::string_view const> const args) -> void;
auto replace_all(std::string str, std::string_view const from, std::string_view const to) -> std::string;
