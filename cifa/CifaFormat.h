#pragma once
// iOS 14 鍏煎锛歭ibc++ 鐨勬诞鐐?std::format/std::print 渚濊禆 to_chars锛?// 鍚庤€呭湪 iOS < 16.3 涓嶅彲鐢ㄣ€傝繖閲岀敤 snprintf 瀹炵幇绛変环鐨勬渶灏忓姛鑳姐€?#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace cifa_fmt
{
inline std::string shortest(double v)
{
    if (std::isnan(v))
    {
        return "nan";
    }
    if (std::isinf(v))
    {
        return v > 0 ? "inf" : "-inf";
    }
    char buf[64];
    // 浠庢渶鐭湁鏁堜綅寮€濮嬭瘯锛屼繚璇佽鍥炴潵鏄竴鏍风殑鍊硷紙杩戜技 std::format("{}")锛?    for (int prec = 1; prec <= 17; ++prec)
    {
        std::snprintf(buf, sizeof(buf), "%.*g", prec, v);
        if (std::strtod(buf, nullptr) == v)
        {
            return buf;
        }
    }
    std::snprintf(buf, sizeof(buf), "%.17g", v);
    return buf;
}

// 澶勭悊 "{:spec}" 閲岀殑 spec锛屼緥濡?"", ".2f", "10.3f", "e", "%"
inline std::string with_spec(const std::string& spec, double v)
{
    if (spec.empty())
    {
        return shortest(v);
    }
    if (std::isnan(v))
    {
        return "nan";
    }
    if (std::isinf(v))
    {
        return v > 0 ? "inf" : "-inf";
    }

    size_t i = 0;
    std::string width;
    while (i < spec.size() && (std::isdigit((unsigned char)spec[i]) || spec[i] == '-' || spec[i] == '+'))
    {
        width += spec[i++];
    }
    std::string prec;
    if (i < spec.size() && spec[i] == '.')
    {
        ++i;
        prec = ".";
        while (i < spec.size() && std::isdigit((unsigned char)spec[i]))
        {
            prec += spec[i++];
        }
    }
    char type = 'f';
    while (i < spec.size())
    {
        char c = spec[i++];
        if (c == 'f' || c == 'F' || c == 'e' || c == 'E' || c == 'g' || c == 'G')
        {
            type = c;
        }
        else if (c == '%')
        {
            type = 'f';
        }
    }

    std::string fmt = "%" + width + prec;
    fmt += type;
    char buf[512];
    std::snprintf(buf, sizeof(buf), fmt.c_str(), v);
    std::string out = buf;
    if (spec.find('%') != std::string::npos)
    {
        out += "%";
    }
    return out;
}
}    // namespace cifa_fmt

