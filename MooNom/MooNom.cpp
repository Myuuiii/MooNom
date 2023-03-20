#include <iostream>
#include <memory.h>
#include <thread>

#include "Memory.h"

namespace offset
{
    constexpr ptrdiff_t dwLocalPlayer = 0xDEA964;
    constexpr ptrdiff_t dwEntityList = 0x4DFFEF4;

    constexpr ptrdiff_t model_ambient_min = 0x5A1194;

    constexpr ptrdiff_t m_clrRender = 0x70;
    constexpr ptrdiff_t m_iTeamNum = 0xF4;
}

struct Color
{
    std::uint8_t r{}, g{}, b{};
};

int main(int argc, char* argv[])
{
    const auto memory = Memory("csgo.exe");
    const auto client = memory.GetModuleAddress("client.dll");
    const auto engine = memory.GetModuleAddress("engine.dll");

    float brightness = 25.f;

    constexpr auto team_color = Color{0, 0, 255};
    constexpr auto enemy_color = Color{255, 0, 0};
    
    std::cout << "MooNom is running" << std::endl;
    
    while (true)
    {
        const auto& local_player = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
        const auto& local_team = memory.Read<int>(local_player + offset::m_iTeamNum);

        for (auto i = 1; i <= 32; ++i)
        {
            if (const auto& entity = memory.Read<std::uintptr_t>(client + offset::dwEntityList + i * 0x10); memory.Read<std::int32_t>(entity + offset::m_iTeamNum) == local_team)
                memory.Write<Color>(entity + offset::m_clrRender, team_color);
            else
                memory.Write<Color>(entity + offset::m_clrRender, enemy_color);
            const auto _this = engine + offset::model_ambient_min - 0x2c;
            memory.Write<std::int32_t>(engine + offset::model_ambient_min,
                                       *reinterpret_cast<std::int32_t*>(&brightness) ^ _this);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
