#include "Application.h"
#include "Engine.h"
#include "GameUtil.h"
#include "McpServer.h"
#if defined(KYS_IOS)
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <string>
#include <system_error>
#endif
#ifndef _WIN32
#include <csignal>
#endif

#include "SDL3/SDL_main.h"

int main(int argc, char* argv[])
{
    bool mcp = false;
    std::string path;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--mcp-stdio") { mcp = true; }
        else if (arg == "--help" || arg == "-h")
        {
            std::puts("Usage: kys [game-directory] [--mcp-stdio]\nMCP mode uses stdin/stdout for JSON-RPC and stderr for game logs. Closing stdin exits.");
            return 0;
        }
        else if (arg.empty() || arg.starts_with("--") || !path.empty())
        {
            std::fprintf(stderr, "Invalid argument: %s\n", arg.c_str());
            return 2;
        }
        else { path = std::move(arg); }
    }
    if (mcp)
    {
#ifndef _WIN32
        std::signal(SIGPIPE, SIG_IGN);
#endif
        if (!McpServer::instance().start())
        {
            std::fputs("Unable to initialize MCP stdio\n", stderr);
            return 1;
        }
    }
#ifdef _WIN32
    if (!mcp) { system("chcp 65001"); }
#endif
#if defined(KYS_IOS)
    if (path.empty())
    {
        const char* home = std::getenv("HOME");
        std::string docs = home ? std::string(home) + "/Documents/" : std::string("./");
        std::string dst = docs + "game";
        std::error_code ec;
        if (!std::filesystem::exists(dst + "/config/kysmod.ini", ec))
        {
            // first run: copy the bundled data into the writable Documents folder
            std::string exe = argv[0] ? argv[0] : "";
            auto slash = exe.find_last_of('/');
            std::string base = slash == std::string::npos ? std::string(".") : exe.substr(0, slash);
            std::filesystem::copy(base + "/game", dst,
                std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, ec);
        }
        path = dst + "/";

        // one-time migration: older data packs ship use_virtual_stick = 0,
        // which leaves touch-only users without any way to open the settings
        std::error_code mec;
        auto cfg = std::filesystem::path(dst) / "config" / "kysmod.ini";
        auto flag = std::filesystem::path(dst) / "config" / "kys_stick_migrate";
        if (std::filesystem::exists(cfg, mec) && !std::filesystem::exists(flag, mec))
        {
            std::ifstream in(cfg);
            std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            in.close();
            const std::string key = "use_virtual_stick = 0";
            if (text.find(key) != std::string::npos)
            {
                text.replace(text.find(key), key.size(), "use_virtual_stick = 1");
                std::ofstream out(cfg, std::ios::trunc);
                out << text;
                out.close();
            }
            std::ofstream f(flag);
            f << "1";
            f.close();
        }

        // one-time migration: the cpp data packs default to the new action
        // battle systems; restore the original turn-based battle
        auto bflag = std::filesystem::path(dst) / "config" / "kys_battle_migrate";
        if (std::filesystem::exists(cfg, mec) && !std::filesystem::exists(bflag, mec))
        {
            std::ifstream bin(cfg);
            std::string btext((std::istreambuf_iterator<char>(bin)), std::istreambuf_iterator<char>());
            bin.close();
            auto replace_value = [&btext](const std::string& key, const std::string& value) {
                auto pos = btext.find(key);
                if (pos == std::string::npos)
                {
                    return;
                }
                auto line_end = btext.find('\n', pos);
                if (line_end == std::string::npos)
                {
                    line_end = btext.size();
                }
                btext.replace(pos, line_end - pos, key + value);
            };
            replace_value("battle_mode = ", "0");
            replace_value("battle_presentation = ", "0");
            std::ofstream bout(cfg, std::ios::trunc);
            bout << btext;
            bout.close();
            std::ofstream bf(bflag);
            bf << "1";
            bf.close();
        }
    }
#endif
    if (!path.empty())
    {
        if (path.back() != '/' && path.back() != '\\') { path += '/'; }
        GameUtil::PATH() = path;
    }
    LOG("Game path is {}\n", GameUtil::PATH());
    Application app;
    return app.run();
}
