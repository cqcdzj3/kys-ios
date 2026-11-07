#pragma once
#include <fmt/format.h>
#include "fmt_compat.h"
#include <string>

// 运行时格式串包装（std::vformat 的 fmt 等价物）
struct runtime_format
{
    std::string fmt;
};

template <typename T>
concept is_printable = requires { fmt::formatter<std::remove_cvref_t<T>>(); };

template <is_printable... Args>
std::string format(const runtime_format& rfmt, Args&&... args)
{
    return fmt::vformat(rfmt.fmt, fmt::make_format_args(args...));
}
