#pragma once
#include "../../main.hpp"

namespace Display::Background
{
    // VDP2 linescroll (nbg0 & nbg1)
    class Linescroll final
    {
    public:
        // static uint32_t getScreenHeight() { 
            // return static_cast<uint32_t>(PPE::Screen::Height()); 
        // }
        
        // Max buffer allocation for Saturn VDP2 line bounds
        static constexpr uint32_t MaxScreenHeight = 240; 
        static constexpr uintptr_t Nbg0VramAddress = 0x25E70000; // each table is 960 bytes (4 bytes per 240 lines) or 0x3C0
        static constexpr uintptr_t Nbg1VramAddress = 0x25E70780; // hard-coded for now, but there was nothing here in memory

        inline void initializeNbg0(void* vramTableAddress = reinterpret_cast<void*>(Nbg0VramAddress))
        {
            vramTableAddr = vramTableAddress;
            slLineScrollMode(scnNBG0, lineSZ1 | lineHScroll);
            slLineScrollTable0(vramTableAddr);
        }
        
        inline void initializeNbg1(void* vramTableAddress = reinterpret_cast<void*>(Nbg1VramAddress))
        {
            vramTableAddr = vramTableAddress;
            slLineScrollMode(scnNBG1, lineSZ1 | lineHScroll);
            slLineScrollTable1(vramTableAddr); // use the same table since I won't use them at the same time (for now)
        }

        inline void update()
        {
            if (!vramTableAddr) return;

            for (uint32_t i = 0; i < waterHeight; ++i)
            {
                lineBuffer[i] = 0;
            }

            // sine wave offsets for water effect
            Angle currentPhase = wavePhase;
            for (uint32_t i = waterHeight; i < MaxScreenHeight; ++i)
            {
                Fxp sineVal = SRL::Math::Trigonometry::Sin(currentPhase);
                lineBuffer[i] = sineVal * amplitude;

                currentPhase += spatialFrequency;
            }

            // wave animation
            wavePhase += waveSpeed;

            // copy from RAM buffer into VRAM
            auto* vramDest = reinterpret_cast<volatile int32_t*>(
                reinterpret_cast<uintptr_t>(vramTableAddr)
            );
            const auto* src = reinterpret_cast<const int32_t*>(lineBuffer);

            for (uint32_t i = 0; i < MaxScreenHeight; ++i)
            {
                vramDest[i] = src[i];
            }
        }
        
        // reset linecsroll table (so foreground isn't distorted)
        inline void Reset()
        {
            if (!vramTableAddr) return;

            for (uint32_t i = 0; i < MaxScreenHeight; ++i)
            {
                lineBuffer[i] = 0;
            }

            auto* vramDest = reinterpret_cast<volatile int32_t*>(
                reinterpret_cast<uintptr_t>(vramTableAddr)
            );
            const auto* src = reinterpret_cast<const int32_t*>(lineBuffer);

            for (uint32_t i = 0; i < MaxScreenHeight; ++i)
            {
                vramDest[i] = src[i];
            }
        }

        void setWaterHeight(uint32_t pixelHeight) { waterHeight = MaxScreenHeight - pixelHeight; }

        void setWaveAmplitude(Fxp amplitude) { amplitude = amplitude; }

        void setWaveSpeed(Angle speed) { waveSpeed = speed; }

    private:
        void* vramTableAddr{ nullptr };
        uint32_t waterHeight{ 240 };

        Fxp amplitude{ Fxp::BuildRaw(3 << 16) };
        Angle wavePhase{ Angle::FromDegrees(0.0) };
        Angle waveSpeed{ Angle::FromDegrees(4.0) };
        Angle spatialFrequency{ Angle::FromDegrees(12.0) };

        alignas(4) Fxp lineBuffer[MaxScreenHeight]{}; // not sure I need to do this but it probably doesn't hurt
    };
}