#include "main.hpp"
#include <srl_timer.hpp>
// #include <ColorHelpers.hpp>
#include "debug/debug.hpp"

void loadingScreen(void)
{
    if (Transition.phase == !TRANSITION_STATE_LOADING) {
        return;
    }
    
    if (PPE::Core.isLoading) {        
        PPE::Ascii::Print(17, 12, "Loading!");
        
        if (PPE::Core.isSoundLoading) {
            PPE::Ascii::Print(15, 14, "SoundFX:%d  ", Sound::GetNumberOfPCMs());
        }
        else {
            PPE::Ascii::Print(15, 14, "Sprites:%d  ", SRL::VDP1::GetTextureCount());
        }
    }
}

static void vblankLoop(void) {
    PPE::Core.frame++; // this controls a lot of logic, drawing, & timing..
    if (PPE::Core.frame > 239)
        PPE::Core.frame = 0;
    #if ENABLE_DEBUG_MODE == 1
        debugText();
    #endif
    loadingScreen();
}

namespace
{
    void GameResetDisplay(void)
    {
        nbg1.Reset();
    }
}

static void InitGame(void)
{
    PPE::CoreOptions.debugMode = false;
    PPE::CoreOptions.debugDisplay = false;
    // PPE::CoreOptions.debugDisplay = true;
    
    PPE::Hooks::onDisplayReset = &GameResetDisplay;
    
    PPE::rnd = SRL::Math::Random<int32_t>(15); // not sure why I put this here...
    PPE::Core.Reset();
    PPE::initInputs();
    PPE::initPlayers();
    PPE::loadGameBackup();
    
    PPE::Ascii::Init(&SystemFont);
    
    loadSpritePalette();
    preloadNbg1();
    Display::Background::Sky::Initialize();
    
    // To save VRAM, you can unload these later
    loadCoreAssets();
    loadCoreSoundAssets();
    loadTitleScreenAssets();
            
    registerGameStates();
    
    PPE::changeState(GAME_STATE_UNINITIALIZED);
}

int main(void)
{
    #ifdef SRL_HIGH_RES
    SRL::Core::Initialize(HighColor::Colors::Black, SRL::TV::Resolutions::Interlaced704x480);
    SRL::TV::TVOff();
    #else
    SRL::Core::Initialize(HighColor::Colors::Black, SRL::TV::Resolutions::Normal704x240);
    SRL::TV::TVOff();
    slSetSprTVMode(static_cast<uint16_t>(SRL::TV::Resolutions::Interlaced704x480)); // tell VDP2 to double the vertical res of VDP1 (high res/non interlaced mode)
    PPE::Screen::SetVirtualDimensions(704, 480);
    #endif
    
    // CRAM mode 0 - required for VDP2 transparency in high-res
    slColRAMMode ( CRM16_1024 ); // must be set before loading any palettes
    slZdspLevel(3);
    
    // Ponesound
    Sound::Driver::Initialize(ADXMode::ADX2304);
    
    SRL::Core::OnVblank += vblankLoop;

    PPE::initInputs();
    InitGame();


    while(1)
    {
        if (SRL::Cd::IsTrayOpen())
        {
            SYS_Exit(0);
        }
        
        if (SRL::Timer::DeltaSeconds() > 0)
            PPE::Core.fps = 1 / SRL::Timer::DeltaSeconds();
        
        auto renderStart = SRL::Timer::Capture();
        
        transitionUpdate();
        
        abcStartCallback();
        
        if (PPE::Core.gameState != GAME_STATE_UNINITIALIZED || Transition.phase != TRANSITION_STATE_IDLE)
        {
            StateFunction[PPE::Core.gameState].input();
            StateFunction[PPE::Core.gameState].update();
        }
                
        // this was only needed for VDP2 fonts
        // #if ENABLE_DEBUG_MODE == 1
        // debugCallback();
        // #endif
        
        if (PPE::CoreOptions.debugDisplay && !gp.isPaused)
        {
            auto renderEnd = SRL::Timer::Capture();
            auto renderTime = renderEnd - renderStart;
            PPE::Ascii::Print(2, 5, "FrameT:%f   ", renderTime.ToMilliseconds());
        }
        
        if (nbg1.scroll)
        {
           nbg1.x += 0.25;
            if (nbg1.x > 512)
                nbg1.x = 0;

            slScrPosNbg1(nbg1.x.RawValue(), nbg1.y.RawValue());
        }
        
        SRL::Core::Synchronize();
    }

    return 0;
}
