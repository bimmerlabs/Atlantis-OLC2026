#pragma once
#include "../core/core.hpp"
#if ENABLE_DEBUG_MODE == 1
    #include "../debug/debug_menu.hpp"
#endif
#include "game.hpp"
#include "../display/display.hpp"

    bool displayStats = true;
    

inline MenuItem_t backgroundItems[] = {    
    { .label = "WaterLevel:   ", .type = MENU_ITEM_UINT, .action = nullptr, .uintVal = { &gp.waterLevel, 8, 240, 2 } },
    { .label = "Back          ", .type = MENU_ITEM_BACK, .action = nullptr, .boolVal = nullptr },
};

inline Menu_t backgroundMenu = {
    "Bg Effects:", backgroundItems, sizeof(backgroundItems) / sizeof(backgroundItems[0]), 0, nullptr
};

inline void exitGameMenu(void)
{
    Pcm::Play(Sounds.Core[CancelSnd], PlayMode::Volatile, 6);
    saveGameBackup();
    gp.isPaused = false;
}

inline void quitGame(void)
{
    Pcm::Play(Sounds.Core[CancelSnd], PlayMode::Volatile, 6);
    saveGameBackup();
    gp.Reset();
    beginTransitionOut();
    transitionState(GAME_STATE_TITLE_SCREEN);
}

inline MenuItem_t gameItems[] = {
    { .label = "Resume        ", .type = MENU_ITEM_ACTION,  .action  = &exitGameMenu, .boolVal = nullptr },
    // { .label = "Turret Offset:", .type = MENU_ITEM_FXP,  .action = nullptr, .fxpVal = { &gp.turretFiringOffset, Fxp(-64), Fxp(64), Fxp(1) } },
    #if ENABLE_DEBUG_MODE == 1
    { .label = "Display Stats:", .type = MENU_ITEM_BOOL, .action = nullptr, .boolVal = &gp.displayStats },
    { .label = "Bg Effects    ", .type = MENU_ITEM_SUBMENU, .action = nullptr, .submenu = &backgroundMenu },
    { .label = "Debug Options ", .type = MENU_ITEM_SUBMENU, .action = nullptr, .submenu = &debugMenu },
    #endif
    { .label = "Quit          ", .type = MENU_ITEM_ACTION,  .action  = &quitGame, .boolVal = nullptr },
};

inline Menu_t gameMenu = {
    "Paused:", gameItems, sizeof(gameItems) / sizeof(gameItems[0]), 0, nullptr
};
