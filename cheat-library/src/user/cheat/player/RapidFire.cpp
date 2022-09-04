#include "pch-il2cpp.h"
#include "RapidFire.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat/game/filters.h>

namespace cheat::feature
{
	//static void LCBaseCombat_DoHitEntity_Hook(app::LCBaseCombat* __this, uint32_t targetID, app::AttackResult* attackResult, bool ignoreCheckCanBeHitInMP, MethodInfo* method);
	static void VCAnimatorEvent_HandleProcessItem_Hook(app::MoleMole_VCAnimatorEvent* __this,
		app::MoleMole_VCAnimatorEvent_MoleMole_VCAnimatorEvent_AnimatorEventPatternProcessItem* processItem,
		app::AnimatorStateInfo processStateInfo, app::MoleMole_VCAnimatorEvent_MoleMole_VCAnimatorEvent_TriggerMode__Enum mode, MethodInfo* method);
	static void LCBaseCombat_FireBeingHitEvent_Hook(app::LCBaseCombat* __this, uint32_t attackeeRuntimeID, app::AttackResult* attackResult, MethodInfo* method);

	RapidFire::RapidFire() : Feature(),
		NF(f_Enabled, u8"��������", u8"��������", false),
		NF(f_MultiHit, u8"�ظ�����", u8"��������", false),
		NF(f_Multiplier, u8"���г���", u8"��������", 2),
		NF(f_OnePunch, u8"һȭģʽ", u8"��������", false),
		NF(f_Randomize, u8"���", u8"��������", false),
		NF(f_minMultiplier, u8"��С����", u8"��������", 1),
		NF(f_maxMultiplier, u8"������", u8"��������", 3),
		NF(f_MultiTarget, u8"��Ŀ��", u8"��������", false),
		NF(f_MultiTargetRadius, u8"��Ŀ��뾶", u8"��������", 20.0f),
		NF(f_MultiAnimation, u8"���ض���", u8"��������", false)
	{
		// HookManager::install(app::MoleMole_LCBaseCombat_DoHitEntity, LCBaseCombat_DoHitEntity_Hook); -- Looks like FireBeingHitEvent is superior to this.
		HookManager::install(app::MoleMole_VCAnimatorEvent_HandleProcessItem, VCAnimatorEvent_HandleProcessItem_Hook);
		HookManager::install(app::MoleMole_LCBaseCombat_FireBeingHitEvent, LCBaseCombat_FireBeingHitEvent_Hook);
	}

	const FeatureGUIInfo& RapidFire::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ u8"��������", u8"�����", true };
		return info;
	}

	void RapidFire::DrawMain()
	{
		ConfigWidget(u8"��/��", f_Enabled, u8"���ù�����������Ҫѡ����ģʽ.");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(255, 165, 0, 255), u8"��ѡ������һ��ģʽ.");

		ConfigWidget(u8"�౶�˺�ģʽ", f_MultiHit, u8"���ö�����.\n" \
			u8"������Ĺ�������.\n" \
			u8"��û�о����ܺõĲ��ԣ��з���.\n" \
			u8"�����������ʻ�һ��ʹ�û����ֵһ��ʹ��.\n");

		ImGui::Indent();

		ConfigWidget(u8"һȭģʽ", f_OnePunch, u8"���ݵ��˵�HP����ɱ��������Ҫ���ٴι���\n" \
			u8"��ʹ������Ӧ�����ó���.\n" \
			u8"���ܸ���ȫ��������������ܲ���ȷ.");

		ConfigWidget(u8"����ģʽ", f_Randomize, u8"����С��������֮�����������.");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(255, 165, 0, 255), u8"�����һȭģʽ�ǳ�ͻ��!");

		if (!f_OnePunch) {
			if (!f_Randomize)
			{
				ConfigWidget(u8"��������", f_Multiplier, 1, 2, 1000, u8"������������.");
			}
			else
			{
				ConfigWidget(u8"��С����", f_minMultiplier, 1, 1, 1000, u8"����������С����.");
				ConfigWidget(u8"�����", f_maxMultiplier, 1, 2, 1000, u8"��������������.");
			}
		}

		ImGui::Unindent();

		ConfigWidget(u8"��Χ�˺�", f_MultiTarget, u8"��ָ����Ŀ��뾶�����ö�Ŀ�깥��.\n" \
			u8"��ʼĿ����Χ��������ЧĿ�꽫������������.\n" \
			u8"�˺�����ֻ������ڳ�ʼĿ���ϣ���������ЧĿ�궼�ᱻ��.\n" \
			u8"��������˶������У����ҵ���Ŀ�������ж�����֣����顰���ԡ������е�ʵ����������Բ鿴�Ƿ���ڲ��ɼ���ʵ��.\n" \
			u8"�������һ��ʹ�ã�����ܻᵼ�¼����ͺ�Ϳ��ٽ�ֹ���㱻������."
		);

		ImGui::Indent();
		ConfigWidget(u8"�뾶 (m)", f_MultiTargetRadius, 0.1f, 5.0f, 50.0f, u8"�����ЧĿ��İ뾶.");
		ImGui::Unindent();

		ConfigWidget(u8"�౶����", f_MultiAnimation, u8"���öද������.\n" \
			u8"���ס����ɫ����ƵҲ�ᱻ�ظ�.");
	}

	bool RapidFire::NeedStatusDraw() const
	{
		return f_Enabled && (f_MultiHit || f_MultiTarget || f_MultiAnimation);
	}

	void RapidFire::DrawStatus()
	{
		if (f_MultiHit)
		{
			if (f_Randomize)
				ImGui::Text(u8"�౶���� ���%d|%d]", f_minMultiplier.value(), f_maxMultiplier.value());
			else if (f_OnePunch)
				ImGui::Text(u8"�౶����[һȭģʽ]");
			else
				ImGui::Text(u8"�౶���� [%d]", f_Multiplier.value());
		}
		if (f_MultiTarget)
			ImGui::Text(u8"�ҵ�[%.01fm]��", f_MultiTargetRadius.value());

		if (f_MultiAnimation)
			ImGui::Text(u8"�౶����");
	}

	RapidFire& RapidFire::GetInstance()
	{
		static RapidFire instance;
		return instance;
	}


	int RapidFire::CalcCountToKill(float attackDamage, uint32_t targetID)
	{
		if (attackDamage == 0)
			return f_Multiplier;

		auto& manager = game::EntityManager::instance();
		auto targetEntity = manager.entity(targetID);
		if (targetEntity == nullptr)
			return f_Multiplier;

		auto baseCombat = targetEntity->combat();
		if (baseCombat == nullptr)
			return f_Multiplier;

		auto safeHP = baseCombat->fields._combatProperty_k__BackingField->fields.HP;
		auto HP = app::MoleMole_SafeFloat_get_Value(safeHP, nullptr);
		int attackCount = (int)ceil(HP / attackDamage);
		return std::clamp(attackCount, 1, 200);
	}

	int RapidFire::GetAttackCount(app::LCBaseCombat* combat, uint32_t targetID, app::AttackResult* attackResult)
	{
		if (!f_MultiHit)
			return 1;

		auto& manager = game::EntityManager::instance();
		auto targetEntity = manager.entity(targetID);
		auto baseCombat = targetEntity->combat();
		if (baseCombat == nullptr)
			return 1;

		int countOfAttacks = f_Multiplier;
		if (f_OnePunch)
		{
			app::MoleMole_Formula_CalcAttackResult(combat->fields._combatProperty_k__BackingField,
				baseCombat->fields._combatProperty_k__BackingField,
				attackResult, manager.avatar()->raw(), targetEntity->raw(), nullptr);
			countOfAttacks = CalcCountToKill(attackResult->fields.damage, targetID);
		}
		if (f_Randomize)
		{
			if (f_minMultiplier.value() >= f_maxMultiplier.value()) 
				countOfAttacks = f_minMultiplier.value();
			else
				countOfAttacks = rand() % (f_maxMultiplier.value() - f_minMultiplier.value()) + f_minMultiplier.value();
			return countOfAttacks;
		}

		return countOfAttacks;
	}

	bool IsAvatarOwner(game::Entity entity)
	{
		auto& manager = game::EntityManager::instance();
		auto avatarID = manager.avatar()->runtimeID();

		while (entity.isGadget())
		{
			game::Entity temp = entity;
			entity = game::Entity(app::MoleMole_GadgetEntity_GetOwnerEntity(reinterpret_cast<app::GadgetEntity*>(entity.raw()), nullptr));
			if (entity.runtimeID() == avatarID)
				return true;
		}

		return false;

	}

	bool IsAttackByAvatar(game::Entity& attacker)
	{
		if (attacker.raw() == nullptr)
			return false;

		auto& manager = game::EntityManager::instance();
		auto avatarID = manager.avatar()->runtimeID();
		auto attackerID = attacker.runtimeID();

		return attackerID == avatarID || IsAvatarOwner(attacker);
	}

	bool IsConfigByAvatar(game::Entity& attacker)
	{
		if (attacker.raw() == nullptr)
			return false;

		auto& manager = game::EntityManager::instance();
		auto avatarID = manager.avatar()->raw()->fields._configID_k__BackingField;
		auto attackerID = attacker.raw()->fields._configID_k__BackingField;
		// LOG_DEBUG("configID = %d", attackerID);
		// Taiga#5555: IDs can be found in ConfigAbility_Avatar_*.json or GadgetExcelConfigData.json
		bool bulletID = attackerID >= 40000160 && attackerID <= 41079999;

		return avatarID == attackerID || bulletID || attacker.type() == app::EntityType__Enum_1::Bullet;
	}

	bool IsValidByFilter(game::Entity* entity)
	{
		if (game::filters::combined::OrganicTargets.IsValid(entity) ||
			game::filters::monster::SentryTurrets.IsValid(entity) ||
			game::filters::combined::Ores.IsValid(entity) ||
			game::filters::puzzle::Geogranum.IsValid(entity) ||
			game::filters::puzzle::LargeRockPile.IsValid(entity) ||
			game::filters::puzzle::SmallRockPile.IsValid(entity))
			return true;
		return false;
	}

	// Raises when any entity do hit event.
	// Just recall attack few times (regulating by combatProp)
	// It's not tested well, so, I think, anticheat can detect it.
	/*static void LCBaseCombat_DoHitEntity_Hook(app::LCBaseCombat* __this, uint32_t targetID, app::AttackResult* attackResult, bool ignoreCheckCanBeHitInMP, MethodInfo* method)
	{
		auto attacker = game::Entity(__this->fields._._._entity);
		RapidFire& rapidFire = RapidFire::GetInstance();
		if (!IsConfigByAvatar(attacker) || !IsAttackByAvatar(attacker) || !rapidFire.f_Enabled)
			return CALL_ORIGIN(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);

		auto& manager = game::EntityManager::instance();
		auto originalTarget = manager.entity(targetID);

		if (!IsValidByFilter(originalTarget))
			return CALL_ORIGIN(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);

		std::vector<cheat::game::Entity*> validEntities;
		validEntities.push_back(originalTarget);

		if (rapidFire.f_MultiTarget)
		{
			auto filteredEntities = manager.entities();
			for (const auto& entity : filteredEntities) {
				auto distance = originalTarget->distance(entity);

				if (entity->runtimeID() == manager.avatar()->runtimeID())
					continue;

				if (entity->runtimeID() == targetID)
					continue;

				if (distance > rapidFire.f_MultiTargetRadius)
					continue;

				if (!IsValidByFilter(entity))
					continue;

				validEntities.push_back(entity);
			}
		}

		for (const auto& entity : validEntities) {

			if (rapidFire.f_MultiHit) {
				int attackCount = rapidFire.GetAttackCount(__this, entity->runtimeID(), attackResult);
				for (int i = 0; i < attackCount; i++)
					app::MoleMole_LCBaseCombat_FireBeingHitEvent(__this, entity->runtimeID(), attackResult, method);
			}
			else CALL_ORIGIN(LCBaseCombat_DoHitEntity_Hook, __this, entity->runtimeID(), attackResult, ignoreCheckCanBeHitInMP, method);
		}

		CALL_ORIGIN(LCBaseCombat_DoHitEntity_Hook, __this, targetID, attackResult, ignoreCheckCanBeHitInMP, method);
	}*/

	static void LCBaseCombat_FireBeingHitEvent_Hook(app::LCBaseCombat* __this, uint32_t attackeeRuntimeID, app::AttackResult* attackResult, MethodInfo* method)
	{
		auto attacker = game::Entity(__this->fields._._._entity);
		RapidFire& rapidFire = RapidFire::GetInstance();
		if (!IsConfigByAvatar(attacker) || !IsAttackByAvatar(attacker) || !rapidFire.f_Enabled)
			return CALL_ORIGIN(LCBaseCombat_FireBeingHitEvent_Hook, __this, attackeeRuntimeID, attackResult, method);

		auto& manager = game::EntityManager::instance();
		auto originalTarget = manager.entity(attackeeRuntimeID);

		if (!IsValidByFilter(originalTarget))
			return CALL_ORIGIN(LCBaseCombat_FireBeingHitEvent_Hook, __this, attackeeRuntimeID, attackResult, method);

		std::vector<cheat::game::Entity*> validEntities;
		validEntities.push_back(originalTarget);

		if (rapidFire.f_MultiTarget)
		{
			auto filteredEntities = manager.entities();
			for (const auto& entity : filteredEntities) {
				auto distance = originalTarget->distance(entity);

				if (entity->runtimeID() == manager.avatar()->runtimeID())
					continue;

				if (entity->runtimeID() == attackeeRuntimeID)
					continue;

				if (distance > rapidFire.f_MultiTargetRadius)
					continue;

				if (!IsValidByFilter(entity))
					continue;

				validEntities.push_back(entity);
			}
		}

		for (const auto& entity : validEntities) {
			int attackCount = rapidFire.f_MultiHit ? rapidFire.GetAttackCount(__this, entity->runtimeID(), attackResult) : 1;
			for (int i = 0; i < attackCount; i++)
				CALL_ORIGIN(LCBaseCombat_FireBeingHitEvent_Hook, __this, entity->runtimeID(), attackResult, method);
		}
	}

	static void VCAnimatorEvent_HandleProcessItem_Hook(app::MoleMole_VCAnimatorEvent* __this,
		app::MoleMole_VCAnimatorEvent_MoleMole_VCAnimatorEvent_AnimatorEventPatternProcessItem* processItem,
		app::AnimatorStateInfo processStateInfo, app::MoleMole_VCAnimatorEvent_MoleMole_VCAnimatorEvent_TriggerMode__Enum mode, MethodInfo* method)
	{
		auto attacker = game::Entity(__this->fields._._._entity);
		RapidFire& rapidFire = RapidFire::GetInstance();

		if (rapidFire.f_MultiAnimation && IsAttackByAvatar(attacker))
			processItem->fields.lastTime = 0;

		CALL_ORIGIN(VCAnimatorEvent_HandleProcessItem_Hook, __this, processItem, processStateInfo, mode, method);
	}
}

