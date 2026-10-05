#pragma once
#include <srl.hpp>
#include "ppe_screen.hpp"

namespace Atlantis
{
    using namespace SRL::Math::Types;

    struct Camera
    {
        Fxp x = 0; 
        Fxp y = 0;
        
        static constexpr int32_t WORLD_WIDTH  = 2048;
        static constexpr int32_t MAP_WIDTH    = 1024;
        static constexpr int32_t VIEW_WIDTH   = 704;
        static constexpr int32_t MAX_CAMERA_X = WORLD_WIDTH - VIEW_WIDTH; // 1344 (not sure if this is right, I think the world center is slightly off

        // so I can separate layer scroll position from world scroll position
        int32_t cachedNbg0Scroll = 0;
        int32_t cachedNbg1Scroll = 0;
        int32_t cachedNbg2Scroll = 0;
        
        static inline Fxp Nbg2YScroll = 0;
        
        static inline Fxp GetHalfWidth()  { return PPE::Screen::HalfWidthFxp(); }  // 352
        static inline Fxp GetHalfHeight() { return PPE::Screen::HalfHeightFxp(); } // 240

        void SetPosition(Fxp newX)
        {
            x = Fxp::Clamp(newX, 0, MAX_CAMERA_X);
            UpdateScrollPosition();
        }
        
        // call this whenever water height changes!
        void SetWaterYScroll(uint32_t waterLevel)
        {
            Nbg2YScroll = Fxp::BuildRaw(((waterLevel / 1) - 64) << 16);
            slScrPosNbg2(cachedNbg2Scroll << 16, Nbg2YScroll.RawValue());
        }

        void ScrollX(Fxp deltaX)
        {
            SetPosition(x + deltaX);
        }

        void Reset(Fxp initialWorldCenterX = 1024, Fxp initialY = 0)
        {
            // I think this broke again, NBG0 is in the wrong initial location
            x = Fxp::Clamp(initialWorldCenterX - GetHalfWidth(), 0, MAX_CAMERA_X);
            y = initialY;
            UpdateScrollRegisters();
        }

        // VDP1 uses 0,0 as the center of the screen so the top left is -352x-240, down is positive
        // but VDP2 uses 0,0 as the top left edge of the screen.  the math is really annoying
        
        // VDP1 to VDP2
        inline Fxp ScreenToWorldX(Fxp screenX) const
        {
            return x + GetHalfWidth() + screenX;
        }

        inline Fxp ScreenToWorldY(Fxp screenY) const
        {
            return screenY + GetHalfHeight();
        }

        template <typename Vec2Type>
        inline void ScreenToWorld(const Vec2Type& screenPos, Fxp& outWorldX, Fxp& outWorldY) const
        {
            outWorldX = ScreenToWorldX(screenPos.X);
            outWorldY = ScreenToWorldY(screenPos.Y);
        }

    private:
        inline void UpdateScrollPosition()
        {
            int32_t camXInt = x.As<int32_t>();
            
            if (camXInt < 0) camXInt = 0;
            if (camXInt > MAX_CAMERA_X) camXInt = MAX_CAMERA_X;

            // NBG1: 2048x480px (half speed)
            cachedNbg1Scroll = camXInt;

            // NBG0: 2048x480px (half of NBG1)
            cachedNbg0Scroll = camXInt >> 1;

            // NBG2: water screen (can't scale)
            cachedNbg2Scroll = camXInt >> 2;

            // Should use SRL functions instead - then I don't need to do Fxp to FIXED conversion
            slScrPosNbg0(cachedNbg0Scroll << 16, 0);
            slScrPosNbg1(cachedNbg1Scroll << 16, 0);
            slScrPosNbg2(cachedNbg2Scroll << 16, Nbg2YScroll.RawValue());
        }
    };

    inline Camera GameCamera;
}