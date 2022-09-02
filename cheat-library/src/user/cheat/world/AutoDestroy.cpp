#include "pch-il2cpp.h"
#include "AutoDestroy.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>
#include <cheat/game/SimpleFilter.h>
#include <cheat/game/EntityManager.h>
#include <cheat/world/AutoChallenge.h>
#include <cheat/game/filters.h>

namespace cheat::feature
{
	static void LCAbilityElement_ReduceModifierDurability_Hook(app::LCAbilityElement* __this, int32_t modifierDurabilityIndex, float reduceDurability, app::Nullable_1_Single_ deltaTime, MethodInfo* method);

	AutoDestroy::AutoDestroy() : Feature(),
		NF(f_Enabled, "Auto Destroy", "AutoDestroy", false),
		NF(f_DestroyOres, "Destroy Ores", "AutoDestroy", false),
		NF(f_DestroyShields, "Destroy Shields", "AutoDestroy", false),
		NF(f_DestroyDoodads, "Destroy Doodads", "AutoDestroy", false),
		NF(f_DestroyPlants, "Destroy Plants", "AutoDestroy", false),
		NF(f_DestroySpecialObjects, "Destroy Special Objects", "AutoDestroy", false),
		NF(f_DestroySpecialChests, "Destroy Special Chests", "AutoDestroy", false),
		NF(f_Range, "Range", "AutoDestroy", 10.0f)
	{
		HookManager::install(app::MoleMole_LCAbilityElement_ReduceModifierDurability, LCAbilityElement_ReduceModifierDurability_Hook);
	}

	const FeatureGUIInfo& AutoDestroy::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ u8"原子崩坏", "World", true };
		return info;
	}

	void AutoDestroy::DrawMain()
	{
		ImGui::TextColored(ImColor(255, 165, 0, 255), u8"此功能安全性有很高的风险.\n"
			u8"不要在你的主要账号上使用");

		ConfigWidget(u8"开/关", f_Enabled, u8"破坏范围内的物品.");
		ImGui::Indent();
		ConfigWidget(u8"矿石类", f_DestroyOres, u8"可采集矿石类物品.");
		ConfigWidget(u8"怪物的护盾", f_DestroyShields, u8"怪物的护盾.");
		ConfigWidget(u8"部分探索物品", f_DestroyDoodads, u8"罐头瓶子之类.");
		ConfigWidget("Plants", f_DestroyPlants, "Dandelion Seeds, Sakura Bloom, etc.");
		ConfigWidget("Special Objects", f_DestroySpecialObjects, "Destroy Ancient Rime, Large and Small Rock Piles");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(255, 165, 0, 255), u8"高风险!");
		ConfigWidget("Special Chests", f_DestroySpecialChests, "Destroy Chests with Brambles, Frozen, or In Rocks");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(255, 165, 0, 255), "Risk Unknown!");
		ImGui::Unindent();
		ConfigWidget(u8"范围 (m)", f_Range, 0.1f, 1.0f, 15.0f);
	}

	bool AutoDestroy::NeedStatusDraw() const
	{
		return f_Enabled;
	}

	void AutoDestroy::DrawStatus()
	{
		ImGui::Text(u8"自动销毁 [%.01fm%s%s%s%s%s%s%s]",
			f_Range.value(),
			f_DestroyOres || f_DestroyShields || f_DestroyDoodads || f_DestroyPlants || f_DestroySpecialObjects || f_DestroySpecialChests ? "|" : "",
			f_DestroyOres ? u8"矿" : "",
			f_DestroyShields ? u8"盾" : "",
			f_DestroyDoodads ? u8"道具" : "",
			f_DestroyPlants ? u8"植" : "",
			f_DestroySpecialObjects ? "SO" : "",
			f_DestroySpecialChests ? "SC" : "");
	}

	AutoDestroy& AutoDestroy::GetInstance()
	{
		static AutoDestroy instance;
		return instance;
	}

	// Thanks to @RyujinZX
	// Every ore has ability element component
	// Durability of ability element is a ore health
	// Every tick ability element check reducing durability, for ore in calm state `reduceDurability` equals 0, means HP don't change
	// We need just change this value to current durability or above to destroy ore
	// This function also can work with some types of shields (TODO: improve killaura with this function)
	static void LCAbilityElement_ReduceModifierDurability_Hook(app::LCAbilityElement* __this, int32_t modifierDurabilityIndex, float reduceDurability, app::Nullable_1_Single_ deltaTime, MethodInfo* method)
	{
		auto& manager = game::EntityManager::instance();
		auto& autoDestroy = AutoDestroy::GetInstance();
		auto& autoChallenge = AutoChallenge::GetInstance();
		auto entity = __this->fields._._._entity;
		// call origin ReduceModifierDurability without correct modifierDurabilityIndex will coz game crash.
		// so use this hook function to destroy challenge's bombbarrel
		if (autoChallenge.f_Enabled && autoChallenge.f_BombDestroy &&
			autoChallenge.f_Range > manager.avatar()->distance(entity) &&
			game::filters::puzzle::Bombbarrel.IsValid(manager.entity(entity))
			)
		{
			reduceDurability = 1000.f;
		}
		if (autoDestroy.f_Enabled &&
			autoDestroy.f_Range > manager.avatar()->distance(entity) &&
			(
				(autoDestroy.f_DestroyOres && game::filters::combined::Ores.IsValid(manager.entity(entity))) ||
				(autoDestroy.f_DestroyDoodads && (game::filters::combined::Doodads.IsValid(manager.entity(entity)) || game::filters::chest::SBramble.IsValid(manager.entity(entity)))) ||
				(autoDestroy.f_DestroyShields && !game::filters::combined::MonsterBosses.IsValid(manager.entity(entity)) && (
					game::filters::combined::MonsterShielded.IsValid(manager.entity(entity)) ||											// For shields attached to monsters, e.g. abyss mage shields.
					game::filters::combined::MonsterEquips.IsValid(manager.entity(entity)))) ||											// For shields/weapons equipped by monsters, e.g. rock shield.
					(autoDestroy.f_DestroyPlants && game::filters::combined::PlantDestroy.IsValid(manager.entity(entity))) ||			// For plants e.g dandelion seeds.
				(autoDestroy.f_DestroySpecialObjects && game::filters::combined::BreakableObjects.IsValid(manager.entity(entity))) ||	// For Breakable Objects e.g Ancient Rime, Large and Small Rock Piles.
				(autoDestroy.f_DestroySpecialChests && game::filters::combined::Chests.IsValid(manager.entity(entity)))					// For Special Chests e.g Brambles, Frozen, Encased in Rock.
				)
			)
		{
			// This value always above any ore durability
			reduceDurability = 1000;
		}
		CALL_ORIGIN(LCAbilityElement_ReduceModifierDurability_Hook, __this, modifierDurabilityIndex, reduceDurability, deltaTime, method);
	}

}

