#pragma once

#include "precompiled.h"

class help_requested : public std::invalid_argument
{
public:
  help_requested() : std::invalid_argument("Help requested.") {}
};

class runtime_error : public std::runtime_error
{
public:
  runtime_error(std::string_view const what) : std::runtime_error(std::string{ what }) {}
};

inline void raise_if(bool expr, std::string_view const message = "Invalid argument.")
{
  if (expr) {
    throw runtime_error(message);
  }
}

template<typename T>
concept PassedParamsStruct = requires(T params) {
                               {
                                 params.gd_word
                                 } -> std::convertible_to<std::string_view>;
                               params.assign("a", "b");
                             };

template<PassedParamsStruct T>
auto fill_args(std::span<std::string_view const> const args) -> T
{
  auto params = T{};
  for (auto it = std::begin(args); it != std::end(args);) {
    if (*it == "--help" or *it == "-h") {
      throw help_requested();
    }
    std::ptrdiff_t advance = 1;
    std::string_view value = (std::next(it) != std::end(args)) ? *std::next(it) : "";

    if (it->starts_with("-")) {
      // Expect next arg to be the value.
      raise_if(value.empty() or value.starts_with("-"));
      advance = 2;
    }

    params.assign(*it, value);
    it += advance;
  }
  if (params.gd_word.empty()) {
    throw help_requested();
  }
  return params;
}

auto determine_card_class(int64_t const card_queue, int64_t const card_type) -> std::string_view;

template<std::integral Integral>
auto parse_number(std::string_view const s) -> std::optional<Integral>
{
  if (Integral value{}; std::from_chars(s.begin(), s.end(), value).ec == std::errc{})
    return value;
  else
    return std::nullopt;
};

auto is_space(char const ch) -> bool;

auto strtrim(std::string_view str) -> std::string;
