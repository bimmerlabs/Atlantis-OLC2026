#pragma once
#include "../core/core.hpp"
#if ENABLE_DEBUG_MODE == 1
    #include "../debug/debug_menu.hpp"
#endif
#include "../display/particles/particlefx.hpp"

// this is just a placeholder.  I used to have options.hpp - 
// maybe that is game_options.hpp, and it has this stuff in a struct
inline int titleGameMode   = 0;
inline int titlePlayers    = 1;
inline int titleDifficulty = 1;

inline const char* titleGameModeLabels[]   = {
                                                "Arcade     ",
                                                "Story      ",
                                                "Time Attack" 
                                               };
inline const char* titleDifficultyLabels[] = {
                                                "Easy  ",
                                                "Normal",
                                                "Hard  "
                                               };

// menu actions
inline void musicTest(void)
{
    Audio::PlayCDTrack(Audio::Settings.currentTrack, false);
}

inline void soundTest(void)
{
    Pcm::Play(Audio::Settings.currentPcm, PlayMode::Volatile, 7);
}

inline void enterDemoMode(void)
{    
    fadeBubblesFx();
    Pcm::Play(Sounds.Core[NextSnd], PlayMode::Volatile, 6);
    beginTransitionOut();
    transitionState(GAME_STATE_DEMO_LOOP);
}

inline void enterCredits(void)
{    
    fadeBubblesFx();
    Pcm::Play(Sounds.Core[NextSnd], PlayMode::Volatile, 6);
    beginTransitionOut();
    transitionState(GAME_STATE_CREDITS);
}

inline void startGame(void)
{    
    fadeBubblesFx();
    Pcm::Play(Sounds.Core[NextSnd], PlayMode::Volatile, 6);
    beginTransitionOut();
    transitionState(GAME_STATE_GAMEPLAY);
}

inline void exitTitleMenu(void)
{
    Pcm::Play(Sounds.Core[CancelSnd], PlayMode::Volatile, 6);
    saveGameBackup();
    changeState(GAME_STATE_TITLE_SCREEN);
}

MenuItem_t inputItems[] = {
    { .label = "Back          ", .type = MENU_ITEM_BACK,    .action = nullptr, .boolVal = nullptr },
};

Menu_t inputMenu = { "Input Settings:", inputItems, sizeof(inputItems)/sizeof(inputItems[0]), 0, nullptr };

MenuItem_t soundItems[] = {
    { .label = "Music Volume: ", .type = MENU_ITEM_INT,  .action = &Audio::SetVolume, .intVal = { &Audio::Settings.masterVolume, 0, Audio::MaxVolume, 1 } },
    { .label = "Music Test:   ", .type = MENU_ITEM_CHOICE, .action = &musicTest, .choiceVal = { &Audio::Settings.currentTrack, musicTrackLabels, LOGO_TRACK, LAST_TRACK-1 } },
    { .label = "Sound Test:   ", .type = MENU_ITEM_INT,  .action = &soundTest, .intVal = { &Audio::Settings.currentPcm, BackSnd, CORE_SND_MAX-1, 1 } },
    { .label = "Back          ", .type = MENU_ITEM_BACK, .action = nullptr, .boolVal = nullptr },
};

Menu_t soundMenu = { "Audio Test:", soundItems, sizeof(soundItems)/sizeof(soundItems[0]), 0, nullptr };


MenuItem_t optionsItems[] = {
    #if ENABLE_DEBUG_MODE == 1
    { .label = "Debug Options ", .type = MENU_ITEM_SUBMENU, .action = nullptr, .submenu = &debugMenu },
    { .label = "Input Settings", .type = MENU_ITEM_SUBMENU, .action = nullptr, .submenu = &inputMenu },
    #endif
    { .label = "Sound Test    ", .type = MENU_ITEM_SUBMENU, .action = nullptr, .submenu = &soundMenu },
    { .label = "Demo Mode:    ", .type = MENU_ITEM_ACTION,  .action    = &enterDemoMode, .boolVal = nullptr },
    { .label = "Credits:      ", .type = MENU_ITEM_ACTION,  .action    = &enterCredits,  .boolVal = nullptr },
    { .label = "Back          ", .type = MENU_ITEM_BACK,    .action = nullptr, .boolVal = nullptr },
};

Menu_t optionsMenu = { "Options:", optionsItems, sizeof(optionsItems)/sizeof(optionsItems[0]), 0, nullptr };

inline MenuItem_t titleItems[] = {
    { .label = "Start:       ", .type = MENU_ITEM_ACTION,  .action    = &startGame, .boolVal = nullptr },
    // { .label = "Game Mode:   ", .type = MENU_ITEM_CHOICE,  .choiceVal = { &titleGameMode, titleGameModeLabels, 0, 3 } },
    // { .label = "Players:     ", .type = MENU_ITEM_INT,     .intVal    = { &titlePlayers, 1, 4, 1 } },
    // { .label = "Difficulty:  ", .type = MENU_ITEM_CHOICE,  .action = nullptr, .choiceVal = { &Game.difficulty, titleDifficultyLabels, GAME_DIFFICULTY_EASY, GAME_DIFFICULTY_MAX } },
    { .label = "Options      ", .type = MENU_ITEM_SUBMENU, .action = nullptr, .submenu   = &optionsMenu },
    { .label = "Exit         ", .type = MENU_ITEM_ACTION,  .action  = &exitTitleMenu, .boolVal = nullptr },
};

inline Menu_t titleMenu = { "Title", titleItems, sizeof(titleItems) / sizeof(titleItems[0]), 0, nullptr };

inline void titleMenu_init(void)
{
    PPE::MenuSystem.init(&titleMenu);
}

inline void titleMenu_input(void)
{
    if (Core.debugInput)
    {
        return;
    }
    
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        Player_t* player = &Player[i];
        
        if (!player->input->isSelected)
        {
            continue;
        }
        
        if (!Management::IsConnected(i)) {
            continue;
        }
        
        PPE::MenuSystem.input(&Input[player->input->id], &exitTitleMenu);
    }
}

inline void titleMenu_update(void)
{
    titleScroll.update();
    drawBubblesFx();
    
    if (Core.debugInput)
    {
        return;
    }
    
    PPE::MenuSystem.draw(3, 8);
    
    if (PPE::CoreOptions.debugDisplay)
    {
        PPE::Ascii::Print(2, 2, "MenuDepth: %d", PPE::MenuSystem.depth);
    }
}