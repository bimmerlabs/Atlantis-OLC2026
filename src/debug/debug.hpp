#pragma once
#include "../core/core.hpp"
#include "../game/game_includes.hpp"
#include "debug_menu.hpp"

typedef enum
{
    DEBUG_STATE_UNINITIALIZED = GAME_STATE_MAX, // Required!
    GAME_STATE_COLLISION,
    GAME_STATE_INPUT,
    GAME_STATE_SPRITES,
    DEBUG_STATE_MAX, // Required - there always needs to be the "max" state
} DEBUG_STATE;

inline void debugText(void)
{
    if (!PPE::CoreOptions.debugDisplay || gp.isPaused)
        return;

    PPE::Ascii::Print(2, 2,  "State:%02d", PPE::Core.gameState);
    PPE::Ascii::Print(12, 2, "Last:%02d", PPE::Core.lastState);
    PPE::Ascii::Print(20, 2, "New:%02d", PPE::Core.newState);
    PPE::Ascii::Print(28, 2, "Next:%02d", PPE::Core.nextState);

    PPE::Ascii::Print(2, 3, "Frame:%03d", PPE::Core.frame);
    PPE::Ascii::Print(2, 4, "FPS:%f  ", PPE::Core.fps);

    PPE::Ascii::Print(2, 6, "Transition:%s     ", transitionMessage[Transition.phase]);  // not sure where transitionMessage went
    PPE::Ascii::Print(2, 7, "isLoading:%d", PPE::Core.isLoading);
    PPE::Ascii::Print(20, 7, "transitionUpdate:%d", screenTransition_update());

    // PPE::Ascii::Print(2, 6, "MasterVolume:%03d", PPE::Audio.masterVolume);
    // PPE::Ascii::Print(2, 7, "cdIsPlaying:%d", PPE::Audio.cdIsPlaying);

    PPE::Ascii::Print(20, 3, "Sprites:%d    ", SRL::VDP1::GetTextureCount());
    PPE::Ascii::Print(20, 4, "VDP1  free:%06d", SRL::VDP1::GetAvailableMemory());
    PPE::Ascii::Print(20, 5, "HWRAM free:%06d", SRL::Memory::HighWorkRam::GetFreeSpace());
    PPE::Ascii::Print(20, 6, "HWRAM used:%06d", SRL::Memory::HighWorkRam::GetUsedSpace());

    switch (PPE::Core.gameState)
    {
        case GAME_STATE_LOGO:
            PPE::Ascii::Print(2, 12, "LogoTimer:%d  ", LogoTimer);
            break;
        case GAME_STATE_TITLE_SCREEN:
            PPE::Ascii::Print(2, 12, "TitleTimer:%d  ", ts.TitleTimer);
            PPE::Ascii::Print(2, 13, "angle:%d  ", ts.angle);
            PPE::Ascii::Print(2, 14, "logoScale:%f  ", ts.logoScale);
            break;
        case GAME_STATE_GAMEPLAY:
        {           
            
            break;
        }
        case GAME_STATE_DEMO_LOOP:
        {
            PPE::Ascii::Print(2, 12, "DemoTimer:%d  ", gp.demoTimer);               
            break;
        }
        default:
            break;
    }
}

// only needed for VDP2 fonts
// #if ENABLE_DEBUG_MODE == 1
// inline bool debugDisplayState = PPE::CoreOptions.debugDisplay;

// void debugCallback(void)
// {
    // // detect if debug display state changed, clear screen if true
    // if (debugDisplayState != PPE::CoreOptions.debugDisplay)
    // {
        // debugDisplayState = PPE::CoreOptions.debugDisplay;
    // }
// }
// #endif