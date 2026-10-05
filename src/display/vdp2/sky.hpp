#pragma once
#include "../../main.hpp"

// based on Reyme's code from quadworld
// stripped out the vertical adjustment, then tried to hack it back in..
// probably just need to go back to his original version
namespace Display::Background
{
    // draws gradient on backscreen
    // title screen background draws on top of it
    class Sky final
    {
    public:
        static void SetGradient(int16_t offsetY = 0, bool isBlack = false)
        {
            uint16_t lineColorTable[256];

            if (isBlack)
            {
                // fill with black
                for (int16_t line = 0; line < 255; line++)
                {
                    lineColorTable[line] = 0x0000;
                }
            }
            else
            {
                for (int16_t line = 0; line < 255; line++)
                {
                    int16_t colorId = ((line + offsetY) * 180) / 256;

                    if (colorId < 0)
                    {
                        colorId = 0;
                    }
                    else if (colorId >= 180)
                    {
                        colorId = 179;
                    }

                    lineColorTable[line] = Sky::GradientCache[colorId];
                }
            }

            // transfer table to VDP2
            slDMACopy((void*)lineColorTable, (void*)Sky::LineColorTable, sizeof(lineColorTable));
        }

        static void Initialize()
        {
            slBackColTable((void*)Sky::LineColorTable);
            Sky::SetGradient(0, false);
        }

        static void SetBlackout(bool isBlack)
        {
            Sky::SetGradient(0, isBlack);
        }

    private:
        struct GradientPoint
        {
            Fxp Anchor = 0;
            Vector3D Color = Vector3D();
        };

        static inline volatile uint16_t* LineColorTable = (volatile uint16_t*)(VDP2_VRAM_A0 + 0x1f400);

        static constexpr const auto GradientCache{
            []() constexpr {

                constexpr GradientPoint gradientTable[] = 
                {
                    { 0.00, Vector3D(5, 12, 35) },
                    { 0.35, Vector3D(0, 85, 120) },
                    { 0.55, Vector3D(210, 180, 110) },
                    { 0.75, Vector3D(40, 140, 160) },
                    { 1.00, Vector3D(15, 50, 90) }
                };

                const auto colorConvert = [](const Vector3D& color)
                {
                    return HighColor(color.X.As<int16_t>(), color.Y.As<int16_t>(), color.Z.As<int16_t>());
                };

                const auto lerp = [](const Vector3D& first, const Vector3D& second, const Fxp& time) {
                    return Vector3D(
                        first.X + (second.X - first.X) * time,
                        first.Y + (second.Y - first.Y) * time,
                        first.Z + (second.Z - first.Z) * time);
                };

                std::array<HighColor, 180> colorTable{};

                for (int degrees = 0; degrees < 180; degrees++)
                {
                    Fxp angle = degrees / 180.0;
                    colorTable[degrees] = HighColor(0, 0, 0);

                    if (angle <= gradientTable[0].Anchor)
                    {
                        colorTable[degrees] = colorConvert(gradientTable[0].Color);
                    }
                    else if (angle >= gradientTable[4].Anchor)
                    {
                        colorTable[degrees] = colorConvert(gradientTable[4].Color);
                    }
                    else
                    {
                        for (int anchor = 0; anchor < (int)(sizeof(gradientTable) / sizeof(GradientPoint)) - 1; anchor++)
                        {
                            if (angle >= gradientTable[anchor].Anchor && angle <= gradientTable[anchor + 1].Anchor)
                            {
                                const auto range = gradientTable[anchor + 1].Anchor - gradientTable[anchor].Anchor;
                                const auto localAngle = (angle - gradientTable[anchor].Anchor) / range;
                                const auto color = lerp(gradientTable[anchor].Color, gradientTable[anchor + 1].Color, localAngle);

                                colorTable[degrees] = colorConvert(color);
                            }
                        }
                    }
                }

                return colorTable;
            }()
        };
    };
}