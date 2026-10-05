#pragma once
#include "../../main.hpp"

struct Nbg_t
{    
    bool scroll = true;
    Fxp x = 0;
    Fxp y = 0;    
    void Reset() { *this = Nbg_t{}; }
};
    
static Nbg_t nbg1;
static Nbg_t nbg0;

SRL::Tilemap::Interfaces::CubeTile* ppeTilemap;
SRL::Tilemap::Interfaces::CubeTile* gameTilemap;

void preloadNbg1(void) {
    ppeTilemap  = new SRL::Tilemap::Interfaces::CubeTile("GRIDA.BIN");
    gameTilemap = new SRL::Tilemap::Interfaces::CubeTile("NBG1.BIN");
}

static bool firstNBG0Load = true;
static bool firstNBG1Load = true;
static bool firstNBG2Load = true;

void nbg1InitTitleScreen(void)
{
    SRL::VDP2::NBG1::ScrollDisable();
    SRL::VDP2::NBG3::ScrollDisable();
    if (firstNBG1Load)
    {
        SRL::VDP2::NBG1::SetCellAddress(SRL::VDP2::VRAM::Allocate(0x0000,32,SRL::VDP2::VramBank::A1,0),0x4000);  // CELL can be anywhere, but in high res it should be in a different bank since ther are limited timings
        SRL::VDP2::NBG1::SetMapAddress(SRL::VDP2::VRAM::Allocate(0x4000,1024,SRL::VDP2::VramBank::B1,1),0x2000); // MAP needs to be in A1 or B1
        firstNBG1Load = false;
    }
    else if (SRL::VDP2::NBG1::TilePalette.GetData())
    {
        SRL::CRAM::SetBankUsedState(SRL::VDP2::NBG1::TilePalette.GetId(), SRL::VDP2::NBG1::Info.ColorMode, false);
        SRL::VDP2::NBG1::TilePalette = SRL::CRAM::Palette();
    }
    
    SRL::VDP2::NBG1::LoadTilemap(*ppeTilemap);
    
    slZoomNbg1(toFIXED(1.0), toFIXED(2.0));
    nbg1.Reset();

    SRL::VDP2::NBG1::SetPriority(SRL::VDP2::Priority::Layer4);
    SRL::VDP2::SetColorCalcMode(SRL::VDP2::ColorCalcMode::UseColorAddition, true);
    SRL::VDP2::NBG1::SetOpacity(0.5);
    SRL::VDP2::NBG1::ScrollEnable();
}

void nbg0Init(void) {
    if (firstNBG0Load)
    {
        SRL::Tilemap::Interfaces::CubeTile* Tilemap = new SRL::Tilemap::Interfaces::CubeTile("NBG0.BIN");
        
        SRL::VDP2::NBG0::LoadTilemap(*Tilemap);
        delete Tilemap;
        
        SRL::VDP2::NBG0::SetPriority(SRL::VDP2::Priority::Layer1);
        slZoomNbg0(toFIXED(1.0), toFIXED(2.0));
        firstNBG0Load = false;
    }
}

void nbg2Init(void) {
    if (firstNBG2Load)
    {        
        SRL::Tilemap::Interfaces::CubeTile* Tilemap = new SRL::Tilemap::Interfaces::CubeTile("NBG2.BIN");
        SRL::VDP2::NBG2::LoadTilemap(*Tilemap);
        delete Tilemap;
        
        SRL::VDP2::NBG2::SetPriority(SRL::VDP2::Priority::Layer2);//set NBG1 priority        
        firstNBG2Load = false;
    }
}

void nbg1InitGame(void)
{
    SRL::VDP2::NBG1::ScrollDisable();
    
    SRL::VDP2::NBG1::LoadTilemap(*gameTilemap);
    
    nbg1.scroll = false;
    
    nbg0Init();
    nbg2Init();
    
    SRL::VDP2::SetColorCalcMode(SRL::VDP2::ColorCalcMode::UseColorAddition, false); // UseColorRatiosTop, UseColorRatios2nd, or UseColorAddition
    SRL::VDP2::NBG0::SetOpacity(1);
    SRL::VDP2::NBG1::SetOpacity(1);
    SRL::VDP2::NBG2::SetOpacity(0.5); // adjust - check if ratio or additive is better
    
    SRL::VDP2::NBG0::ScrollEnable();
    SRL::VDP2::NBG1::ScrollEnable();
    SRL::VDP2::NBG2::ScrollEnable();
}
