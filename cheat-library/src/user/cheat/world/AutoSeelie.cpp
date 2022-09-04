#include "pch-il2cpp.h"
#include "AutoSeelie.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>

namespace cheat::feature
{
	AutoSeelie::AutoSeelie() : Feature(),
		NF(f_Enabled, u8"�Զ�����", u8"�Զ�����", false),
		NF(f_ElectroSeelie, u8"�Զ�����", u8"�Զ�����", false),
		nextTime(0)
	{
		events::GameUpdateEvent += MY_METHOD_HANDLER(AutoSeelie::OnGameUpdate);
	}
	const FeatureGUIInfo& AutoSeelie::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "", u8"������", true };
		return info;
	}

	void AutoSeelie::DrawMain()
	{
		ConfigWidget(u8"�Զ�����", f_Enabled, u8"�����Զ��ؼ�");

		if (f_Enabled)
		{
			ImGui::Indent();

			ConfigWidget(u8"�Զ�����", f_ElectroSeelie, u8"����������Ҫ�ֶ�����electroseelie��\n���ǿ�ʼ�ڰ뾶100�׷�Χ��ʹ�ô�ѡ���Զ��ƶ�."                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            );
			ImGui::SameLine();
			ImGui::TextColored(ImColor(255, 165, 0, 255), u8"���Ķ�ע������!");
			ImGui::Unindent();
		}

	}

	bool AutoSeelie::NeedStatusDraw() const
	{
		return f_Enabled;
	}

	void AutoSeelie::DrawStatus()
	{
		ImGui::Text(u8"�Զ����� %s", f_ElectroSeelie ? u8"+ ��" : "");
	}

	AutoSeelie& AutoSeelie::GetInstance()
	{
		static AutoSeelie instance;
		return instance;
	}

	bool AutoSeelie::IsEntityForVac(game::Entity* entity)
	{
		auto& manager = game::EntityManager::instance();
		auto distance = manager.avatar()->distance(entity);
		float radius = 100.0f;

		if (entity->name().find("Seelie") != std::string::npos)
		{
			if (entity->name().find("ElectricSeelie") != std::string::npos)
			{

				if (f_ElectroSeelie)
				{
					auto EntityGameObject = app::MoleMole_BaseEntity_get_rootGameObject(entity->raw(), nullptr);
					auto Transform = app::GameObject_GetComponentByName(EntityGameObject, string_to_il2cppi("Transform"), nullptr);
					auto child = app::Transform_GetChild(reinterpret_cast<app::Transform*>(Transform), 1, nullptr);
					auto pre_status = app::Component_1_get_gameObject(reinterpret_cast<app::Component_1*>(child), nullptr);
					auto status = app::GameObject_get_active(reinterpret_cast<app::GameObject*>(pre_status), nullptr);

					if (status)
					{
						return false;
					}
					return distance <= radius;
				}
				return false;
			}
			return distance <= radius;
		}
		return false;
	}

	void AutoSeelie::OnGameUpdate()
	{
		if (!f_Enabled)
			return;

		auto currentTime = util::GetCurrentTimeMillisec();
		if (currentTime < nextTime)
			return;

		auto& manager = game::EntityManager::instance();
		auto avatarEntity = manager.avatar();
		for (const auto& entity : manager.entities())
		{
			if (!IsEntityForVac(entity))
				continue;

			entity->setRelativePosition(avatarEntity->relativePosition());
		}
		nextTime = currentTime + 1000;
	}

}