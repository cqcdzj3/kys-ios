#include "Application.h"
#include "Engine.h"
#include "GameUtil.h"
#include "McpServer.h"
#if defined(KYS_IOS)
#include <cstdlib>
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
