#pragma once
#include <pixelpoppyengine.hpp>
#include <ponesound.hpp>

using namespace SRL::Ponesound;

enum GameMusicTracks : uint16_t
{
    LOGO_TRACK  = 2, // MUSIC TRACK 1
    GOL_TRACK   = 3,
    BALLS_TRACK = 4,
    LAST_TRACK  = 5
};

inline const char* musicTrackLabels[] = {
    "The Sea     ",
    "Game of Life",
    "Electric One",
};

enum GameSoundEffect : uint8_t
{
    BackSnd = 0,
    CancelSnd,
    CursorSnd,
    NextSnd,
    StartSnd,
    TickSnd,
    CORE_SND_MAX
};

struct GameSoundAssets_t
{
    short Core[CORE_SND_MAX];
};

inline GameSoundAssets_t Sounds = {};

inline void setMenuSounds(PPE::MenuSystem_t *menu)
{
    menu->onInit = [](){
        Pcm::Play(Sounds.Core[StartSnd], PlayMode::Volatile, 7);
    };
    menu->onSelect = [](){
        Pcm::Play(Sounds.Core[NextSnd], PlayMode::Volatile, 6);
    };
    menu->onAdjustLR = [](){
        Pcm::Play(Sounds.Core[TickSnd], PlayMode::Volatile, 6);
    };
    menu->onAdjustUD = [](){
        Pcm::Play(Sounds.Core[CursorSnd], PlayMode::Volatile, 6);
    };
    menu->onBack = [](){
        Pcm::Play(Sounds.Core[CancelSnd], PlayMode::Volatile, 6);
    };
}

// would be nice to have a message system so I don't have to directly play audio samples in game code