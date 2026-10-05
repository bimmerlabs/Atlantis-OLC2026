#pragma once
#include "../core/core.hpp"
#include "../display/display.hpp"
#include "../display/vdp2/linescroll.hpp"
#include "game.hpp"
#include "game_menu.hpp"

using namespace SRL::Math::Types;

constexpr Fxp ORACLE_WORLD_Y = 144;

constexpr Fxp WORLD_WIDTH    = 2048;
constexpr Fxp SPAWN_WIDTH    = 2752;
constexpr Fxp WORLD_CENTER_X = WORLD_WIDTH / 2;
static inline Fxp turretWorldY = 176; // placeholder

Display::Background::Linescroll waterScroll;

inline void gameplayInit(void)
// this is all a mess, needs reorganized
{
    if (!Assets.gameplayAssetsLoaded)
    {
        loadGameplayAssets();
    }
    nbg1InitGame();
    
    PPE::Audio::ResetVolume();
    PPE::Audio::PlayCDTrack(GOL_TRACK, true);

    gp.Reset();

    Atlantis::Defense.raiders.Clear(); // do I need to clear turrets too?
    Atlantis::Defense.projectiles.Clear();    
    Atlantis::GameCamera.Reset(WORLD_CENTER_X);
    
    DateTime time = DateTime::Now();

    SRL::Math::Random<int32_t> rnd = SRL::Math::Random<int32_t>(time.Minute());

    const int16_t spawnY = -SRL::TV::Height;

    // this could be controlled by a difficult setting
    constexpr size_t TOTAL_WAVES = 10;
    constexpr size_t RAIDERS_PER_WAVE = 32;
    constexpr uint32_t WAVE_INTERVAL = 2*60; // 5 seconds @ 60 FPS

    Atlantis::Defense.StartLevel(TOTAL_WAVES, RAIDERS_PER_WAVE, WAVE_INTERVAL);
    
    titleScroll.Reset(); // rest NBG1 before initializing NBG0
    waterScroll.initializeNbg0();
    
    Atlantis::GameCamera.SetWaterYScroll(gp.waterLevel);
    waterScroll.setWaterHeight(gp.waterLevel);
}

inline void gameplayDraw(void)
{
    if (!gp.isPaused)
    {
        PPE::Ascii::Print(2,  1, "Score: %09d ", gp.gameScore);
        PPE::Ascii::Print(28, 1, "Turrets: %d/9", gp.turretCount);
    }
    
    const Fxp halfWidth = Atlantis::GameCamera.GetHalfWidth();

    // the Oracle
    if (Core.gameState != GAME_STATE_DEMO_LOOP)
    {
        SetSpritePositionFxp(&Cursor, gp.cursorPos.X, gp.cursorPos.Y);
        FastSpriteDraw(&Cursor);
        
        SetSpritePositionFxp(&Oracle, 0, ORACLE_WORLD_Y);
        FastSpriteDraw(&Oracle);
    }

    // auto turrets
    for (int i = 0; i < MAX_TURRETS; ++i)
    {
        if (!gp.turrets[i].active) continue;
        
        Fxp screenX = gp.turrets[i].worldPos.X - Atlantis::GameCamera.x - halfWidth;
        // Fxp screenX = gp.turrets[i].cursorPos.X - Atlantis::GameCamera.x;
        if (screenX < -halfWidth || screenX > halfWidth) continue;
        
        SetSpritePositionFxp(&Turret, screenX, Fxp(176));
        FastSpriteDraw(&Turret);
    }
    
    // projectiles
    for (const auto& proj : Atlantis::Defense.projectiles.items)
    {
        if (!proj.active) continue;

        Fxp screenX = proj.pos.X - Atlantis::GameCamera.x - halfWidth;
        if (screenX < -halfWidth || screenX > halfWidth) continue;

        Sprite_t* spriteToDraw = &Red;

        switch (proj.owner)
        {
            case Atlantis::ProjectileOwner::Player:
                spriteToDraw = &Blue;
                break;

            case Atlantis::ProjectileOwner::Turret:
                spriteToDraw = &Red;
                break;

            case Atlantis::ProjectileOwner::Raider:
                spriteToDraw = &Green;
                break;
        }

        SetSpritePositionFxp(spriteToDraw, screenX, proj.pos.Y);
        FastSpriteDraw(spriteToDraw);
    }
    
    for (const auto& raider : Atlantis::Defense.raiders.items)
    {
        if (!raider.active) continue;

        Fxp screenX = raider.pos.X - Atlantis::GameCamera.x - halfWidth;
        if (screenX < -halfWidth || screenX > halfWidth) continue;
        
        switch (raider.type)
        {
            case Atlantis::RaiderType::Standard:
                Spiders.id = Spiders.anim[0].asset;
                break;
            case Atlantis::RaiderType::ZigZag:
                Spiders.id = Spiders.anim[0].asset + 1;
                break;
            case Atlantis::RaiderType::Swerve:
                Spiders.id = Spiders.anim[0].asset + 2;
                break;
            case Atlantis::RaiderType::FastDropper:
                Spiders.id = Spiders.anim[0].asset +3 ;
                break;
        }
        SetSpritePositionFxp(&Spiders, screenX, (raider.pos.Y));
        FastSpriteDraw(&Spiders);
    }
}

static inline uint16_t nextWaterUpdate = 0;

inline void gameplayUpdate(void)
{
    // increase water level every 30 seconds
    static uint32_t waterFrameCounter = 0;
    if (!gp.isPaused) waterFrameCounter++;
    if (waterFrameCounter >= 15*60)
    {
        waterFrameCounter = 0;
        if (gp.waterLevel <= 108)
        {
            gp.waterLevel += 2;
        }
    }
    
    // always update linescreen
    waterScroll.setWaterHeight(gp.waterLevel);
    Atlantis::GameCamera.SetWaterYScroll(gp.waterLevel);
    waterScroll.update();
    
    if (gp.isPaused)
    {
        PPE::MenuSystem.draw(3, 8);
        return;
    }

    auto turretStart = SRL::Timer::Capture();
    
    Atlantis::TurretManager::Update(
        gp.turrets, 
        Atlantis::Defense, 
        Atlantis::GameCamera.x, 
        Atlantis::GameCamera.GetHalfWidth(),
        Atlantis::GameCamera.GetHalfHeight(),
        PPE::Core.frame
    );

    auto turretEnd = SRL::Timer::Capture();

    auto defenseStart = SRL::Timer::Capture();
    Atlantis::Defense.Update(&gp.gameScore);
    auto defenseEnd = SRL::Timer::Capture();

    auto renderStart = SRL::Timer::Capture();
    gameplayDraw();
    auto renderEnd = SRL::Timer::Capture();

    // cursor position to world position
    Fxp cursorWorldX = Atlantis::GameCamera.ScreenToWorldX(gp.cursorPos.X);
    Fxp cursorWorldY = Atlantis::GameCamera.ScreenToWorldY(gp.cursorPos.Y);

    if (gp.displayStats)
    {
        // // VDP1 printing
        // PPE::Ascii::Print(0, 0, "Test: (0,0)");
        // PPE::Ascii::Print(2, 2, "MaxGridWidth: %2d", PPE::Ascii::MaxGridWidth());
        // PPE::Ascii::Print(2, 3, "MaxGridHeight:%2d", PPE::Ascii::MaxGridHeight());
        // PPE::Ascii::Print(2, 4, "CenteredX:    %2d", PPE::Ascii::CenteredX(PPE::Ascii::MaxGridWidth()));
        // PPE::Ascii::Print(2, 5, "CenteredY:    %2d", PPE::Ascii::CenteredY(PPE::Ascii::MaxGridHeight()));
        // PPE::Ascii::Print(PPE::Ascii::MaxGridWidth()-1, PPE::Ascii::MaxGridHeight()-1, "Test: (44,28)");
        
        // PPE::Ascii::Print(2, 2, "HalfScreenHeight:%f ", Atlantis::GameCamera.GetHalfHeight());
        // PPE::Ascii::Print(2, 3, "HalfScreenWidth: %f ", Atlantis::GameCamera.GetHalfWidth());
        
        PPE::Ascii::Print(2, 4, "Cam Left X  : %4d", Atlantis::GameCamera.x.As<int32_t>());
        PPE::Ascii::Print(2, 6, "Cursor Screen: %f,%f", gp.cursorPos.X, gp.cursorPos.Y);
        PPE::Ascii::Print(2, 7, "Cursor World  : %f,%f", cursorWorldX, cursorWorldY);
        // PPE::Ascii::Print(2, 8, "Local Tile (64x30) : X:%2d Y:%2d", tx, ty);
        // PPE::Ascii::Print(2, 9, "Global Tile(128x30): X:%3d Y:%2d", globalTx, ty);
        
        // PPE::Ascii::Print(2, 4, "Cam  X: %f     ", Atlantis::GameCamera.x);
        // PPE::Ascii::Print(2, 5, "Cam  Y: %f     ", Atlantis::GameCamera.y);
        // PPE::Ascii::Print(2, 6, "HalfWidth: %f ", Atlantis::GameCamera.GetHalfWidth());
        // PPE::Ascii::Print(2, 7, "HalfHeight:%f ", Atlantis::GameCamera.GetHalfHeight());
        
        // PPE::Ascii::Print(2, 14, "cursorPos X,Y:        %f,%f", gp.cursorPos.X, gp.cursorPos.Y);
        
        PPE::Ascii::Print(2, 17, "WaterLevel:  %2d ", gp.waterLevel);
        PPE::Ascii::Print(2, 18, "Turrets:     %3d ", Atlantis::TurretManager::GetActiveCount(gp.turrets)); // wow that's part of the manager, but it's super overcomplicated
        PPE::Ascii::Print(2, 19, "Raiders:     %3d ", Atlantis::Defense.raiders.Count());
        PPE::Ascii::Print(2, 20, "Projectiles: %3d ", Atlantis::Defense.projectiles.Count());
        
        auto turretTime = turretEnd - turretStart;
        PPE::Ascii::Print(2, 22, "Turret Time: %f ", turretTime.ToMilliseconds());
        auto defenseTime = defenseEnd - defenseStart;
        PPE::Ascii::Print(2, 23, "Defense Time: %f ", defenseTime.ToMilliseconds());
        auto renderTime = renderEnd - renderStart;
        PPE::Ascii::Print(2, 24, "Render Time: %f ", renderTime.ToMilliseconds());
    }
}

// this is used for debug mode input
static inline void MoveCursor(Fxp dx, Fxp dy, Fxp bufferLeft, Fxp bufferRight)
{
    if (dx != 0)
    {
        gp.cursorPos.X += dx * gp.horizontalSpeed;

        if (gp.cursorPos.X > bufferRight)
        {
            Fxp oldCamX = Atlantis::GameCamera.x;
            Fxp pushDelta = gp.cursorPos.X - bufferRight;
            Atlantis::GameCamera.ScrollX(pushDelta);

            Fxp actualMoveX = Atlantis::GameCamera.x - oldCamX;
            gp.cursorPos.X -= actualMoveX;
        }
        else if (gp.cursorPos.X < bufferLeft)
        {
            Fxp oldCamX = Atlantis::GameCamera.x;
            Fxp pushDelta = gp.cursorPos.X - bufferLeft;
            Atlantis::GameCamera.ScrollX(pushDelta);

            Fxp actualMoveX = Atlantis::GameCamera.x - oldCamX;
            gp.cursorPos.X -= actualMoveX;
        }
    }

    if (dy != 0)
    {
        gp.cursorPos.Y += dy * gp.verticalSpeed;
    }
}

inline void gameplayInput(void)
{
    if (PPE::Core.debugInput) return;
    if (Transition.phase != TRANSITION_STATE_IDLE) return;

    const Fxp halfWidth  = PPE::Screen::HalfWidthFxp();
    const Fxp halfHeight = PPE::Screen::HalfHeightFxp();

    constexpr Fxp BUFFER_X = 40;
    constexpr Fxp BUFFER_Y = 30;

    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        Player_t* player = &PPE::Player[i];
        if (!player->input->isSelected) continue;

        Input_t* input = &Input[player->input->id];
        Digital gamepad(input->id);

        if (!gp.isPaused && gamepad.WasPressed(Digital::Button::START))
        {
            PPE::MenuSystem.init(&gameMenu);
            // setMenuSounds(&PPE::MenuSystem);
            // enableFont();
            gp.isPaused = true;
            return;
        }
        else if (gp.isPaused)
        {
            PPE::MenuSystem.input(input, &exitGameMenu);
            return;
        }
        
        // buffers for scrolling the screen with the cursor
        const Fxp bufferRight  =  halfWidth  - BUFFER_X;
        const Fxp bufferLeft   = -halfWidth  + BUFFER_X;
        const Fxp bufferBottom =  halfHeight - BUFFER_Y;
        const Fxp bufferTop    = -halfHeight + BUFFER_Y;
        
        // PPE::Ascii::Print(2, 12, "bufferRight,bufferLeft:%f,%f", bufferRight, bufferLeft);
        // PPE::Ascii::Print(2, 13, "bufferBottom,bufferTop:%f,%f", bufferBottom, bufferTop);
        
        #if ENABLE_DEBUG_MODE == 1
        // works for debug but is too slow for gameplay
            if (input->left.Update(gamepad, Digital::Button::Left))
            {
                MoveCursor(-1, 0, bufferLeft, bufferRight);
            }
            else if (input->right.Update(gamepad, Digital::Button::Right))
            {
                MoveCursor(1, 0, bufferLeft, bufferRight);
            }

            if (input->up.Update(gamepad, Digital::Button::Up))
            {
                MoveCursor(0, -1, bufferLeft, bufferRight);
            }
            else if (input->down.Update(gamepad, Digital::Button::Down))
            {
                MoveCursor (0, 1, bufferLeft, bufferRight);
            }
        #else
            if (gamepad.IsHeld(Digital::Button::Left))
            {
                gp.cursorPos.X -= 1 * gp.horizontalSpeed;

                if (gp.cursorPos.X < bufferLeft)
                {
                    Fxp oldCamX = Atlantis::GameCamera.x;
                    Fxp pushDelta = gp.cursorPos.X - bufferLeft;
                    Atlantis::GameCamera.ScrollX(pushDelta);

                    Fxp actualMoveX = Atlantis::GameCamera.x - oldCamX;
                    gp.cursorPos.X -= actualMoveX;
                }
            }
            else if (gamepad.IsHeld(Digital::Button::Right))
            {              
                gp.cursorPos.X += 1 * gp.horizontalSpeed;

                // Check if cursor entered right buffer boundary
                if (gp.cursorPos.X > bufferRight)
                {
                    Fxp oldCamX = Atlantis::GameCamera.x;
                    Fxp pushDelta = gp.cursorPos.X - bufferRight;
                    Atlantis::GameCamera.ScrollX(pushDelta);

                    Fxp actualMoveX = Atlantis::GameCamera.x - oldCamX;
                    gp.cursorPos.X -= actualMoveX;
                }
            }

            // 2. VERTICAL MOVEMENT
            if (gamepad.IsHeld(Digital::Button::Up))
            {
                gp.cursorPos.Y -= 1 * gp.verticalSpeed;
            }
            else if (gamepad.IsHeld(Digital::Button::Down))
            {
                gp.cursorPos.Y += 1 * gp.verticalSpeed;
            }
        #endif
        
        // clamp cursor to screen
        gp.cursorPos.X = Fxp::Clamp(gp.cursorPos.X, -halfWidth + 32, halfWidth - 32);
        gp.cursorPos.Y = Fxp::Clamp(gp.cursorPos.Y, -halfHeight + 32, halfHeight - 32);

        // fire from oracle
        if (gamepad.WasPressed(Digital::Button::A))
        {
            Vector2D dir(gp.cursorPos.X, gp.cursorPos.Y - 192); // dumb hack

            int32_t dx = dir.X.As<int32_t>();
            int32_t dy = dir.Y.As<int32_t>();
            uint32_t distSq = static_cast<uint32_t>(dx * dx + dy * dy);
            
            if (distSq > 0)
            {
                uint32_t approxDist = Integer::FastSqrt(distSq);
                Fxp lenFxp = Fxp::BuildRaw(approxDist << 16);

                dir.X = (dir.X / lenFxp) * 8;
                dir.Y = (dir.Y / lenFxp) * 8;

                Vector2D playerTurretWorldPos(Atlantis::GameCamera.x + halfWidth, 192); // dumb hack

                Atlantis::Defense.FireProjectile(
                    playerTurretWorldPos, 
                    dir, 
                    Atlantis::ATTACK_RADIUS, 
                    Atlantis::ProjectileOwner::Player
                );
            }
        }
        
        // auto turret
        if (gamepad.WasPressed(Digital::Button::C))
        {            
            Fxp cursorWorldX, cursorWorldY;
            // don't place turrets too close to screen edge (another dumb hack)
            gp.cursorPos.X = Fxp::Clamp(gp.cursorPos.X, -halfWidth + 64, halfWidth - 64);
            Atlantis::GameCamera.ScreenToWorld(gp.cursorPos, cursorWorldX, cursorWorldY);

            for (int t = 0; t < MAX_TURRETS; ++t)
            {
                if (!gp.turrets[t].active)
                {
                    gp.turrets[t].worldPos = Vector2D(cursorWorldX, turretWorldY);
                    gp.turrets[t].cursorPos.X = gp.cursorPos.X;
                    gp.turrets[t].cursorPos.Y = turretWorldY; // having cursor vs world being different is annoying.  it should also calculate the offset internally
                    gp.turrets[t].ai.Reset();
                    gp.turrets[t].active = true;
                    gp.turretCount++;
                    break;
                }
            }
        }
        
        // the timing of this appears to matter - put after any other possible screen transforms/placements        
        constexpr Fxp TRIGGER_SCROLL_SPEED = 6;

        // fast movement
        if (gamepad.IsHeld(Digital::Button::R))
        {
            Atlantis::GameCamera.ScrollX(TRIGGER_SCROLL_SPEED);
        }
        else if (gamepad.IsHeld(Digital::Button::L))
        {
            Atlantis::GameCamera.ScrollX(-TRIGGER_SCROLL_SPEED);
        }
    }
}