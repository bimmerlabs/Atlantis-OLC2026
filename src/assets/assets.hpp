#pragma once
#include "../main.hpp"
#include "game_audio.hpp"
#include "../display/vdp1/sprites.hpp"
#include "../display/palette.hpp"
#include <tmsf.hpp>

using namespace SRL::Types;
using namespace SRL::Math::Types;

#define PaletteID (0)

// tmsf format loads in alphabetical order
enum
{
    CORE_SPRITE_KNOT = 0,
    CORE_SPRITE_MAX,
} CORE_SPRITES;

enum
{
    TITLE_SPRITE_KNOT = 0,
    TITLE_SPRITE_MAX,
} TITLE_SPRITES;


// tmsf format loads in alphabetical order
enum
{
    GAME_CURSOR = 0,
    GAME_OCTOPUS,
    GAME_ORACLE,
    GAME_PLANE_MEDIUM,
    GAME_PLANE_RED,
    // GAME_PLANE_SMALL,
    GAME_SPIDER,
    GAME_TURRET,
} GAME_SPRITES;

enum
{
    BALLS_04X04 = 0,
    BALLS_08X08,
    BALLS_16X16,
} BALL_SPRITES;

#define FIRST_SAMPLE 0
#define LAST_SAMPLE 7

struct Assets_t
{
    bool coreAssetsLoaded = false;
    bool coreSoundsLoaded = false;
    bool titleAssetsLoaded = false;
    bool inputAssetsLoaded = false;
    bool gameplayAssetsLoaded = false;
    
    uint16_t startofTitleAssets = 0;
    uint16_t startofInputAssets = 0;
    uint16_t startofGameplayAssets = 0;
};

inline Assets_t Assets = {};
inline TilemapObject* coreTiles = nullptr;
inline TilemapObject* particleTiles = nullptr;
inline TilemapObject* titleTiles = nullptr;
inline TilemapObject* inputTiles = nullptr;
inline TilemapObject* ballTiles = nullptr;
inline TilemapObject* gameplayTiles = nullptr;
inline TilemapObject* fontTiles = nullptr;

inline void loadCoreAssets(void)
{    
    particleTiles = new TilemapObject("PARTICLE.LZ", 1, false, true);
    
    coreTiles = new TilemapObject("CORE.LZ", 1, false, true);
    ppplogo.id = coreTiles->sprite[0].SpriteIndex;
    ppplogo.anim[0].asset = ppplogo.id; // title screen
    ppplogo.anim[1].asset = ppplogo.id + 1; // logo screen
        
    // currently just the default font
    fontTiles = new TilemapObject("FONT.LZ", 1, false, true);
    SystemFont.id = fontTiles->sprite[0].SpriteIndex;
    SystemFont.anim[0].asset = SystemFont.id;
    SystemFont.zmode = _ZmLT;
    delete fontTiles;    
    
    Assets.coreAssetsLoaded = true;
}

inline void loadCoreSoundAssets(void)
{
    Pcm::LoadSound("CORE.SND", Sounds.Core, CORE_SND_MAX);  
    setMenuSounds(&PPE::MenuSystem);
    Assets.coreSoundsLoaded = true;
}

inline void loadTitleScreenAssets(void)
{
    titleTiles = new TilemapObject("TITLE.LZ", 1, false, true);    
    // should assign a sprite instead?  or maybe not needed is why I did it that way
    Assets.titleAssetsLoaded = true;
}

inline void loadInputScreenAssets(void)
{
    PPE::Core.isLoading = true;
    inputTiles = new TilemapObject("INPUT.LZ", 1, false, true);
    
    // Assets.startofTitleAssets = coreTiles->Count(); // count doesn't exist, but maybe it should?
    Assets.startofInputAssets = inputTiles->sprite[0].SpriteIndex;
    
    Controller.anim[0].asset = inputTiles->sprite[0].SpriteIndex; // analog
    Controller.anim[1].asset = inputTiles->sprite[1].SpriteIndex; // analog
    Controller.id = Controller.anim[1].asset; // analog
    
    delete inputTiles;
    Assets.inputAssetsLoaded = true;
    PPE::Core.isLoading = false;
}

void unloadInputAssets(void)
{
    delete inputTiles;
    inputTiles = nullptr;
    SRL::VDP1::ResetTextureHeap(Assets.startofInputAssets);
    Assets.inputAssetsLoaded = false;
}

// for GOL
enum 
{
    GRAY = 0,
    MAGENTA,
    RED,
    YELLOW,
    GREEN,
    BLUE,
    VIOLET,
} BALL_COLORS;

inline void loadGameplayAssets(void)
{
    PPE::Core.isLoading = true;
    
    // BALLS!!!
    // Assets.startofTitleAssets = coreTiles->Count(); // count doesn't exist, but maybe it should?
    ballTiles = new TilemapObject("BALLS.LZ", 1, false, true);
    
    Assets.startofGameplayAssets = ballTiles->sprite[BALLS_04X04].SpriteIndex;
    
    Gray.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Gray.scl = { .x = 1, .y = 1};
    Gray.id = 0;
    Gray.pal_id = 0;
    Gray.flip = sprNoflip;
    Gray.mesh = MESHoff;
    Gray.zmode = _ZmCC;
    Gray.id = ballTiles->sprite[BALLS_08X08].SpriteIndex + GRAY;
    
    Magenta.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Magenta.scl = { .x = 1, .y = 1};
    Magenta.id = 0;
    Magenta.pal_id = 0;
    Magenta.flip = sprNoflip;
    Magenta.mesh = MESHoff;
    Magenta.zmode = _ZmCC;
    Magenta.id = ballTiles->sprite[BALLS_16X16].SpriteIndex + MAGENTA;
    
    Red.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Red.scl = { .x = 1, .y = 1};
    Red.id = 0;
    Red.pal_id = 0;
    Red.flip = sprNoflip;
    Red.mesh = MESHoff;
    Red.zmode = _ZmCC;
    Red.id = ballTiles->sprite[BALLS_08X08].SpriteIndex + RED;
    
    Yellow.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Yellow.scl = { .x = 1, .y = 1};
    Yellow.id = 0;
    Yellow.pal_id = 0;
    Yellow.flip = sprNoflip;
    Yellow.mesh = MESHoff;
    Yellow.zmode = _ZmCC;
    Yellow.id = ballTiles->sprite[BALLS_08X08].SpriteIndex + YELLOW;
    
    Green.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Green.scl = { .x = 1, .y = 1};
    Green.id = 0;
    Green.pal_id = 0;
    Green.flip = sprNoflip;
    Green.mesh = MESHoff;
    Green.zmode = _ZmCC;
    Green.id = ballTiles->sprite[BALLS_08X08].SpriteIndex + GREEN;
    
    Blue.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Blue.scl = { .x = 0.75, .y = 0.75};
    Blue.id = 0;
    Blue.pal_id = 0;
    Blue.flip = sprNoflip;
    Blue.mesh = MESHoff;
    Blue.zmode = _ZmCC;
    Blue.id = ballTiles->sprite[BALLS_16X16].SpriteIndex + BLUE;
    
    Violet.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Violet.scl = { .x = 1, .y = 1};
    Violet.id = 0;
    Violet.pal_id = 0;
    Violet.flip = sprNoflip;
    Violet.mesh = MESHoff;
    Violet.zmode = _ZmCC;
    Violet.id = ballTiles->sprite[BALLS_08X08].SpriteIndex + VIOLET;
    
    Explode.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Explode.scl = { .x = 1, .y = 1};
    Explode.id = 0;
    Explode.pal_id = 0;
    Explode.flip = sprNoflip;
    Explode.mesh = MESHoff;
    Explode.zmode = _ZmCC;
    Explode.id = ballTiles->sprite[BALLS_08X08].SpriteIndex + MAGENTA;
    Explode.anim[0].max = ballTiles->sprite[BALLS_08X08].MaxFrames;
    Explode.anim[0].frame = 0;
    
    delete ballTiles;
        
    // gameplay sprites
    gameplayTiles = new TilemapObject("SPRITES.LZ", 1, false, true);
    
    // player objects
    Cursor.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Cursor.scl = { .x = 2, .y = 2};
    Cursor.id = 0;
    Cursor.pal_id = 0;
    Cursor.flip = sprNoflip;
    Cursor.mesh = MESHoff;
    Cursor.zmode = _ZmCB;
    Cursor.id = gameplayTiles->sprite[GAME_CURSOR].SpriteIndex;
    // could make a loader for this
    Cursor.anim[0].max = gameplayTiles->sprite[GAME_CURSOR].MaxFrames;
    Cursor.anim[0].asset = Cursor.id;
    Cursor.anim[0].frame = 0;
    
    // player objects
    Octopus.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Octopus.scl = { .x = 2, .y = 2};
    Octopus.id = 0;
    Octopus.pal_id = 0;
    Octopus.flip = sprNoflip;
    Octopus.mesh = MESHoff;
    Octopus.zmode = _ZmCB;
    Octopus.id = gameplayTiles->sprite[GAME_OCTOPUS].SpriteIndex;    
    Octopus.anim[0].max = gameplayTiles->sprite[GAME_OCTOPUS].MaxFrames;
    Octopus.anim[0].frame = 0;
    Octopus.anim[0].asset = Octopus.id;
    
    Oracle.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Oracle.scl = { .x = 2, .y = 2};
    Oracle.id = 0;
    Oracle.pal_id = 0;
    Oracle.flip = sprNoflip;
    Oracle.mesh = MESHoff;
    Oracle.zmode = _ZmCB;
    Oracle.id = gameplayTiles->sprite[GAME_ORACLE].SpriteIndex;
    Oracle.anim[0].max = gameplayTiles->sprite[GAME_ORACLE].MaxFrames;
    Oracle.anim[0].frame = 0;
    Oracle.anim[0].asset = Oracle.id;
    
    Turret.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Turret.scl = { .x = 2, .y = 2};
    Turret.id = 0;
    Turret.pal_id = 0;
    Turret.flip = sprNoflip;
    Turret.mesh = MESHoff;
    Turret.zmode = _ZmCB;
    Turret.id = gameplayTiles->sprite[GAME_TURRET].SpriteIndex;
    Turret.anim[0].max = gameplayTiles->sprite[GAME_TURRET].MaxFrames;
    Turret.anim[0].frame = 0;
    
    RedPlane.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    RedPlane.scl = { .x = 2, .y = 2};
    RedPlane.id = 0;
    RedPlane.pal_id = 0;
    RedPlane.flip = sprNoflip;
    RedPlane.mesh = MESHoff;
    RedPlane.zmode = _ZmCC;
    RedPlane.id = gameplayTiles->sprite[GAME_PLANE_RED].SpriteIndex;
    
    // enemies    
    MediumPlane.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    MediumPlane.scl = { .x = 2, .y = 2};
    MediumPlane.id = 0;
    MediumPlane.pal_id = 0;
    MediumPlane.flip = sprNoflip;
    MediumPlane.mesh = MESHoff;
    MediumPlane.zmode = _ZmCC;
    MediumPlane.id = gameplayTiles->sprite[GAME_PLANE_MEDIUM].SpriteIndex;
    
    // SmallPlane.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    // SmallPlane.scl = { .x = 2, .y = 2};
    // SmallPlane.id = 0;
    // SmallPlane.pal_id = 0;
    // SmallPlane.flip = sprNoflip;
    // SmallPlane.mesh = MESHoff;
    // SmallPlane.zmode = _ZmCC;
    // SmallPlane.id = gameplayTiles->sprite[GAME_PLANE_SMALL].SpriteIndex;
        
    Spiders.pos = { .x = 0, .y = 0, .z = 100, .r = 0};
    Spiders.scl = { .x = 1, .y = 1};
    Spiders.id = 0;
    Spiders.pal_id = 0;
    Spiders.flip = sprNoflip;
    Spiders.mesh = MESHoff;
    Spiders.zmode = _ZmCB;
    Spiders.id = gameplayTiles->sprite[GAME_SPIDER].SpriteIndex;
    Spiders.anim[0].asset = Spiders.id;
    Spiders.anim[0].max = gameplayTiles->sprite[GAME_SPIDER].MaxFrames; // not really an animation, but helps track how many there are to choose from
    Spiders.anim[0].frame = 0;
    
    delete gameplayTiles;
    
    Assets.gameplayAssetsLoaded = true;
    PPE::Core.isLoading = false;
}

void unloadGameplayAssets(void)
{
    ballTiles = nullptr;
    ballTiles = gameplayTiles;
    SRL::VDP1::ResetTextureHeap(Assets.startofGameplayAssets);
    Assets.gameplayAssetsLoaded = false;
}
