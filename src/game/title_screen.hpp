#pragma once
#include "../core/core.hpp"
#include "../display/display.hpp"
#include "../display/vdp2/linescroll.hpp"
#include "../display/particles/particlefx.hpp"

#if ENABLE_DEBUG_MODE == 1
    #define TITLE_TIMER (10*60)  // attract mode countdown
#else
    #define TITLE_TIMER (60*60)
#endif

#define PRESS_START_DELAY (4) // SECONDS

int32_t textureIndex = 0;

struct TitleScreen_t
{
    unsigned int TitleTimer = 0;
    bool DrawStartText = true;
    Fxp logoScale = 1;
    Angle angle = 0;
    bool attractScreen = false;  
    void Reset() { *this = TitleScreen_t{}; }
};

static TitleScreen_t ts = {};

static bool attractScreen = false;

inline void titleScreen_init(void)
{
    if (PPE::Core.lastState != GAME_STATE_LOGO)
    {
        nbg1InitTitleScreen();
     
        SRL::VDP2::NBG0::ScrollDisable();
        SRL::VDP2::NBG1::ScrollEnable();
        SRL::VDP2::NBG2::ScrollDisable();
        
        PPE::Audio::ResetVolume();
        PPE::Audio::PlayCDTrack(LOGO_TRACK, true);
    }
    ts.Reset();
    nbg1.Reset();
    
    // linescroll effect
    titleScroll.initializeNbg1();
    titleScroll.setWaterHeight(240);
   
    initBubblesFx();
}

inline void titleScreen_input(void)
{
    if (ts.TitleTimer < PRESS_START_DELAY*60) return; // 3 seconds
    
    if (checkStartButton() && Transition.phase == TRANSITION_STATE_IDLE)
    {
        changeState(GAME_STATE_TITLE_MENU);
    }
}

inline void titleScreen_update(void)
{
    titleScroll.update();
    drawBubblesFx();
    
    if (ts.TitleTimer < TITLE_TIMER)
    {
        ts.TitleTimer++;    
    }
    else
    {
        // TOTAL BODGE (attract mode)
        // didn't work as intended because ts. gets reset on screen init - need separate value
        if (attractScreen)
        {
            transitionState(GAME_STATE_CREDITS);
            attractScreen = false;
        }
        else {
            transitionState(GAME_STATE_DEMO_LOOP);
            attractScreen = true;
        }
    }

    if (!PPE::CoreOptions.debugDisplay)
    {
        PPE::Ascii::PrintWrapped(0, 5, 24, "Atlantis!", PPE::Ascii::Align::CenterX);
    }

    // blink "press start"
    if (PPE::Core.frame % 30 == 0) // replace with actual time logic?
    {
        ts.DrawStartText = !ts.DrawStartText;
    }
    
    if (ts.TitleTimer > PRESS_START_DELAY*60)
    {
        if (ts.DrawStartText)
            PPE::Ascii::PrintWrapped(0, 24, 24, "PRESS START", PPE::Ascii::Align::CenterX);
        else
            PPE::Ascii::PrintWrapped(0, 24, 24, "           ", PPE::Ascii::Align::CenterX);

        PPE::Ascii::PrintWrapped(0, 25, 24, VERSION, PPE::Ascii::Align::CenterX);
    }

    SRL::Scene2D::DrawSprite(titleTiles->sprite[0].SpriteIndex, PaletteID, Vector3D(0.0, 0.0, 500.0), ts.angle, Vector2D(2, ts.logoScale));
    
    if (ts.logoScale < 2)
    {
        ts.logoScale += 0.01;
        if (ts.logoScale > 2)
            ts.logoScale = 2;
    }
    else
    {
        ts.angle += 0.001;
    }  
}