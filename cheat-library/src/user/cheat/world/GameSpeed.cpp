#include "pch-il2cpp.h"
#include "GameSpeed.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature
{
    GameSpeed::GameSpeed() : Feature(),
        NF(f_Enabled, u8"����", u8"ȫ�ֱ���", false),
        NF(f_Hotkey, u8"��Ϸ�ٶ��ȼ�", u8"ȫ�ֱ���", Hotkey(VK_CAPITAL)),
        NF(f_Speed, u8"�ٶȱ���", u8"ȫ�ֱ���", 5.0f)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(GameSpeed::OnGameUpdate);
    }

    const FeatureGUIInfo& GameSpeed::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"ȫ�ֱ���", "World", true };
        return info;
    }

    void GameSpeed::DrawMain()
    {
        ConfigWidget(u8"����", f_Enabled, u8"ʹ���ȼ�������Ϸ");
        if (f_Enabled)
        {
            ConfigWidget(u8"��Ϸ�ٶ��ȼ�", f_Hotkey);
            ConfigWidget(f_Speed, 1.0f, 0.0f, 20.0f, u8"������Ϸ�ٶȳ���\n" \
                u8"��Ҫ�ڿ���������ʹ�ã����ڲ˵�/����ʹ�ã��ǳ�Σ��!");
        }
    }

    bool GameSpeed::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void GameSpeed::DrawStatus()
    {
        ImGui::Text(u8"ȫ�ּ���");
    }

    GameSpeed& GameSpeed::GetInstance()
    {
        static GameSpeed instance;
        return instance;
    }

    void GameSpeed::OnGameUpdate()
    {
        static bool isSpeed = false;
        float currentSpeed = app::Time_get_timeScale(nullptr);

        if (f_Enabled)
        {
            if (f_Hotkey.value().IsPressed() && !isSpeed)
            {
                if (currentSpeed == 1.0f)
                {
                    app::Time_set_timeScale(f_Speed, nullptr);
                    isSpeed = true;
                }
            }

            if (!f_Hotkey.value().IsPressed() && isSpeed)
            {
                if (currentSpeed != 1.0f)
                {
                    app::Time_set_timeScale(1.0f, nullptr);
                    isSpeed = false;
                }
            }
        }
        else
        {
            // Aditional check if user is still pressing key and they decide to disable the feature
            if (isSpeed)
            {
                if (currentSpeed != 1.0f)
                {
                    app::Time_set_timeScale(1.0f, nullptr);
                    isSpeed = false;
                }
            }
        }
    }
}
