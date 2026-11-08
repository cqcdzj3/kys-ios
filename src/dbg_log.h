#pragma once
#include <fstream>
#include <string>
#include "GameUtil.h"

// lightweight debug log for field issues (written next to the save data)
inline void kys_dbg(const std::string& line)
{
    std::ofstream f(GameUtil::PATH() + "debug.log", std::ios::app);
    f << line << "\n";
}
