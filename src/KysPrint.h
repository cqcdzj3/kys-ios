#pragma once
// iOS 14 compatibility: libc++ std::print instantiates the floating-point
// formatter even for integer arguments, which pulls in to_chars (iOS 16.3+).
// Use std::format + fwrite instead: it only instantiates the formatter for the
// argument types actually used (integers, strings), which is safe on iOS 14.
#include <cstdio>
#include <format>
#include <utility>

namespace kys
{
template <typename... Args>
inline void print(std::format_string<Args...> fmt, Args&&... args)
{
    const auto text = std::format(fmt, std::forward<Args>(args)...);
    std::fwrite(text.data(), 1, text.size(), stdout);
}

template <typename... Args>
inline void println(std::format_string<Args...> fmt, Args&&... args)
{
    auto text = std::format(fmt, std::forward<Args>(args)...);
    text += '\n';
    std::fwrite(text.data(), 1, text.size(), stdout);
}
}    // namespace kys
