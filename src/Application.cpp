#include "Application.h"
#include "Audio.h"
#include "DayNightSystem.h"
#include "Engine.h"
#include "Event.h"
#include "Font.h"
#include "GameUtil.h"
#include "TextureManager.h"
#include "TitleScene.h"
#include "Types.h"
#include "UIKeyConfig.h"
#include <algorithm>

Application::Application()
{
}

Application::~Application()
{
}

int Application::run()
{
#ifdef __ANDROID__
    // 首次运行时将 assets/game.zip 解压到默认游戏目录，引擎初始化前完成。
    Engine::extractAssetsIfNeeded();
    GameUtil::selectAndroidGamePath();
#endif
    static constexpr int UI_WIDTH = 1280;
    static constexpr int UI_HEIGHT = 720;

    auto game = GameUtil::getInstance();
    renderer_ = game->getString("game", "renderer", "");
    title_ = game->getString("game", "title", "All Heroes in Kam Yung Stories");
    int fullscreen = game->getInt("game", "fullscreen", 0);

    auto engine = Engine::getInstance();
    engine->setUISize(UI_WIDTH, UI_HEIGHT);
    engine->init(nullptr, 0, 0, renderer_, fullscreen);
    engine->setWindowTitle(title_);
    engine->addEventWatch([](void*, EngineEvent* e) -> bool
        {
            if (e->type == EVENT_QUIT)
            {
                //退出游戏
                return false;
            }
            if (e->type == EVENT_DID_ENTER_BACKGROUND)
            {
                //暂停音频
                Audio::getInstance()->pauseMusic();
            }
            if (e->type == EVENT_DID_ENTER_FOREGROUND)
            {
                //恢复音频
                Audio::getInstance()->resumeMusic();
            }
            return true;
        },
        nullptr);

    //引擎初始化之后才能创建纹理
    int render_width = (std::max)(1, game->getInt("game", "render_width", UI_WIDTH));
    int render_height = (std::max)(1, game->getInt("game", "render_height", UI_HEIGHT));
    engine->createAssistTexture("scene", render_width, render_height);

    config();

    RunNode::addIntoDrawTop(DayNightSystem::getInstance());

    // 标题场景可能因为残留的场景栈被意外结束（例如战败后 exitAll 误伤），
    // 这里循环重建，保证任何情况下都能回到标题而不是黑屏卡死
    while (true)
    {
        RunNode::clearRoot();
        auto t0 = Engine::getInstance()->getTicks();
        auto s = std::make_shared<TitleScene>();
        s->run();
        if (Engine::getInstance()->getTicks() - t0 < 200)
        {
            SDL_Delay(200);    // 避免异常情况下空转
        }
    }

    return 0;
}

void Application::config()
{
    auto game = GameUtil::getInstance();
    //RunNode::setRefreshInterval(game->getReal("game", "refresh_interval", 16));
    Audio::getInstance()->setVolume(game->getInt("music", "volume", 50));
    Audio::getInstance()->setVolumeWav(game->getInt("music", "volumewav", 50));
    Event::getInstance()->setUseScript(game->getInt("game", "use_script", 1));
    Font::getInstance()->setStatMessage(game->getInt("game", "stat_font", 0));
    Font::getInstance()->setSimplified(game->getInt("game", "simplified_chinese", 1));
    TextureManager::getInstance()->setLoadFromPath(game->getInt("game", "png_from_path", 0));
    TextureManager::getInstance()->setLoadAll(game->getInt("game", "load_all_png", 0));
    UIKeyConfig::readFromString(game->getString("game", "key", ""));
    Scene::setKeyWalkDealy(game->getInt("game", "walk_speed", 20));
    RunNode::setUseVirtualStick(game->getInt("game", "use_virtual_stick", 0));
    Scene::setTileScale(game->getInt("game", "tile_scale", 1));
#if defined(__ANDROID__) || defined(KYS_IOS)
    RunNode::setUseVirtualStick(game->getInt("game", "use_virtual_stick", 1));
#endif
    RunNode ::setRenderMessage(game->getInt("game", "render_message", 0));
    Save::getInstance()->setZipSave(game->getInt("game", "zip_save", 1));

    Role::setMaxValue();
    Role::setLevelUpList();
    Item::setSpecialItems();
}
