#include "pch-il2cpp.h"
#include "AutoRun.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat-base/render/renderer.h>

namespace cheat::feature
{

	AutoRun::AutoRun() : Feature(),
		NF(f_Enabled, u8"�Զ�����", "Player::AutoRun", false),
		NF(f_Speed,	u8"�ٶ�", "Player::AutoRun",1.0f)
	{
		events::GameUpdateEvent += MY_METHOD_HANDLER(AutoRun::OnGameUpdate);
	}

	const FeatureGUIInfo& AutoRun::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ u8"�Զ����ߣ����﹦�ܣ�", "Player", true };
		return info;
	}

	void AutoRun::DrawMain()
	{
		ConfigWidget(u8"��/��", f_Enabled);
		ConfigWidget(u8"�ٶ�", f_Speed, 0.01f, 0.01f, 1000.0f, u8"�ٶ�\n�����鳬��5");
	}

	bool AutoRun::NeedStatusDraw() const
	{
		return f_Enabled;
	}

	void AutoRun::DrawStatus()
	{
		ImGui::Text(u8"�Զ�����[%.01f]",f_Speed.value());
	}

	AutoRun& AutoRun::GetInstance()
	{
		static AutoRun instance;
		return instance;
	}

	void EnableAutoRun(float speed) {
		
		auto& manager = game::EntityManager::instance();
		auto avatarEntity = manager.avatar();

		auto baseMove = avatarEntity->moveComponent();
		auto rigidBody = avatarEntity->rigidbody();

		if (baseMove == nullptr || rigidBody == nullptr || renderer::IsInputLocked())
			return;

		auto cameraEntity = game::Entity(reinterpret_cast<app::BaseEntity*>(manager.mainCamera()));
		auto relativeEntity = &cameraEntity;

		app::Vector3 dir = relativeEntity->forward();
		app::Vector3 prevPos = avatarEntity->relativePosition();

		float deltaTime = app::Time_get_deltaTime(nullptr);
		app::Vector3 newPos = prevPos + dir * speed * deltaTime;

		avatarEntity->setRelativePosition(newPos);
	}

	void AutoRun::OnGameUpdate() {
		if (f_Enabled) {
			float speed = f_Speed.value();
			EnableAutoRun(speed);
		}
	}
}
