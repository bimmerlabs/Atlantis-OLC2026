#pragma once
#include "../core/core.hpp"
#include "../assets/assets.hpp"
#include "../display/display.hpp"

using namespace SRL::Types;
using namespace SRL::Math::Types;
using namespace SRL::Math;
using namespace SRL::Input;

#define PPE_LOGO_CAT (3 * 60)
#define PPE_LOGO_TIMER (6 * 60)

inline unsigned int LogoTimer = 0;

inline void logo_init(void)
{
    if (!Assets.coreAssetsLoaded)
    {
        loadCoreAssets();
    }
    if (!Assets.titleAssetsLoaded)
    {
        loadTitleScreenAssets();
    }
    
    nbg1InitTitleScreen();
    
    if (!nbg0AsciiFirstLoad)
    {
        initFont();
    }
    
    SRL::TV::TVOn();
    PPE::Audio::ResetVolume();
    PPE::Audio::PlayCDTrack(LOGO_TRACK, true);
    
    logoScreenInit();
    
    titleScroll.Reset(); // reset linescroll table
    
    SRL::VDP2::SetColorCalcMode(SRL::VDP2::ColorCalcMode::UseColorAddition, true);
    SRL::VDP2::NBG1::SetOpacity(0.5);
    
    SRL::VDP2::NBG0::ScrollDisable();
    SRL::VDP2::NBG1::ScrollEnable();
    SRL::VDP2::NBG2::ScrollDisable();
    
    LogoTimer = 0;
}

inline void logo_input(void)
{
    #if ENABLE_DEBUG_MODE == 1
    if (checkStartButton() && Transition.phase == TRANSITION_STATE_IDLE)
    {
        changeState(GAME_STATE_TITLE_SCREEN);
    }
    #endif
}

inline void logo_update(void)
{
    LogoTimer++;
    
    if (LogoTimer == PPE_LOGO_TIMER)
    {
        transitionState(GAME_STATE_TITLE_SCREEN);
    }

    if (!PPE::CoreOptions.debugDisplay)
    {
        PPE::Ascii::PrintWrapped(0, 8, 24, "Pixel Poppy Productions...", PPE::Ascii::Align::CenterX);
    }
    
    // not really right, but leave for now
    SRL::Scene2D::DrawSprite(coreTiles->sprite[0].SpriteIndex, PaletteID, Vector3D(0.0, 0.0, 500.0), Vector2D(2, 2));
    
    if (LogoTimer > PPE_LOGO_CAT)
    {
        PPE::Ascii::PrintWrapped(0, 20, 24, "Presents!", PPE::Ascii::Align::CenterX);
        SRL::Scene2D::DrawSprite(coreTiles->sprite[0].SpriteIndex+1, PaletteID, Vector3D(0.0, 0.0, 500.0), Vector2D(2, 2));
    }
}