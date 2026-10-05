#pragma once
#include <srl.hpp>
#include "atlantis_defense.hpp"

namespace Atlantis
{
    struct TurretConfig
    {
        Fxp arcRadius = 120;
        Angle angularSpeed = Angle::FromRadians(0.03);
        Fxp projSpeed = 7;
        int fireCadence = 32;
        int inertiaMaxFrames = 30;
        Fxp trackWidth = 128; // how many pixels wide the turret scans
    };

    template <typename RaiderContainer>
    struct TurretUpdateContext
    {
        const RaiderContainer& raiders;
        Vector2D position;
        Fxp cameraX;
        Fxp halfWidth;
        Fxp halfHeight;
        int currentFrame;
    };

    class AutoTurret
    {
    private:
        int sweepDir = 1;
        int inertiaFrames = 0;
        Angle currentAngle = Angle::ThreeQuarterPi();

    public:
        TurretConfig config;

        AutoTurret() = default;

        void Reset(void)
        {
            sweepDir = 1;
            inertiaFrames = 0;
            currentAngle = Angle::ThreeQuarterPi();
        }

        Angle GetCurrentAngle(void) const { return currentAngle; }

        template <typename RaiderContainer>
        bool Update(
            const TurretUpdateContext<RaiderContainer>& ctx,
            Vector2D& outCursor,
            Vector2D& outFireDir)
        {
            if (inertiaFrames > 0)
            {
                inertiaFrames--;
            }

            int32_t bestScore = -999999;
            Vector2D bestInterceptDir(0, 0);
            bool targetFound = false;

            const Fxp minX = ctx.position.X - config.trackWidth;
            const Fxp maxX = ctx.position.X + config.trackWidth;

            for (const auto& raider : ctx.raiders)
            {
                if (!raider.active) continue;

                if (raider.pos.X < minX || raider.pos.X > maxX) continue;

                Vector2D targetWorld(raider.pos.X, raider.pos.Y);
                Vector2D turretToTarget = targetWorld - ctx.position;

                int32_t dx = turretToTarget.X.template As<int32_t>();
                int32_t dy = turretToTarget.Y.template As<int32_t>();
                uint32_t distSq = static_cast<uint32_t>(dx * dx + dy * dy);
                uint32_t approxDist = Integer::FastSqrt(distSq);

                Fxp flightTime = Fxp::BuildRaw(approxDist << 16) / config.projSpeed; // should just make everything Fxp

                Vector2D interceptWorld = targetWorld + (raider.vel * flightTime);
                Vector2D interceptDir = interceptWorld - ctx.position;

                // prioritize raiders that are closest to the ground
                int32_t score = raider.pos.Y.template As<int32_t>() * 4;

                if (score > bestScore)
                {
                    bestScore = score;
                    targetFound = true;
                    bestInterceptDir = interceptDir;
                }
            }

            if (targetFound)
            {
                Angle targetAngle = Trigonometry::Atan2(bestInterceptDir.Y, bestInterceptDir.X);
                Angle angleDelta = targetAngle - currentAngle;

                if (angleDelta.RawValue() != 0)
                {
                    if (angleDelta.RawValue() < 0x8000) // right sweep
                    {
                        if (sweepDir != 1)
                        {
                            sweepDir = 1;
                            inertiaFrames = config.inertiaMaxFrames;
                        }

                        if (angleDelta.RawValue() <= config.angularSpeed.RawValue())
                            currentAngle = targetAngle;
                        else
                            currentAngle = currentAngle + config.angularSpeed;
                    }
                    else // left sweep
                    {
                        if (sweepDir != -1)
                        {
                            sweepDir = -1;
                            inertiaFrames = config.inertiaMaxFrames;
                        }

                        if ((0x10000 - angleDelta.RawValue()) <= config.angularSpeed.RawValue())
                            currentAngle = targetAngle;
                        else
                            currentAngle = currentAngle - config.angularSpeed;
                    }
                }
            }

            // find arc position in world space
            Vector2D arcWorldPos(
                ctx.position.X + (Trigonometry::Cos(currentAngle) * config.arcRadius),
                ctx.position.Y + (Trigonometry::Sin(currentAngle) * config.arcRadius)
            );

            // Fire!!
            bool shouldFire = false;
            if (targetFound && (ctx.currentFrame % config.fireCadence == 0))
            {
                Vector2D dir = arcWorldPos - ctx.position;
                
                int32_t dirX = dir.X.template As<int32_t>();
                int32_t dirY = dir.Y.template As<int32_t>();
                uint32_t lenSq = static_cast<uint32_t>(dirX * dirX + dirY * dirY);
                uint32_t approxLen = Integer::FastSqrt(lenSq);

                if (approxLen > 0)
                {
                    Fxp lenFxp = Fxp::BuildRaw(approxLen << 16);
                    outFireDir.X = (dir.X / lenFxp) * config.projSpeed;
                    outFireDir.Y = (dir.Y / lenFxp) * config.projSpeed;
                    shouldFire = true;
                }
            }

            // so projectiles start from correct location on screen
            outCursor.X = arcWorldPos.X - ctx.cameraX;
            outCursor.Y = arcWorldPos.Y;

            outCursor.X = Fxp::Clamp(outCursor.X, -ctx.halfWidth, ctx.halfWidth);
            outCursor.Y = Fxp::Clamp(outCursor.Y, -ctx.halfHeight, ctx.halfHeight);

            return shouldFire;
        }
    };
    
    // need a way to blow up or move a turret
    struct TurretManager
    {
        template <typename PlacedTurretType, typename DefenseType, size_t N>
        static void Update(
            PlacedTurretType (&turrets)[N], 
            DefenseType& defense, 
            Fxp cameraX, 
            Fxp halfWidth, 
            Fxp halfHeight, 
            int currentFrame)
        {
            for (size_t i = 0; i < N; ++i)
            {
                auto& turret = turrets[i];
                if (!turret.active) continue;

                TurretUpdateContext ctx{
                    .raiders = defense.raiders.items,
                    .position = turret.worldPos,
                    .cameraX = cameraX,
                    .halfWidth = halfWidth,
                    .halfHeight = halfHeight,
                    .currentFrame = currentFrame
                };

                Vector2D fireDir;
                if (turret.ai.Update(ctx, turret.cursorPos, fireDir))
                {
                    defense.FireProjectile(
                        turret.worldPos, 
                        fireDir, 
                        24, 
                        Atlantis::ProjectileOwner::Turret
                    );
                }
            }
        }

        template <typename PlacedTurretType, size_t N>
        static int GetActiveCount(const PlacedTurretType (&turrets)[N])
        {
            int count = 0;
            for (size_t i = 0; i < N; ++i)
            {
                if (turrets[i].active) count++;
            }
            return count;
        }
    };
}