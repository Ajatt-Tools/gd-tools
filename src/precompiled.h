// STL
#include <algorithm>
#include <cassert>
#include <charconv>
#include <chrono>
#include <concepts>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>
#include <ranges>
#include <regex>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

// Getpid
#if __linux__
#include <unistd.h> // Glibc's getpid
#elif _WIN32
#include <windows.h> // GetCurrentProcessId
#endif

// Other
#include <cpr/cpr.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <marisa/trie.h>
#include <nlohmann/json.hpp>
#include <rdricpp/rdricpp.h>
