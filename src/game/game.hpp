#pragma once
#include "../core/core.hpp"

constexpr int MAX_TURRETS    = 9; // needs tuning - also why did I put this here?

struct Turret_t
{
    Vector2D worldPos;
    Vector2D reticlePos;
    Atlantis::AutoTurret ai;
    bool active = false;
};

// none of this is implemented yet (holdovers from PPP)
enum
{
    GAME_MODE_STORY = 0,
    GAME_MODE_MAX,
} GAME_MODE;

enum
{
    GAME_DIFFICULTY_EASY = 0,
    GAME_DIFFICULTY_MEDIUM,
    GAME_DIFFICULTY_HARD,
    GAME_DIFFICULTY_MAX,
} GAME_DIFFICULTY;

struct Game_t
{
    int32_t gameMode          = GAME_MODE_STORY;
    int32_t numPlayers        = 1;
    int32_t minPlayers        = 1;
    int32_t maxPlayers        = 1;
    int32_t currentNumPlayers = 0;
    int32_t difficulty        = GAME_DIFFICULTY_MEDIUM;
    
    void Reset() { *this = Game_t{}; }
};
//

inline Game_t Game = {};

struct Gameplay_t
{
    bool isPaused = false;
    
    bool paused = true; // gol
    bool displayStats = false;
    
    bool underAttack = false; // meant to give time to place turrets, but I forgot :D
    
    SRL::Math::Types::Vector2D reticlePos = {0, 0};
    
    Fxp cursorWorldX = 0;
    Fxp cursorWorldY = 0;
    
    Fxp horizontalSpeed = 4;
    Fxp verticalSpeed   = 3;
    uint32_t waterLevel = 40;
    
    Turret_t turrets[MAX_TURRETS] = {};
    uint8_t turretCount = 0;
        
    bool demoMode = false;
    bool drawDemoText = false;
    int32_t demoTimer = 0;
    
    uint32_t gameScore = 0;
        
    void Reset() { *this = Gameplay_t{}; }
};

static Gameplay_t gp = {};
