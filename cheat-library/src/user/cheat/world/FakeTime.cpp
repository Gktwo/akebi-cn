#include "pch-il2cpp.h"
#include "FakeTime.h"
#include <cheat/events.h>


namespace cheat::feature
{
	//CNLouisLiu
	void* LevelTimeManager = NULL;
	FakeTime::FakeTime() : Feature(),
		NF(f_Enabled, u8"���ʱ��", u8"��/��", false),
		NF(f_TimeHour, u8"���ʱ��", u8"ʱ", 12),
		NF(f_TimeMinute, u8"���ʱ��", u8"��", 0)
	{
		HookManager::install(app::LevelTimeManager_SetInternalTimeOfDay, LevelTimeManager_SetInternalTimeOfDay_Hook);

		events::GameUpdateEvent += MY_METHOD_HANDLER(FakeTime::OnGameUpdate);
	}
	FakeTime& FakeTime::GetInstance()
	{
		static FakeTime instance;
		return instance;
	}
	const FeatureGUIInfo& FakeTime::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ u8"���ʱ��", "World", true };
		return info;
	}
	void FakeTime::DrawMain()
	{
		ConfigWidget(u8"��/��", f_Enabled, u8"������Ϸʱ��");
		ConfigWidget(u8"ʱ", f_TimeHour, 1, 0, 24);
		ConfigWidget(u8"��", f_TimeMinute, 1, 0, 60);
	}
	bool FakeTime::NeedStatusDraw() const
	{
		return f_Enabled;
	}
	void FakeTime::DrawStatus()
	{
		ImGui::Text(u8"���ʱ��|%d:%d", f_TimeHour.value(), f_TimeMinute.value());
	}
	float FakeTime::ConversionTime() {

		float time = float(f_TimeHour);
		float timemin = f_TimeMinute / 60;
		return time + timemin;
	}
	void FakeTime::OnGameUpdate()
	{
		if (LevelTimeManager != NULL && f_Enabled) {
			auto& faketime = GetInstance();
			CALL_ORIGIN(LevelTimeManager_SetInternalTimeOfDay_Hook, LevelTimeManager, faketime.ConversionTime(), false, false, (MethodInfo*)0);
		}
	}
	void FakeTime::LevelTimeManager_SetInternalTimeOfDay_Hook(void* __this, float inHours, bool force, bool refreshEnviroTime, MethodInfo* method) {
		float Hours = inHours;
		auto& faketime = GetInstance();
		if (faketime.f_Enabled)
		{
			Hours = faketime.ConversionTime();
		}
		CALL_ORIGIN(LevelTimeManager_SetInternalTimeOfDay_Hook, __this, Hours, force, refreshEnviroTime, method);

	}

}