#pragma once
#include "../core/core.hpp"
#include "../display/display.hpp"
#include "../display/vdp2/linescroll.hpp"
#include "../display/particles/particlefx.hpp"

#define CREDITS_TIMER (30*60)

struct Credits_t
{
    unsigned int Timer = 0;  
    void Reset() { *this = Credits_t{}; }
};

static Credits_t credits = {};

inline void creditsInit(void)
{
    PPE::Audio::ResetVolume();
    PPE::Audio::PlayCDTrack(BALLS_TRACK, true);
    
    credits.Reset();
    if (PPE::Core.lastState != GAME_STATE_TITLE_MENU)
    {
        nbg1.Reset();
        
        // linescroll effect
        titleScroll.initializeNbg1();
        titleScroll.setWaterHeight(240);
       
        initBubblesFx();
    }
}

inline void creditsInput(void)
{
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        Player_t* player = &PPE::Player[i];
        if (!player->input->isSelected) continue;

        Input_t* input = &Input[player->input->id];
        Digital gamepad(input->id);

        if (gamepad.WasPressed(Digital::Button::START))
        {
            transitionState(GAME_STATE_LOGO);
        }
    }
}

inline void creditsUpdate(void)
{
    
    if (PPE::Core.lastState != GAME_STATE_TITLE_MENU)
    {
        credits.Timer++;
        if (credits.Timer >= CREDITS_TIMER)
            transitionState(GAME_STATE_LOGO);
    }
        
    titleScroll.update();
    drawBubblesFx();
    
    PPE::Ascii::PrintWrapped(0, 2, 32, "Atlantis Credits!", PPE::Ascii::Align::CenterX);
    
    PPE::Ascii::PrintWrapped(0, 4, 32, "Saturn Ring Library:", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 5, 32, "reyeme (srl.reye.me)", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 7, 32, "Pixel Poppy Engine:", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 8, 32, "hassmaschine", PPE::Ascii::Align::CenterX);
    
    PPE::Ascii::PrintWrapped(0, 10, 32, "Programming:", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 11, 32, "hassmaschine", PPE::Ascii::Align::CenterX);
    
    PPE::Ascii::PrintWrapped(0, 13, 32, "Original Artwork:", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 14, 32, "sherbul", PPE::Ascii::Align::CenterX);
    
    PPE::Ascii::PrintWrapped(0, 16, 32, "Additional Artwork:", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 17, 32, "hassmaschine", PPE::Ascii::Align::CenterX);
    
    PPE::Ascii::PrintWrapped(0, 19, 32, "Music:", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 20, 37, "\"The Sea\": hassmaschine, moby gratis", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 21, 32, "\"A Few\": moby gratis", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 22, 32, "\"Electric One\": moby gratis", PPE::Ascii::Align::CenterX);
    
    PPE::Ascii::PrintWrapped(0, 24, 32, "Special Thanks!", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 25, 32, "reyeme: Line Colors", PPE::Ascii::Align::CenterX);
    PPE::Ascii::PrintWrapped(0, 26, 32, "7shades: Vdp2 Wizardry", PPE::Ascii::Align::CenterX);
}