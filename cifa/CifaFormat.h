#pragma once
// iOS 14 compatibility: libc++ float std::format/std::print depends on to_chars,
// which is unavailable before iOS 16.3. snprintf-based equivalents live here.
#include <cctype>
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
    // try increasing precision until the text round-trips exactly
    for (int prec = 1; prec <= 17; ++prec)
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

// handle the spec part of "{:spec}" such as "", ".2f", "10.3f", "e", "%"
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
    while (i < spec.size() && (std::isdigit(static_cast<unsigned char>(spec[i])) || spec[i] == '-' || spec[i] == '+'))
    {
        width += spec[i++];
    }
    std::string prec;
    if (i < spec.size() && spec[i] == '.')
    {
        ++i;
        prec = ".";
        while (i < spec.size() && std::isdigit(static_cast<unsigned char>(spec[i])))
        {
            prec += spec[i++];
        }
    }
    char type = 'f';
    bool percent = false;
    while (i < spec.size())
    {
        char c = spec[i++];
        if (c == 'f' || c == 'F' || c == 'e' || c == 'E' || c == 'g' || c == 'G')
        {
            type = c;
        }
        else if (c == '%')
        {
            percent = true;
        }
    }

    std::string fmt = "%" + width + prec;
    fmt += type;
    char buf[512];
    std::snprintf(buf, sizeof(buf), fmt.c_str(), v);
    std::string out = buf;
    if (percent)
    {
        out += "%";
    }
    return out;
}
}    // namespace cifa_fmt
