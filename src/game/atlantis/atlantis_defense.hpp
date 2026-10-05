#pragma once
#include <srl.hpp>
#include "atlantis_pool.hpp"

namespace Atlantis
{
    // todo:  make these configurable on init()
    constexpr size_t MAX_RAIDERS = 128;
    constexpr size_t MAX_PROJECTILES = 256;
    constexpr int16_t WorldWidth = 2752; // it doesn't use the definition from gameplay, which is problematic
    constexpr int ATTACK_RADIUS   = 16;

    enum class RaiderType : uint8_t
    {
        Standard,
        ZigZag,
        FastDropper,
        Swerve
    };
    
    struct Raider
    {
        Vector2D pos{};
        Vector2D vel{};
        Vector2D startPos{};
        RaiderType type = RaiderType::Standard;
        uint16_t animFrame = 0;
        bool active = false;
    };
    
    enum class ProjectileOwner : uint8_t
    {
        Player,
        PlayerShip,
        Turret,
        Raider,
        RaiderShip
    };

    struct Projectile
    {
        Vector2D pos{};
        Vector2D vel{};
        Fxp blastRadiusSq = 0;
        ProjectileOwner owner = ProjectileOwner::Player; // sprite type 
        bool active = false;
    };
    
    struct RaiderPointValue
    {
        uint32_t killScore;
        uint32_t basePenalty;
    };

    inline constexpr RaiderPointValue GetRaiderPoints(RaiderType type)
    {
        switch (type)
        {
            case RaiderType::Standard:   return { 100, 90 };
            case RaiderType::ZigZag:     return { 150, 140 };
            case RaiderType::FastDropper:return { 200, 150 };
            case RaiderType::Swerve:     return { 125, 110 };
            default:                     return { 100, 90 };
        }
    }

    struct DefenseSystem
    {
        ObjectPool<Raider, MAX_RAIDERS> raiders;
        ObjectPool<Projectile, MAX_PROJECTILES> projectiles;
        
        // will be the same for every playthrough...
        SRL::Math::Random<int32_t> rnd{15};
        
        // wave config
        size_t maxConcurrentWaves = 5;
        size_t waveSize = 12;
        uint32_t waveIntervalFrames = 180; // in frames
        uint32_t lastWaveSpawnFrame = 0;
        uint32_t globalFrameCounter = 0;
        
        void StartLevel(size_t maxWaves, size_t perWaveSize, uint32_t intervalFrames)
        {
            raiders.Clear();
            maxConcurrentWaves = maxWaves;
            waveSize = perWaveSize;
            waveIntervalFrames = intervalFrames;
            lastWaveSpawnFrame = 0;
            globalFrameCounter = 0;

            // initial wave
            SpawnWave();
        }

        bool SpawnRaider(Vector2D startPos, Vector2D velocity, RaiderType type)
        {
            if (auto* raider = raiders.Spawn())
            {
                raider->pos = startPos;
                raider->startPos = startPos;
                raider->vel = velocity;
                raider->type = type;
                raider->animFrame = static_cast<uint16_t>(rnd.GetNumber(0, 255));
                raider->active = true;
                return true;
            }
            return false;
        }
        
        void SpawnWave()
        {
            const int16_t minSpawnX = 32;
            const int16_t maxSpawnX = WorldWidth - 32;
            const int16_t spawnY = -SRL::TV::Height;

            for (size_t i = 0; i < waveSize; ++i)
            {
                uint8_t typeRoll = static_cast<uint8_t>(rnd.GetNumber(0, 3));
                RaiderType type = static_cast<RaiderType>(typeRoll);

                Vector2D vel(0, 0);
                if (type == RaiderType::Standard)
                {
                    vel.Y = 1;
                }
                else if (type == RaiderType::ZigZag)
                {
                    vel.Y = 2;
                }
                else if (type == RaiderType::FastDropper)
                {
                    vel.Y = 4;
                }
                else if (type == RaiderType::Swerve)
                {
                    int32_t roll = rnd.GetNumber(0, 100);
                    if (roll > 50)
                    {
                        vel.X = 3;
                    }
                    else
                    {
                        vel.X = 2;
                    }
                    vel.Y = 2;
                }

                int16_t spawnX = static_cast<int16_t>(rnd.GetNumber(minSpawnX, maxSpawnX));
!
                SpawnRaider(Vector2D(spawnX, spawnY), vel, type);
            }
            
            lastWaveSpawnFrame = globalFrameCounter;
        }
        
        void FireProjectile(Vector2D startPos, Vector2D velocity, Fxp radiusSq, ProjectileOwner owner = ProjectileOwner::Player)
        {
            if (auto* proj = projectiles.Spawn())
            {
                proj->pos.X = startPos.X;
                // since I know the owner I can hack Y position by the owner type :D
                switch (owner)
                {
                    case ProjectileOwner::Player:
                        proj->pos.Y = startPos.Y - 64 - 16; // hack to keep aligned with oracle
                        break;
                    case ProjectileOwner::Turret:
                        proj->pos.Y = startPos.Y - 16; // hack to keep aligned with turret
                        break;
                    case ProjectileOwner::Raider:
                    case ProjectileOwner::RaiderShip:
                    case ProjectileOwner::PlayerShip:
                    default:
                        proj->pos.Y = startPos.Y;
                        break;
                }                    
                proj->vel = velocity; // need to adjust the velocity to the raider speed
                proj->blastRadiusSq = radiusSq;
                proj->owner = owner;
                proj->active = true;
            }
        }

        void Update(uint32_t* gameScore = nullptr)
        {
            globalFrameCounter++;
            
            size_t activeRaiderCount = 0;
            for (const auto& raider : raiders.items)
            {
                if (raider.active) activeRaiderCount++;
            }

            size_t activeWaveCount = (activeRaiderCount + waveSize - 1) / waveSize;

            if (activeWaveCount < maxConcurrentWaves)
            {
                if ((globalFrameCounter - lastWaveSpawnFrame) >= waveIntervalFrames)
                {
                    SpawnWave();
                }
            }
            
            // update and fire
            for (auto& raider : raiders.items)
            {
                if (!raider.active) continue;
                
                raider.animFrame++;
                
                switch (raider.type)
                {
                    case RaiderType::Standard:
                    {
                        raider.pos += raider.vel;
                        break;
                    }
                    case RaiderType::ZigZag:
                    {
                        raider.pos.Y += raider.vel.Y;
                        int16_t offset = static_cast<int16_t>((raider.animFrame & 32) ? 4 : -4); // change direction every 32 frames
                        raider.pos.X += offset;
                        break;
                    }
                    case RaiderType::FastDropper:
                    {
                        raider.pos += raider.vel;
                        break;
                    }
                    case RaiderType::Swerve:
                    {
                        raider.pos += raider.vel;
                        break;
                    }
                }
            
                if (raider.pos.Y > -100 && raider.pos.Y < 160) // don't fire unless in this screen range
                {
                    if (rnd.GetNumber(0, 200) < 1) // needs tuning
                    {
                        FireProjectile(
                            raider.pos, 
                            Vector2D(0, 2), // need to vary by type of raider
                            16, 
                            ProjectileOwner::Raider
                        );
                    }
                }

                // raiders die if the reach the bottom of the screen
                if (raider.pos.Y > 180)
                {
                    // draw explosion here!
                    RaiderPointValue pts = GetRaiderPoints(raider.type);

                    if (*gameScore >= pts.basePenalty)
                    {
                        *gameScore -= pts.basePenalty;
                    }
                    else
                    {
                        *gameScore = 0;
                    }

                    raiders.Despawn(&raider);
                }
                    
            }
            
            // check collisions - probably should have kept it as Fxp instead of raw, but I've had overflow issues before
            constexpr int32_t RADIUS_RAW = ATTACK_RADIUS << 16;

            // this is soooo fucking ugly
            // unroll if I get time / motivation
            for (auto& proj : projectiles.items)
            {
                if (!proj.active) continue;
                proj.pos += proj.vel;

                if (proj.pos.Y < -240 || proj.pos.Y > 240)
                {
                    projectiles.Despawn(&proj);
                    continue;
                }

                if (proj.owner == ProjectileOwner::Player || 
                    proj.owner == ProjectileOwner::Turret || 
                    proj.owner == ProjectileOwner::PlayerShip)
                {
                    int32_t projX = proj.pos.X.RawValue();
                    int32_t projY = proj.pos.Y.RawValue();

                    for (auto& raider : raiders.items)
                    {
                        if (!raider.active) continue;

                        int32_t dx = raider.pos.X.RawValue() - projX;
                        if (dx < 0) dx = -dx;

                        if (dx <= RADIUS_RAW)
                        {
                            int32_t dy = raider.pos.Y.RawValue() - projY;
                            if (dy < 0) dy = -dy;

                            if (dy <= RADIUS_RAW)
                            {
                                RaiderPointValue pts = GetRaiderPoints(raider.type);
                                
                                if (UINT32_MAX - *gameScore >= pts.killScore)
                                {
                                    *gameScore += pts.killScore;
                                }
                                else
                                {
                                    *gameScore = UINT32_MAX;
                                }

                                raiders.Despawn(&raider);
                                projectiles.Despawn(&proj);
                                break;
                            }
                        }
                    }
                }
            }
        }
    };

    inline DefenseSystem Defense;
}