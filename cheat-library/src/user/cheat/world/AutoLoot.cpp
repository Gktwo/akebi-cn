#include "pch-il2cpp.h"
#include "AutoLoot.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/filters.h>
#include <cheat/game/Chest.h>

namespace cheat::feature 
{
	static void LCSelectPickup_AddInteeBtnByID_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method);
	static bool LCSelectPickup_IsInPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method);
	static bool LCSelectPickup_IsOutPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method);

	float g_default_range = 3.0f;

    AutoLoot::AutoLoot() : Feature(),
        NF(f_AutoPickup,     "Auto-pickup drops",               "AutoLoot", false),
		NF(f_AutoTreasure,   "Auto-open treasures",             "AutoLoot", false),
		NF(f_UseCustomRange, "Use custom pickup range",         "AutoLoot", false),
		NF(f_PickupFilter,	 "Pickup filter",					"AutoLoot", false),
		NF(f_PickupFilter_Animals,	 "Animals filter",			"AutoLoot", true),
		NF(f_PickupFilter_DropItems, "Drop items filter",		"AutoLoot", true),
		NF(f_PickupFilter_Resources, "Resources filter",		"AutoLoot", true),
		NF(f_Chest,			 "Chests",							"AutoLoot", false),
		NF(f_Leyline,		 "Leylines",						"AutoLoot", false),
		NF(f_Investigate,	 "Search points",					"AutoLoot", false),
		NF(f_QuestInteract,  "Quest interacts",					"AutoLoot", false),
        NF(f_Others,		 "Other treasures",					"AutoLoot", false),
		NF(f_DelayTime,		 "Delay time (in ms)",				"AutoLoot", 200),
		NF(f_UseDelayTimeFluctuation, "Use delay fluctuation", "AutoLoot", false),
		NF(f_DelayTimeFluctuation,		 "Delay fluctuation +(in ms)",				"AutoLoot", 200),
        NF(f_CustomRange,    "Pickup Range",                    "AutoLoot", 5.0f),
		toBeLootedItems(), nextLootTime(0)
    {
		// Auto loot
		HookManager::install(app::MoleMole_LCSelectPickup_AddInteeBtnByID, LCSelectPickup_AddInteeBtnByID_Hook);
		HookManager::install(app::MoleMole_LCSelectPickup_IsInPosition, LCSelectPickup_IsInPosition_Hook);
		HookManager::install(app::MoleMole_LCSelectPickup_IsOutPosition, LCSelectPickup_IsOutPosition_Hook);

		events::GameUpdateEvent += MY_METHOD_HANDLER(AutoLoot::OnGameUpdate);
	}

    const FeatureGUIInfo& AutoLoot::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"自动战利品", "World", true };
        return info;
    }

    void AutoLoot::DrawMain()
    {
		if (ImGui::BeginTable("AutoLootDrawTable", 2, ImGuiTableFlags_NoBordersInBody))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::BeginGroupPanel(u8"自动拾取");
			{
				ConfigWidget(u8"开关", f_AutoPickup, u8"自动拾取周围物品.\n" \
					u8"Note: 过大的速度和范围会导致风险.\n" \
					u8"过于逆天会导致封号.\n\n" \
					u8"如果与自定义范围一起使用，请确保先启用该选项.");
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 165, 0, 255), u8"查看注意事项!");
			}
			ImGui::EndGroupPanel();
			
			ImGui::BeginGroupPanel(u8"拾取范围");
			{
				ConfigWidget(u8"开/关", f_UseCustomRange, "Enable custom pickup range.\n" \
					"High values are not recommended, as it is easily detected by the server.\n\n" \
					u8"如果与自动拾取/自动宝箱一起使用，请最后打开此选项.");
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 165, 0, 255), u8"查看注意事项");
				ImGui::SetNextItemWidth(100.0f);
				ConfigWidget(u8"范围 (m)", f_CustomRange, 0.1f, 0.5f, 40.0f, "Modifies pickup/open range to this value (in meters).");
			}
			ImGui::EndGroupPanel();
			
			ImGui::BeginGroupPanel(u8"拾取速度");
			{
				ImGui::SetNextItemWidth(100.0f);
				ConfigWidget(u8"延迟时间 (ms)", f_DelayTime, 1, 0, 1000, u8"抢夺/打开操作之间的延迟（毫秒）.\n" \
					u8"小于200ms的值是不安全的.\n 如果未启用自动功能，则不使用.");
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(u8"模拟人工点击");
			{
				ConfigWidget(u8"开/关", f_UseDelayTimeFluctuation, u8"启用延迟波动.\n" \
					u8"模拟人工点击延迟，因为手动点击从不一致.");
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 165, 0, 255), "Read the note!");
				ImGui::SetNextItemWidth(100.0f);
				ConfigWidget(u8"延迟范围 +(ms)", f_DelayTimeFluctuation, 1, 0, 1000, u8"延迟在“延迟时间+延迟时间+范围”之间随机波动");
			}
			ImGui::EndGroupPanel();
			
			ImGui::TableSetColumnIndex(1);
			ImGui::BeginGroupPanel(u8"自动宝箱");
			{
				ConfigWidget(u8"开关", f_AutoTreasure, "Automatically opens chests and other treasures.\n" \
					"Note: Using this with custom range and low delay times is extremely risky.\n" \
					"Abuse will definitely merit a ban.\n\n" \
					u8"如果与自定义范围一起使用，请确保先启用该选项.");
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 165, 0, 255), "Read the note!");
				ImGui::Indent();
				ConfigWidget(u8"宝箱", f_Chest, "Common, precious, luxurious, etc.");
				ConfigWidget(u8"地脉", f_Leyline, "Mora/XP, overworld/Trounce bosses, etc.");
				ConfigWidget(u8"调查点", f_Investigate, "Marked as Investigate/Search, etc.");
				ConfigWidget(u8"任务交互", f_QuestInteract, "Valid quest interact points.");
				ConfigWidget(u8"其他", f_Others, "Book Pages, Spincrystals, etc.");
				ImGui::Unindent();
			}
			ImGui::EndGroupPanel();
			ImGui::EndTable();
		}
			
    	ImGui::BeginGroupPanel(u8"拾取类型");
	    {
			ConfigWidget(u8"开/关", f_PickupFilter, "Enable pickup filter.\n");
			ConfigWidget(u8"动物", f_PickupFilter_Animals, "Fish, Lizard, Frog, Flying animals."); ImGui::SameLine();
			ConfigWidget(u8"掉落物", f_PickupFilter_DropItems, "Material, Mineral, Artifact."); ImGui::SameLine();
			ConfigWidget(u8"资源", f_PickupFilter_Resources, "Everything beside Animals and Drop Items (Plants, Books, etc).");
	    }
    	ImGui::EndGroupPanel();
    }

    bool AutoLoot::NeedStatusDraw() const
	{
        return f_AutoPickup || f_AutoTreasure || f_UseCustomRange || f_PickupFilter;
    }

    void AutoLoot::DrawStatus() 
    {
		ImGui::Text(u8"自动战利品\n[%s%s%s%s%s%s]",
			f_AutoPickup ? "AP" : "",
			f_AutoTreasure ? fmt::format("{}AT", f_AutoPickup ? "|" : "").c_str() : "",
			f_UseCustomRange ? fmt::format("{}CR{:.1f}m", f_AutoPickup || f_AutoTreasure ? "|" : "", f_CustomRange.value()).c_str() : "",
			f_PickupFilter ? fmt::format("{}PF", f_AutoPickup || f_AutoTreasure || f_UseCustomRange ? "|" : "").c_str() : "",
			f_AutoPickup || f_AutoTreasure ? fmt::format("|{}ms", f_DelayTime.value()).c_str() : "", 
			f_UseDelayTimeFluctuation ? fmt::format("|FL+{}ms", f_DelayTimeFluctuation.value()).c_str() : ""
		);
    }

    AutoLoot& AutoLoot::GetInstance()
    {
        static AutoLoot instance;
        return instance;
    }

	bool AutoLoot::OnCreateButton(app::BaseEntity* entity)
	{
		if (!f_AutoPickup)
			return false;

		auto itemModule = GET_SINGLETON(MoleMole_ItemModule);
		if (itemModule == nullptr)
			return false;
    	
		auto entityId = entity->fields._runtimeID_k__BackingField;
		if (f_DelayTime == 0)
		{
			app::MoleMole_ItemModule_PickItem(itemModule, entityId, nullptr);
			return true;
		}

		toBeLootedItems.push(entityId);
		return false;
	}

	void AutoLoot::OnGameUpdate()
	{
		auto currentTime = util::GetCurrentTimeMillisec();
		if (currentTime < nextLootTime)
			return;

		auto entityManager = GET_SINGLETON(MoleMole_EntityManager);
		if (entityManager == nullptr)
			return;

		// RyujinZX#6666
		if (f_AutoTreasure) 
		{
			auto& manager = game::EntityManager::instance();
			for (auto& entity : manager.entities(game::filters::combined::Chests)) 
			{
				float range = f_UseCustomRange ? f_CustomRange : g_default_range;
				if (manager.avatar()->distance(entity) >= range)
					continue;

				auto chest = reinterpret_cast<game::Chest*>(entity);
				auto chestType = chest->itemType();

				if (!f_Investigate && chestType == game::Chest::ItemType::Investigate)
					continue;

				if (!f_QuestInteract && chestType == game::Chest::ItemType::QuestInteract)
					continue;

				if (!f_Others && (
					chestType == game::Chest::ItemType::BGM ||
					chestType == game::Chest::ItemType::BookPage
					))
					continue;

				if (!f_Leyline && chestType == game::Chest::ItemType::Flora)
					continue;

				if (chestType == game::Chest::ItemType::Chest)
				{
					if (!f_Chest)
						continue;
					auto ChestState = chest->chestState();
					if (ChestState != game::Chest::ChestState::None)
						continue;
				}

				uint32_t entityId = entity->runtimeID();
				toBeLootedItems.push(entityId);
			}
		}

		auto entityId = toBeLootedItems.pop();
		if (!entityId)
			return;

		auto itemModule = GET_SINGLETON(MoleMole_ItemModule);
		if (itemModule == nullptr)
			return;

		auto entity = app::MoleMole_EntityManager_GetValidEntity(entityManager, *entityId, nullptr);
		if (entity == nullptr)
			return;

		app::MoleMole_ItemModule_PickItem(itemModule, *entityId, nullptr);

		int fluctuation = 0;
		if (f_UseDelayTimeFluctuation)
		{
			fluctuation = std::rand() % (f_DelayTimeFluctuation + 1);
		}

		nextLootTime = currentTime + (int)f_DelayTime + fluctuation;
	}

	void AutoLoot::OnCheckIsInPosition(bool& result, app::BaseEntity* entity)
	{
		if (f_AutoPickup || f_UseCustomRange) {
			float pickupRange = f_UseCustomRange ? f_CustomRange : g_default_range;
			if (f_PickupFilter)
			{
				if (!f_PickupFilter_Animals && entity->fields.entityType == app::EntityType__Enum_1::EnvAnimal ||
					!f_PickupFilter_DropItems && entity->fields.entityType == app::EntityType__Enum_1::DropItem ||
					!f_PickupFilter_Resources && entity->fields.entityType == app::EntityType__Enum_1::GatherObject)
				{
					result = false;
					return;
				}
			}
			
			auto& manager = game::EntityManager::instance();
			result = manager.avatar()->distance(entity) < pickupRange;
		}
	}

	static void LCSelectPickup_AddInteeBtnByID_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method)
	{
		AutoLoot& autoLoot = AutoLoot::GetInstance();
		bool canceled = autoLoot.OnCreateButton(entity);
		if (!canceled)
			CALL_ORIGIN(LCSelectPickup_AddInteeBtnByID_Hook, __this, entity, method);
	}

	static bool LCSelectPickup_IsInPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method)
	{
		bool result = CALL_ORIGIN(LCSelectPickup_IsInPosition_Hook, __this, entity, method);

		AutoLoot& autoLoot = AutoLoot::GetInstance();
		autoLoot.OnCheckIsInPosition(result, entity);

		return result;
	}

	static bool LCSelectPickup_IsOutPosition_Hook(void* __this, app::BaseEntity* entity, MethodInfo* method)
	{
		bool result = CALL_ORIGIN(LCSelectPickup_IsOutPosition_Hook, __this, entity, method);

		AutoLoot& autoLoot = AutoLoot::GetInstance();
		autoLoot.OnCheckIsInPosition(result, entity);

		return result;
	}
}

