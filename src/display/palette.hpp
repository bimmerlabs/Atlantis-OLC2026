#pragma once

#include "../main.hpp"
#include "palette.pal"

// for sprites
SRL::CRAM::Palette gamePalette;

inline SRL::CRAM::Palette loadSpritePalette(void)
{
    SRL::CRAM::TextureColorMode mode = SRL::CRAM::TextureColorMode::Paletted256;

    int32_t id = 0;

    SRL::CRAM::Palette cramPalette(mode, id);

    cramPalette.Load((SRL::Types::HighColor*)game_pal, 256);
    
    return cramPalette;
}

inline void LoadBackgroundPalette(uint16_t* palData)
{
    SRL::CRAM::TextureColorMode mode = SRL::CRAM::TextureColorMode::Paletted256;

    int32_t id = 1;
    SRL::CRAM::Palette backgroundPalette(mode, id);
    backgroundPalette.Load((SRL::Types::HighColor*)palData, 256);
}