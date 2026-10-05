#pragma once
#include "../../main.hpp"

// this can only be used if an additional layer is available on VDP2
// we chose to use it for gameplay instead

static bool nbg0AsciiFirstLoad = false;

void initFont(void) {
    SRL::VDP2::NBG0::RegisterAsciiScroll();
    
    SRL::ASCII::Clear();
    SRL::Bitmap::TGA* font = new SRL::Bitmap::TGA("FONT.TGA");
    SRL::ASCII::LoadFont(font, 1);
    delete font;
    SRL::ASCII::SetPalette(0);
    SRL::ASCII::SetFont(1);
    
    SRL::VDP2::NBG0::SetPriority(SRL::VDP2::Priority::Layer6);
    SRL::VDP2::NBG0::ScrollEnable();
    
    #ifdef SRL_HIGH_RES
    slZoomNbg0(toFIXED(0.50), toFIXED(0.5));
    #else
    slZoomNbg0(toFIXED(0.50), toFIXED(1.0));
    #endif
    
    nbg0AsciiFirstLoad = true;
}

void enableFont(void) {
    SRL::VDP2::NBG0::ScrollDisable();
    SRL::VDP2::NBG0::SetPriority(SRL::VDP2::Priority::Layer6);
    slScrPosNbg0(0, 0);
    #ifdef SRL_HIGH_RES
    slZoomNbg0(toFIXED(0.50), toFIXED(0.5));
    #else
    slZoomNbg0(toFIXED(0.50), toFIXED(1.0));
    #endif
    slScrPosNbg0(0,0);
    SRL::VDP2::NBG1::SetOpacity(1);
    SRL::VDP2::NBG0::RegisterAsciiScroll();
    SRL::VDP2::NBG0::ScrollEnable();
}