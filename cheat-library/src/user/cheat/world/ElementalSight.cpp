#include "pch-il2cpp.h"
#include "ElementalSight.h"

#include <helpers.h>

namespace cheat::feature
{
    static void LevelSceneElementViewPlugin_Tick_Hook(app::LevelSceneElementViewPlugin* __this, float inDeltaTime, MethodInfo* method);

    ElementalSight::ElementalSight() : Feature(),
        NF(f_Enabled, u8"Ԫ����Ұ", u8"Ԫ����Ұ", false)
    {
        HookManager::install(app::MoleMole_LevelSceneElementViewPlugin_Tick, LevelSceneElementViewPlugin_Tick_Hook);
    }

    const FeatureGUIInfo& ElementalSight::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", u8"������", false };
        return info;
    }

    void ElementalSight::DrawMain()
    {
        ConfigWidget(u8"Ԫ����Ұ", f_Enabled, u8"Ԫ�����߼�ʹ���ƶ�ʱҲ�ᱣ��.\n"
                     u8"Ҫ�رգ���رղ��ٴ�ʹ��Ԫ������.");
    }

    bool ElementalSight::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void ElementalSight::DrawStatus()
    {
        ImGui::Text(u8"Ԫ����Ұ");
    }

    ElementalSight& ElementalSight::GetInstance()
    {
        static ElementalSight instance;
        return instance;
    }

    static void LevelSceneElementViewPlugin_Tick_Hook(app::LevelSceneElementViewPlugin* __this, float inDeltaTime, MethodInfo* method)
    {
        ElementalSight& ElementalSight = ElementalSight::GetInstance();
        if (ElementalSight.f_Enabled)
            __this->fields._triggerElementView = true;
        CALL_ORIGIN(LevelSceneElementViewPlugin_Tick_Hook, __this, inDeltaTime, method);
    }
}

