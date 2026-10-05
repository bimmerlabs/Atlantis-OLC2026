#pragma once
#include <srl.hpp>
// eventually I'd like to eliminate this file..

#define GAME_CONFIG_INCLUDED

// would be nice if a game state that isn't registered, just defaults to uninitialized (thus preventing a game lock) - or a compiler warning
typedef enum
{
    GAME_STATE_UNINITIALIZED = 0, // Required!
    GAME_STATE_BUP, // Required!
    GAME_STATE_LOGO, // define the rest of these how you like
    GAME_STATE_TITLE_SCREEN,
    GAME_STATE_NAME_ENTRY,
    GAME_STATE_TITLE_MENU,
    GAME_STATE_TITLE_OPTIONS,
    GAME_STATE_TEAM_SELECT,
    GAME_STATE_CHARACTER_SELECT,
    GAME_STATE_GAMEPLAY,
    GAME_STATE_DEMO_LOOP,
    GAME_STATE_CREDITS,
    GAME_STATE_HIGHSCORES,
    GAME_STATE_MAX, // Required - there always needs to be the "max" state
} GAME_STATE;

#define PPE_INITIAL_STATE GAME_STATE_LOGO

// these are hard coded into PPE and that definitely sucks
// also doesn't even work correctly
// (copied directly from pixel poppy pong, but its state logic is totally different)
#define ATTRACT_SCREEN_1 GAME_STATE_DEMO_LOOP
#define ATTRACT_SCREEN_2 GAME_STATE_CREDITS
#define ATTRACT_SCREEN_3 GAME_STATE_DEMO_LOOP
#define ATTRACT_SCREEN_4 GAME_STATE_HIGHSCORES
#define ATTRACT_SCREEN_MAX (4)

#define MAX_PLAYERS (2)

struct GameOptions_t
{
    bool test1  = false;
    bool test2 = false;
};

inline unsigned char FileName[13] = "PPEATLANTIS";   // 11 ASCII chars + NUL, 12 bytes
inline unsigned char Comment[11]  = "Atlantis26"; // 10 ASCII chars + NUL, 11 bytes

inline GameOptions_t GameOptions = {};

struct PlayerState_t
{
    int32_t score = 0;
    int8_t lives = 3;
    int8_t characterId = -1;
};
