#include "pch-il2cpp.h"
#include "MobVacuum.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>

namespace cheat::feature 
{
    MobVacuum::MobVacuum() : Feature(),
        NF(f_Enabled,        "Mob vacuum", "MobVacuum", false),
        NF(f_IncludeMonsters, u8"��������", "MobVacuum", true),
        NF(f_MonsterCommon, u8"��ͨ��", "MobVacuum", true),
        NF(f_MonsterElites, u8"��Ӣ��", "MobVacuum", true),
        NF(f_MonsterBosses, "Boss", "MobVacuum", true),
        NF(f_IncludeAnimals, u8"��������", "MobVacuum", true),
        NF(f_AnimalDrop, u8"�ɹ�������", "MobVacuum", true),
        NF(f_AnimalPickUp, u8"�ɲɼ�����", "MobVacuum", true),
        NF(f_AnimalNPC, "NPCs", "MobVacuum", true),
        NF(f_Speed, "Speed", "MobVacuum", 2.5f),
        NF(f_Distance, "Distance", "MobVacuum", 1.5f),
        NF(f_Radius, "Radius", "MobVacuum", 10.0f),
        NF(f_OnlyTarget, "Only targeted", "MobVacuum", true),
        NF(f_Instantly,  "Instantly",     "MobVacuum", false),
        NF(f_SetCollider, "SetCollider", "MobVacuum", false)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(MobVacuum::OnGameUpdate);
        events::MoveSyncEvent += MY_METHOD_HANDLER(MobVacuum::OnMoveSync);
    }

    const FeatureGUIInfo& MobVacuum::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"��������", "World", true };
        return info;
    }

    void MobVacuum::DrawMain()
    {
        ConfigWidget(u8"��/��", f_Enabled, u8"ʹ����������.\n" \
            "Mobs within the specified radius will move\nto a specified distance in front of the player.");

        bool filtersChanged = false;
        ImGui::BeginGroupPanel(u8"����");
        {
            filtersChanged |= ConfigWidget(f_IncludeMonsters, u8"��������.");
            filtersChanged |= ConfigWidget(f_MonsterCommon, u8"С��."); ImGui::SameLine();
            filtersChanged |= ConfigWidget(f_MonsterElites, u8"��Ӣ��."); ImGui::SameLine();
            filtersChanged |= ConfigWidget(f_MonsterBosses, u8"boss��.");
        }
        ImGui::EndGroupPanel();
        
        ImGui::BeginGroupPanel(u8"����");
        {
            filtersChanged |= ConfigWidget(f_IncludeAnimals, u8"��������.");
            filtersChanged |= ConfigWidget(f_AnimalDrop, u8"��Ҫ��ɱ�Ķ�����."); ImGui::SameLine();
            filtersChanged |= ConfigWidget(f_AnimalPickUp, u8"����ֱ�Ӳɼ��Ķ�����."); ImGui::SameLine();
            filtersChanged |= ConfigWidget(f_AnimalNPC, "Animals without mechanics.");
        }
        ImGui::EndGroupPanel();

        if (filtersChanged)
            UpdateFilters();

    	ConfigWidget(u8"˲������", f_Instantly, u8"��������.");
        ConfigWidget(u8"ֻ���ñ�ע���Ŀ��", f_OnlyTarget, u8"�������,ָ�������ڶ��㹥���Ĺ����Ӱ�춯��.");
        ConfigWidget(u8"�Ƴ���ײ��", f_SetCollider, u8"������ã����ｫ�޷����㣬���۾�����С");
        ConfigWidget(u8"�ٶ�", f_Speed, 0.1f, 1.0f, 15.0f, u8"���û�п����������ã���ô�����ƶ��ٶ�.");
        ConfigWidget(u8"�뾶 (m)", f_Radius, 0.1f, 5.0f, 150.0f, u8"���ð뾶.");
        ConfigWidget(u8"Χ�ƾ��� (m)", f_Distance, 0.1f, 0.5f, 10.0f, u8"Ŀ��Χ��������Χ.");
    }

    bool MobVacuum::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void MobVacuum::DrawStatus() 
    { 
        ImGui::Text(u8"�������� [%s]\n[%s|%.01fm|%.01fm|%s]",
            f_IncludeMonsters && f_IncludeAnimals ? u8"ȫ��" : f_IncludeMonsters ? u8"����" : f_IncludeAnimals ? u8"����" : "None",
            f_Instantly ? u8"����" : fmt::format("Normal|{:.1f}", f_Speed.value()).c_str(),
            f_Radius.value(),
            f_Distance.value(),
            f_OnlyTarget ? u8"���" : u8"ȫ��"
            f_SetCollider ? "RC" : "");
    }

    MobVacuum& MobVacuum::GetInstance()
    {
        static MobVacuum instance;
        return instance;
    }

    // Combines selected vacuum filters.
    void MobVacuum::UpdateFilters() {
        
        m_Filters.clear();

        if (f_IncludeMonsters) {
            if (f_MonsterCommon) m_Filters.push_back(&game::filters::combined::MonsterCommon);
            if (f_MonsterElites) m_Filters.push_back(&game::filters::combined::MonsterElites);
            if (f_MonsterBosses) m_Filters.push_back(&game::filters::combined::MonsterBosses);
        }

        if (f_IncludeAnimals) {
            if (f_AnimalDrop) m_Filters.push_back(&game::filters::combined::AnimalDrop);
            if (f_AnimalPickUp) m_Filters.push_back(&game::filters::combined::AnimalPickUp);
            if (f_AnimalNPC) m_Filters.push_back(&game::filters::combined::AnimalNPC);
        }
    }

    // Check if entity valid for mob vacuum.
    bool MobVacuum::IsEntityForVac(game::Entity* entity)
    {
        if (m_Filters.empty())
            return false;

        bool entityValid = std::any_of(m_Filters.cbegin(), m_Filters.cend(), 
            [entity](const game::IEntityFilter* filter) { return filter->IsValid(entity); });
        if (!entityValid)
            return false;

        auto& manager = game::EntityManager::instance();
        if (f_OnlyTarget && game::filters::combined::Monsters.IsValid(entity))
        {
            auto monsterCombat = entity->combat();
            if (monsterCombat == nullptr || monsterCombat->fields._attackTarget.runtimeID != manager.avatar()->runtimeID())
                return false;
        }

		auto distance = manager.avatar()->distance(entity);
        return distance <= f_Radius;
    }

    // Calculate mob vacuum target position.
    app::Vector3 MobVacuum::CalcMobVacTargetPos()
    {
        auto& manager = game::EntityManager::instance();
        auto avatarEntity = manager.avatar();
        if (avatarEntity == nullptr)
            return {};

        return avatarEntity->relativePosition() + avatarEntity->forward() * f_Distance;
    }

    // Set Monster's collider
    // Taiga#5555: There might be an in-game function for this already I'm just not sure which one
    void SetMonsterCollider(bool v)
    {
        auto monsterRoot = app::GameObject_Find(string_to_il2cppi("/EntityRoot/MonsterRoot"), nullptr);
        if (monsterRoot != nullptr)
        {
            auto transform = app::GameObject_GetComponentByName(monsterRoot, string_to_il2cppi("Transform"), nullptr);
            auto monsterCount = app::Transform_get_childCount(reinterpret_cast<app::Transform*>(transform), nullptr);
            for (int i = 0; i <= monsterCount - 1; i++)
            {
                auto monsters = app::Transform_GetChild(reinterpret_cast<app::Transform*>(transform), i, nullptr);
                auto monsterGameObject = app::Component_1_get_gameObject(reinterpret_cast<app::Component_1*>(monsters), nullptr);
                auto monsterTransform = app::GameObject_GetComponentByName(monsterGameObject, string_to_il2cppi("Transform"), nullptr);
                auto transformChild = app::Transform_GetChild(reinterpret_cast<app::Transform*>(monsterTransform), 1, nullptr);
                auto colliderGameObject = app::Component_1_get_gameObject(reinterpret_cast<app::Component_1*>(transformChild), nullptr);
                app::GameObject_SetActive(colliderGameObject, v, nullptr);
            }
        }
    }

    // Mob vacuum update function.
    // Changes position of monster, if mob vacuum enabled.
    void MobVacuum::OnGameUpdate()
    {
        static auto positions = new std::map<uint32_t, app::Vector3>();

        if (!f_Enabled)
            return;

        app::Vector3 targetPos = CalcMobVacTargetPos();
        if (IsVectorZero(targetPos))
            return;

        UpdateFilters();
        if (!f_IncludeMonsters && !f_IncludeAnimals)
            return;

        if (m_Filters.empty())
            return;

        auto& manager = game::EntityManager::instance();
        auto newPositions = new std::map<uint32_t, app::Vector3>();
        for (const auto& entity : manager.entities())
        {
            if (!IsEntityForVac(entity))
                continue;

            SetMonsterCollider(!f_SetCollider);

            if (f_Instantly)
            {
                entity->setRelativePosition(targetPos);
                continue;
            }

            uint32_t entityId = entity->runtimeID();
            app::Vector3 entityRelPos = positions->count(entityId) ? (*positions)[entityId] : entity->relativePosition();
            app::Vector3 newPosition = {};
            if (app::Vector3_Distance(entityRelPos, targetPos, nullptr) < 0.1)
            {
                newPosition = targetPos;
            }
            else
            {
                app::Vector3 dir = GetVectorDirection(entityRelPos, targetPos);
                float deltaTime = app::Time_get_deltaTime(nullptr);
                newPosition = entityRelPos + dir * f_Speed * deltaTime;
            }

            (*newPositions)[entityId] = newPosition;
            entity->setRelativePosition(newPosition);
        }

        delete positions;
        positions = newPositions;
    }

    // Mob vacuum sync packet replace.
    // Replacing move sync speed and motion state.
    //   Callow: I think it is more safe method, 
    //           because for server monster don't change position instantly.
    void MobVacuum::OnMoveSync(uint32_t entityId, app::MotionInfo* syncInfo)
    {
        if (!f_Enabled || f_Instantly)
            return;

        auto& manager = game::EntityManager::instance();
        auto entity = manager.entity(entityId);
        if (!IsEntityForVac(entity))
            return;

        SetMonsterCollider(!f_SetCollider);

        app::Vector3 targetPos = CalcMobVacTargetPos();
        app::Vector3 entityPos = entity->relativePosition();
        if (app::Vector3_Distance(targetPos, entityPos, nullptr) < 0.2)
            return;

        app::Vector3 dir = GetVectorDirection(targetPos, entityPos);
        app::Vector3 scaledDir = dir * f_Speed;

        syncInfo->fields.speed_->fields.x = scaledDir.x;
        syncInfo->fields.speed_->fields.y = scaledDir.y;
        syncInfo->fields.speed_->fields.z = scaledDir.z;

        switch (syncInfo->fields.motionState)
        {
        case app::MotionState__Enum::MotionStandby:
        case app::MotionState__Enum::MotionStandbyMove:
        case app::MotionState__Enum::MotionWalk:
        case app::MotionState__Enum::MotionDangerDash:
            syncInfo->fields.motionState = app::MotionState__Enum::MotionRun;
        }
    }
}

