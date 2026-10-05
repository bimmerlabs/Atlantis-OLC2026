#pragma once
#include <srl.hpp>
#include <particles.hpp>

#define EXPLODE_EMIT_RATE (1)
SRL::FX::ParticleConfig particleCfg;
SRL::FX::ParticleConfig explodeCfg;

static SRL::FX::ParticleSystem particles1;
static SRL::FX::ParticleSystem particles2; // I need the second one until I can fix the number of particles being tied to the frame rate

static SRL::FX::ParticleSystem explode;

inline void initBubblesFx(void)
{       particleCfg.spawnX   = 0;
    particleCfg.spawnY   = 230;
    particleCfg.spawnModeX = SRL::FX::Spawn_Random;
    particleCfg.spawnModeY = SRL::FX::Spawn_Fixed;
    particleCfg.minScale = 0.8;
    particleCfg.maxScale = 1.2;
    particleCfg.emitRate = 1;
    particleCfg.minVX    = 1.5;
    particleCfg.maxVX    = -1.5;
    particleCfg.minVY    = -2;
    particleCfg.maxVY    = -3.5;
    particleCfg.gravityX = 0.00;
    particleCfg.gravityY = -0.01;
    particleCfg.radius = 0;
    particleCfg.decay    = 200;
    particleCfg.spawnModeX = SRL::FX::Spawn_Random;
    particleCfg.spawnModeY = SRL::FX::Spawn_Fixed;
    particleCfg.scaleMode = SRL::FX::Scale_Grow;
    
    particles1.Init(0, 3, &particleCfg);
    particles2.Init(0, 3, &particleCfg);    
}
inline void initBombFx(void)
{   
    explodeCfg.spawnX   = 0;
    explodeCfg.spawnY   = 0;
    explodeCfg.minScale = 0.1;
    explodeCfg.maxScale = 1.5;
    explodeCfg.minVX    = -3;
    explodeCfg.maxVX    = 3;
    explodeCfg.minVY    = -3;
    explodeCfg.maxVY    = 3;
    explodeCfg.minVZ    = -0.03;
    explodeCfg.maxVZ    = -0.03;
    explodeCfg.gravityX = 0.00;
    explodeCfg.gravityY = 0.00;
    explodeCfg.radius = 12;
    explodeCfg.emitRate = 0;
    explodeCfg.decay    = 40;
    explodeCfg.spawnModeX = SRL::FX::Spawn_Fixed;
    explodeCfg.spawnModeY = SRL::FX::Spawn_Fixed;
    explodeCfg.scaleMode = SRL::FX::Scale_Shrink;
    explodeCfg.animate  = true;
    
    explode.Init(12, 6, &explodeCfg); // placeholder sprite ID for now
}
inline void drawBubblesFx(void)
{
    particles1.Emit();
    particles1.Update();
    particles1.Draw();
    particles2.Emit();
    particles2.Update();
    particles2.Draw();
}

// let the particles expire
inline void fadeBubblesFx(void)
{
    particles1.Update();
    particles1.Draw();
    particles2.Update();
    particles2.Draw();
}
inline void drawExplosionFx(void)
{
    explode.Emit();
    explode.Update();
    explode.Draw();    
}