#include "pch-il2cpp.h"
#include "AnimationChanger.h"

#include <helpers.h>
#include <cheat/events.h>
#include <misc/cpp/imgui_stdlib.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature
{
    static std::string animations[] = {
    //  All characters
    "SlipFaceWall",
    "SlipBackWall",
    "DropDown",
    "JumpOffWall",
    "Jump",
    "JumpForRun",
    "JumpForWalk",
    "Fly",
    "FlyStart",
    "JumpForSprint",
    "SwimIdle",
    "SwimMove",
    "SwimDash",
    "ClimbMove1",
    "ClimbIdle",
    "ClimbJump",
    "ClimbMove0",
    "FallToGroundRun",
    "FallOnGroundLit",
    "FallOnGround",
    "FallToGroundRunHard",
    "FallToGroundSprint",
    "Walk",
    "Run",
    "Standby",
    "RunToIdle",
    "RunToWalk",
    "WalkToIdle",
    "WalkToRun",
    "Sprint",
    "SprintToIdle",
    "SprintToRun",
    "ClimbDownToGround",
    "SprintBS",
    "ShowUp",
    "CrouchToStandby",
    "CrouchIdle",
    "CrouchRoll",
    "CrouchMove",
    "SkiffNormal",
    "Upstairs",
    "JumpUpWallForStandby",
    "JumpUpWallReady",
    "Standby2ClimbA",
    "SwimJump",
    "SwimJumpDrop",
    "SwimJumpToWater",
    "Standby2ClimbB",
    "CrouchDrop",
    "TurnDir",
    "StandbyWeapon",
    "StandbyPutaway",
    "StandbyPutawayOver",
    "Icespine_Out",
    "Icespine",
    "LiquidStrike_MoveStandby",
    "LiquidStrike_AS",
    "LiquidStrike_BS",
    "LiquidStrike_BS1",
    "LiquidStrike_Move",
    "LiquidStrike_Strike",
    "LiquidStrike_FatalStandby",
    "LiquidStrike_FatalMove",
    "LiquidStrike_AS_OnWater",
    "LiquidStrike_BS_0",
    "FrozenWindmill",
    "FrozenWindmill_AS",
    "Attack03",
    "Attack04",
    "Attack05",
    "Attack01",
    "Attack02",
    "ExtraAttack",
    "ExtraAttack_AS",
    "ExtraAttack_BS",
    "Katana_Attack03",
    "Katana_Attack04",
    "Katana_Attack02",
    "Katana_Attack01",
    "Katana_Attack05",
    "FallingAnthem_Loop",
    "FallingAnthem_AS_2",
    "FallingAnthem_BS_1",
    "FallingAnthem_BS_2",
    "FallingAnthem_AS_1",
    "FallingAnthem_Loop_Low",
    "SitBDown",
    "SitBLoop",
    "SitBUp",
    "SitDown",
    "SitLoop",
    "SitUp",
    "StandbyShow_01",
    "StandbyShow_02",
    "StandbyVoice",
    "Think01BS",
    "Think01Loop",
    "Think01AS",
    "Akimbo02BS",
    "Akimbo02Loop",
    "Akimbo02AS",
    "ChannelBS",
    "ChannelLoop",
    "ChannelAS",
    "PlayMusic_Lyre_AS",
    "PlayMusic_Lyre_BS",
    "PlayMusic_Lyre_Loop",
    "PlayMusic_Qin_BS",
    "PlayMusic_Qin_AS",
    "PlayMusic_Qin_Loop",
    "PlayMusic_Drum_BS",
    "PlayMusic_Drum_AS",
    "PlayMusic_Drum_Loop",
    "ActivitySkill_ElectricCoreFly",
    "ActivitySkill_GrapplingHook",
    "Hit_H",
    "Hit_L",
    "Hit_Throw",
    "Hit_Throw_Ground",
    "Hit_ThrowAir",
    "Struggle",
    "NormalDie",
    "SwimDie",
    "HitGroundDie",
    "FallDie_AS",
    "FallDie",
    //  Main Character only
    "UziExplode_AS",
    "UziExplode_BS",
    "UziExplode_Charge_01",
    "UziExplode_Strike_02",
    "UziExplode_Charge_02",
    "UziExplode_Strike_01",
    "UziExplode_BS_1",
    "WindBreathe_AS",
    "WindBreathe",
    "Hogyoku_AS",
    "Hogyoku_BS",
    "Hogyoku",
    "Hogyoku_Charge",
    "Hogyoku_Charge_AS",
    "Hogyoku_Charge_2",
    "RockTide_AS",
    "RockTide",
    "CrouchThrowBS",
    "CrouchThrowLoop",
    "CrouchThrowAS",
    "FindCatThrowBS",
    "FindCatThrowLoop",
    "FindCatThrowAS",
    "Player_Electric_ElementalArt",
    "Player_Electric_ElementalArt_AS",
    "Player_Electric_ElementalBurst",
    "Player_Electric_ElementalBurst_AS",
    "PutHand01BS",
    "PutHand01Loop",
    "PutHand01AS",
    "Akimbo01BS",
    "Backrake01BS",
    "Forerake01BS",
    "StrikeChest01BS",
    "Akimbo01Loop",
    "Akimbo01AS",
    "Backrake01Loop",
    "Backrake01AS",
    "Forerake01Loop",
    "Forerake01AS",
    "StrikeChest01Loop",
    "StrikeChest01AS",
    "HoldHead01BS",
    "HoldHead01Loop",
    "HoldHead01AS",
    "Clap01",
    "Turn01_90LBS",
    "Turn01_90RBS",
    "Turn01_90LAS",
    "Turn01_90RAS",
    "Alert01BS",
    "Alert01Loop",
    "Alert01AS",
    "Fishing01_BS",
    "Fishing01Loop",
    "Fishing01AS",
    "Think01_BS",
    "Think01_Loop",
    "Think01_AS",
    "Channel01BS",
    "Channel01Loop",
    "Channel01AS",
    "Fishing_Battle_BS",
    "Fishing_Cast_AS",
    "Fishing_Cast_BS",
    "Fishing_Cast_Loop",
    "Fishing_Choose",
    "Fishing_Choose_Loop",
    "Fishing_End",
    "Fishing_Pull_01",
    "Fishing_Pull_02",
    "Fishing_Wait",
    "Fishing_Pull_Fail",
    "Bartender_MixingStandby",
    "Bartender_MixingStart",
    "Bartender_MixingToPour",
    "Bartender_Pour",
    "Bartender_PourFinish",
    "Bartender_PourStandby",
    "Bartender_AddLoop",
    "Bartender_PrepareStart",
    "Bartender_Standby",
    "Bartender_AddStandby",
    "Bartender_PrepareToStandby",
    "Bartender_StandbyFinish",
    "Blocking_BS",
    "Blocking_Loop",
    "Blocking_Back",
    "Blocking_Bounce",
    "Blocking_Hit",
    "Blocking_AS"
    };

    AnimationChanger::AnimationChanger() : Feature(),
        NF(f_Enabled, u8"�Զ��嶯������", "Visuals::AnimationChanger", false),
        NF(f_Animation, u8"����", "Visuals::AnimationChanger", "ExtraAttack"),
        NF(f_ApplyKey, u8"Ӧ��", "Visuals::AnimationChanger", Hotkey('Y')),
        NF(f_ResetKey, u8"����", "Visuals::AnimationChanger", Hotkey('R'))
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(AnimationChanger::OnGameUpdate);
    }

    const FeatureGUIInfo& AnimationChanger::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"�Զ��嶯��", u8"ģ����Ӿ���", false };
        return info;
    }

    void AnimationChanger::DrawMain()
    {
        ImGui::BeginGroupPanel(u8"�Զ��嶯��");
        {
            ConfigWidget(u8"�Զ��嶯��", f_Enabled, u8"���Ļ��ɫ�Ķ�����\n ע�� ����Ҫ��ɫ�⣬���ж�����������ÿ����ɫ.");
            if (f_Enabled)
            {
                if (ImGui::BeginCombo(u8"����", f_Animation.value().c_str()))
                {
                    for (auto& animation : animations)
                    {
                        bool is_selected = (f_Animation.value().c_str() == animation);
                        if (ImGui::Selectable(animation.c_str(), is_selected))
                            f_Animation.value() = animation;

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                ConfigWidget(u8"Ӧ��", f_ApplyKey, true);
                ConfigWidget(u8"����", f_ResetKey, true);
            }
        }
        ImGui::EndGroupPanel();
    }

    bool AnimationChanger::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void AnimationChanger::DrawStatus()
    {
        ImGui::Text(u8"�Զ��嶯��");
    }

    AnimationChanger& AnimationChanger::GetInstance()
    {
        static AnimationChanger instance;
        return instance;
    }

    void AnimationChanger::OnGameUpdate()
    {
        if (!f_Enabled)
            return;

        // Taiga#5555: Maybe need to add separate option to change delay value if user feels like it's too fast or slow.
        UPDATE_DELAY(400);

        auto& manager = game::EntityManager::instance();
        auto avatar = manager.avatar();
        if (avatar->animator() == nullptr)
            return;

        if (f_ApplyKey.value().IsPressed())
            app::Animator_Play(avatar->animator(), string_to_il2cppi(f_Animation.value().c_str()), 0, 0, nullptr);

        if (f_ResetKey.value().IsPressed())
            app::Animator_Rebind(avatar->animator(), nullptr);
    }
}
