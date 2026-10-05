#pragma once
#include "gameplay.hpp"

#define DEMO_TIMER (30 * 60) // 30 seconds

inline void demoInit(void)
{
    gameplayInit();
    gp.demoMode = true;
    gp.demoTimer = 0;

    // very simple auto-demo placement
    Vector2D turretPos[MAX_TURRETS] = {
        Vector2D(WORLD_CENTER_X - 300, 176),
        Vector2D(WORLD_CENTER_X - 100, 176),
        Vector2D(WORLD_CENTER_X + 100, 176),
        Vector2D(WORLD_CENTER_X + 300, 176)
    };

    for (int i = 0; i < MAX_TURRETS; i++)
    {
        gp.turrets[i].worldPos = turretPos[i];
        gp.turrets[i].active = true;
        gp.turrets[i].ai.Reset();
        
        gp.turrets[i].ai.config.fireCadence = 10 + (i * 2); 
    }
}

inline void demoUpdate(void)
{
    if (gp.demoTimer < DEMO_TIMER)
    {
        gp.demoTimer++;
    }
    else
    {
        transitionState(GAME_STATE_UNINITIALIZED);
        return;
    }

    gameplayUpdate();
    
    if (PPE::Core.frame % 16 < 8)
    {
        PPE::Ascii::Print(20, 24, "DEMO!");
    }
    else
    {
        PPE::Ascii::Print(20, 24, "     ");
    }
    
}
   
inline void demoInput(void)
{
    if (PPE::Core.debugInput)
    {
        return;
    }
    
    checkStartButton();
    
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        Player_t* player = &PPE::Player[i];
        
        if (!player->input->isSelected)
        {
            continue;
        }
        
        Digital gamepad(player->input->id);
        
        if (!gp.isPaused && gamepad.WasPressed(Digital::Button::START) && Transition.phase == TRANSITION_STATE_IDLE)
        {
            transitionState(GAME_STATE_TITLE_SCREEN);
        }
    }
}