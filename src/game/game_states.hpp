#pragma once
#include "../core/core.hpp"

// this sucks - shouldn't need to include these, should just be "game.hpp" or whatever
#include "title_screen.hpp"
#include "title_menu.hpp"
#include "gameplay.hpp"
#include "demo.hpp"
#include "credits.hpp"

// the problem with the transitions is you can't go from transition out white to transition in black, or vice versa
inline void registerGameStates(void)
{
    registerState(GAME_STATE_UNINITIALIZED, nullptr, nullptr, nullptr);
    registerTransition(GAME_STATE_UNINITIALIZED,
        // TRANSITION_FADE_BLACK | TRANSITION_MOSAIC | TRANSITION_NONE,  // entering: fade in
        TRANSITION_FADE_WHITE | TRANSITION_NONE,  // entering: fade in
        TRANSITION_FADE_WHITE | TRANSITION_NONE); // leaving: fade out + music out
        
    // BUP check (unfinished)
    registerState(GAME_STATE_BUP, PPE::bup_init, PPE::bup_update, PPE::bup_input);
    registerTransition(GAME_STATE_BUP,
        TRANSITION_FADE_WHITE | TRANSITION_NONE,
        TRANSITION_FADE_WHITE | TRANSITION_NONE);
        
    registerState(GAME_STATE_LOGO, logo_init, logo_update, logo_input);
    registerTransition(GAME_STATE_LOGO,
        TRANSITION_FADE_WHITE | TRANSITION_NONE,
        TRANSITION_FADE_WHITE | TRANSITION_NONE);
        
    registerState(GAME_STATE_TITLE_SCREEN,   titleScreen_init,   titleScreen_update,   titleScreen_input);
    registerTransition(GAME_STATE_TITLE_SCREEN,
        TRANSITION_FADE_WHITE | TRANSITION_NONE,
        TRANSITION_FADE_WHITE | TRANSITION_NONE | TRANSITION_MUSIC);
        
    registerState(GAME_STATE_TITLE_MENU,     titleMenu_init,     titleMenu_update,     titleMenu_input);
    // registerTransition(GAME_STATE_TITLE_MENU, TRANSITION_MOSAIC, TRANSITION_MOSAIC);// not sure this is workin
        
    registerState(GAME_STATE_GAMEPLAY,  gameplayInit, gameplayUpdate, gameplayInput);
    registerTransition(GAME_STATE_GAMEPLAY,
        TRANSITION_FADE_WHITE | TRANSITION_NONE,
        TRANSITION_FADE_WHITE | TRANSITION_NONE | TRANSITION_MUSIC);
        
    registerState(GAME_STATE_DEMO_LOOP,  demoInit, demoUpdate, demoInput);
    registerTransition(GAME_STATE_DEMO_LOOP,
        TRANSITION_FADE_WHITE | TRANSITION_NONE,
        TRANSITION_FADE_WHITE | TRANSITION_NONE | TRANSITION_MUSIC);
        
    registerState(GAME_STATE_CREDITS,  creditsInit, creditsUpdate, creditsInput);
    registerTransition(GAME_STATE_CREDITS,
        TRANSITION_FADE_WHITE | TRANSITION_NONE,
        TRANSITION_FADE_WHITE | TRANSITION_NONE | TRANSITION_MUSIC);
}