#pragma once

#include "precompiled.h"

namespace gd {
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

} // namespace gd

inline void raise_if(bool expr, std::string_view const message = "Invalid argument.")
{
  if (expr) {
    throw gd::runtime_error(message);
  }
}

template<typename T>
concept PassedParamsStruct = requires(T params) {
  { params.gd_word } -> std::convertible_to<std::string_view>;
  params.assign("a", "b");
};

template<PassedParamsStruct T>
auto fill_args(std::span<std::string_view const> const args) -> T
{
  auto params = T{};
  for (auto it = std::begin(args); it != std::end(args);) {
    if (*it == "--help" or *it == "-h") {
      throw gd::help_requested();
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
    throw gd::help_requested();
  }
  return params;
}

auto determine_card_class(int64_t const card_queue, int64_t const card_type) noexcept -> std::string_view;

template<std::integral Integral>
auto parse_number(std::string_view const s) -> std::optional<Integral>
{
  if (Integral value{}; std::from_chars(s.begin(), s.end(), value).ec == std::errc{})
    return value;
  else
    return std::nullopt;
};

auto is_space(char const ch) noexcept -> bool;

auto strtrim(std::string_view str) noexcept -> std::string;

void str_replace(std::string& str, std::string_view from, std::string_view to) noexcept;

// Getpid
#if __linux__
inline std::string const this_pid{ std::to_string(getpid()) }; // Glibc's getpid
#elif _WIN32
inline std::string const this_pid{ std::to_string(GetCurrentProcessId()) };
#endif

inline auto user_home() -> std::filesystem::path
{
  return std::getenv("HOME");
}

template<typename Stored>
auto join_with(std::vector<Stored> const& seq, std::string_view const sep) -> std::string
{
  std::stringstream ss;
  for (size_t idx = 0; idx != seq.size(); ++idx) {
    ss << seq.at(idx);
    if (idx != seq.size() - 1) {
      ss << sep;
    }
  }
  return ss.str();
}
