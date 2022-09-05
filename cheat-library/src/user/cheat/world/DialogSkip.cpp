    #include "pch-il2cpp.h"
#include "DialogSkip.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature
{
    static void InLevelCutScenePageContext_UpdateView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method);
    static void InLevelCutScenePageContext_ClearView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method);
    static void CriwareMediaPlayer_Update(app::CriwareMediaPlayer* __this, MethodInfo* method);

    DialogSkip::DialogSkip() : Feature(),
        NF(f_Enabled, "Auto talk", "AutoTalk", false),
        NF(f_AutoSelectDialog, "Auto select dialog", "AutoTalk", true),
        NF(f_ExcludeImportant, "Exclude Katheryne/Tubby/Wagner", "AutoTalk", true),
        NF(f_FastDialog, "Fast dialog", "AutoTalk", false),
        NF(f_CutsceneUSM, "Skip Cutscenes", "AutoTalk", false),
        NF(f_TimeSpeedup, u8"速度", "AutoTalk", 5.0f)
    {
        HookManager::install(app::MoleMole_InLevelCutScenePageContext_UpdateView, InLevelCutScenePageContext_UpdateView_Hook);
        HookManager::install(app::MoleMole_InLevelCutScenePageContext_ClearView, InLevelCutScenePageContext_ClearView_Hook);
        HookManager::install(app::CriwareMediaPlayer_Update, CriwareMediaPlayer_Update);
    }

    const FeatureGUIInfo& DialogSkip::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"自动剧情", u8"大世界", true };
        return info;
    }

    void DialogSkip::DrawMain()
    {
        ConfigWidget(u8"开/关", f_Enabled, u8"自动继续对话框.");
        ConfigWidget(u8"自动选择选项", f_AutoSelectDialog, u8"自动选择对话框选项.");
        if (f_AutoSelectDialog)
        {
            ImGui::Indent();
            ConfigWidget(u8"不包括 凯瑟琳/壶灵/瓦格纳", f_ExcludeImportant, u8"从自动选择中排除凯瑟琳/壶灵/瓦格纳.");
            ImGui::Unindent();
        }
        ConfigWidget(u8"快速对话", f_FastDialog, u8"Speeds up Time");
        if (f_FastDialog)
        {
            ConfigWidget(f_TimeSpeedup, 0.1f, 2.0f, 50.0f, u8"时间加速倍数\n较高的值将导致与服务器的同步问题，不建议延迟Internet连接.");
        }
        ConfigWidget(u8"跳过动画", f_CutsceneUSM, u8"自动跳过游戏电影.");
    }

    bool DialogSkip::NeedStatusDraw() const
    {
        return f_Enabled || f_CutsceneUSM;
    }

    void DialogSkip::DrawStatus()
    {
        if (f_Enabled)
            ImGui::Text(u8"快速剧情 [%s%s%s%s%s]",
                f_AutoSelectDialog ? "Auto" : "Manual",
                f_AutoSelectDialog && (f_ExcludeImportant || f_FastDialog) ? "|" : "",
                f_ExcludeImportant ? "Exc" : "",
                f_ExcludeImportant && f_FastDialog ? "|" : "",
                f_FastDialog ? "Fast" : "Normal");

        ImGui::Text(f_CutsceneUSM ? "Skip Cutscenes" : "");
    }

    DialogSkip& DialogSkip::GetInstance()
    {
        static DialogSkip instance;
        return instance;
    }

    // Raised when dialog view updating
    // We call free click, if auto talk enabled, that means we just emulate user click
    // When appear dialog choose we create notify with dialog select first item.
    void DialogSkip::OnCutScenePageUpdate(app::InLevelCutScenePageContext* context)
    {
        if (!f_Enabled)
            return;

        auto talkDialog = context->fields._talkDialog;
        if (talkDialog == nullptr)
            return;

        if (f_FastDialog)
            app::Time_set_timeScale(f_TimeSpeedup, nullptr);
        else
            app::Time_set_timeScale(1.0f, nullptr);

        bool isImportant = false;
        if (f_ExcludeImportant)
        {
            // TODO: Add a custom filter in the future where users can
            // add their own name substrings of entities to avoid
            // speeding up dialog on.
            std::vector<std::string> impEntitiesNames = {
                "Djinn",
                "Katheryne",
                "Wagner",
                "Ahangar"
            };
            auto dialogPartnerID = context->fields._inteeID;
            auto& manager = game::EntityManager::instance();
            auto dialogPartner = manager.entity(dialogPartnerID);
            auto dialogPartnerName = dialogPartner->name();
            for (auto impEntityName : impEntitiesNames)
            {
                if (dialogPartnerName.find(impEntityName) != -1) {
                    LOG_DEBUG("%s %s %d", dialogPartnerName.c_str(), impEntityName, dialogPartnerName.find(impEntityName));
                    isImportant = true;
                    break;
                }
            }
        }

        if (talkDialog->fields._inSelect && f_AutoSelectDialog && !isImportant)
        {
            int32_t value = 0;
            auto object = il2cpp_value_box((Il2CppClass*)*app::Int32__TypeInfo, &value);
            app::Notify notify{};
            notify.type = app::MoleMole_NotifyTypes__Enum::DialogSelectNotify;
            notify.body = (app::Object*)object;
            app::MoleMole_TalkDialogContext_OnDialogSelectItem(talkDialog, &notify, nullptr);
        }
        else if (!talkDialog->fields._inSelect)
            app::MoleMole_InLevelCutScenePageContext_OnFreeClick(context, nullptr);
    }

    static void InLevelCutScenePageContext_UpdateView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method)
    {
        CALL_ORIGIN(InLevelCutScenePageContext_UpdateView_Hook, __this, method);

        DialogSkip& dialogSkip = DialogSkip::GetInstance();
        dialogSkip.OnCutScenePageUpdate(__this);
    }

    // Raised when exiting a dialog. We try to hackishly return to normal value.
    // Should be a better way to store the pre-dialog speed using Time_get_timeScale.
    static void InLevelCutScenePageContext_ClearView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method)
    {
        float gameSpeed = app::Time_get_timeScale(nullptr);
        if (gameSpeed > 1.0f)
            app::Time_set_timeScale(1.0f, nullptr);
        CALL_ORIGIN(InLevelCutScenePageContext_ClearView_Hook, __this, method);
    }

    static void CriwareMediaPlayer_Update(app::CriwareMediaPlayer* __this, MethodInfo* method)
    {
        DialogSkip& dialogSkip = DialogSkip::GetInstance();
        if (dialogSkip.f_CutsceneUSM)
            app::CriwareMediaPlayer_Skip(__this, nullptr);

        return CALL_ORIGIN(CriwareMediaPlayer_Update, __this, method);
    }
}

