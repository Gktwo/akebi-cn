#include <pch.h>
#include "Settings.h"

#include <cheat-base/cheat/CheatManagerBase.h>
#include <cheat-base/render/renderer.h>
#include <cheat-base/render/gui-util.h>
#include <misc/cpp/imgui_stdlib.h>
#include <cheat-base/util.h>

#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

namespace cheat::feature
{
	Settings::Settings() : Feature(),
		NF(f_MenuKey, u8"显示作弊菜单按键", "General", Hotkey(VK_F1)),
		NF(f_HotkeysEnabled, u8"使用热键", "General", true),
		
		NF(f_StatusMove, u8"移动功能窗口", "General::StatusWindow", true),
		NF(f_StatusShow, u8"显示功能窗口", "General::StatusWindow", true),

		NF(f_InfoMove, u8"移动info窗口", "General::InfoWindow", true),
		NF(f_InfoShow, u8"显示info窗口", "General::InfoWindow", true),

		NF(f_FpsMove, u8"移动fps窗口", "General::FPS", false),
		NF(f_FpsShow, u8"显示fps窗口", "General::FPS", true),

		NF(f_NotificationsShow, u8"显示通知窗口", "General::Notify", true),
		NF(f_NotificationsDelay, u8"通知延迟", "General::Notify", 500),

		NF(f_FileLogging, u8"文件日志记录", "General::Logging", false),
		NF(f_ConsoleLogging, u8"控制台日志", "General::Logging", true),

		NF(f_FastExitEnable, u8"快速退出", "General::FastExit", false),
		NF(f_HotkeyExit, u8"热键", "General::FastExit", Hotkey(VK_F12)),
		
		NF(f_FontSize, u8"字体大小", "General", 16.0f),
		NF(f_ShowStyleEditor, u8"显示颜色自定义", "General", false),
		NFS(f_DefaultTheme, u8"主题", "General::Colors", "Default"),
		themesDir(util::GetCurrentPath() / "themes")

	{
		renderer::SetGlobalFontSize(static_cast<float>(f_FontSize));
		f_HotkeyExit.value().PressedEvent += MY_METHOD_HANDLER(Settings::OnExitKeyPressed);
		if (!std::filesystem::exists(themesDir))
			std::filesystem::create_directory(themesDir);

	}

	bool inited = false;
	void Settings::Init() {
		if (this->f_DefaultTheme.value() != "Default" && !inited)
		{
			LOG_INFO("Loading theme: %s", themesDir / (f_DefaultTheme.value() + ".json").c_str());
			if (!std::filesystem::exists(themesDir / (f_DefaultTheme.value() + ".json")))
				f_DefaultTheme = "Default";
			else Colors_Import(f_DefaultTheme.value());
			inited = true;
		}
	}

	const FeatureGUIInfo& Settings::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "", "Settings", false };
		return info;
	}

	void Settings::Colors_Export(std::string name)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		auto colors = style.Colors;

		nlohmann::json json;
		for (int i = 0; i < ImGuiCol_COUNT; i++)
			json[ImGui::GetStyleColorName((ImGuiCol)i)] = { colors[i].x, colors[i].y, colors[i].z, colors[i].w };
		std::ofstream file(themesDir / (name + ".json"));
		file << std::setw(4) << json << std::endl;
	}

	void Settings::Colors_Import(std::string name)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		auto colors = style.Colors;
		nlohmann::json json;
		std::ifstream file(themesDir / (name + ".json"));
		file >> json;
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			auto color = json[ImGui::GetStyleColorName((ImGuiCol)i)];
			colors[i].x = color[0];
			colors[i].y = color[1];
			colors[i].z = color[2];
			colors[i].w = color[3];
		}
	}

	void Settings::DrawMain()
	{

		ImGui::BeginGroupPanel("General");
		{
			ConfigWidget(f_MenuKey, false,
				u8"键切换主菜单可见性。不能为空.\n"\
				u8"如果您忘记了这个键，您可以在配置文件中查看或设置它.");
			ConfigWidget(f_HotkeysEnabled, "Enable hotkeys.");
					}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"控制台日志");
		{
			bool consoleChanged = ConfigWidget(f_ConsoleLogging,
				u8"启用控制台以记录信息（更改将在重新启动后生效)");
			if (consoleChanged && !f_ConsoleLogging)
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::ConsoleLogger);
			}

			bool fileLogging = ConfigWidget(f_FileLogging,
				u8"启用文件日志记录（更改将在重新启动后生效）.\n" \
				u8"将为日志创建应用程序目录中的文件夹.");
			if (fileLogging && !f_FileLogging)
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::FileLogger);
			}
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"激活功能窗口");
		{
			ConfigWidget(f_StatusShow);
			ConfigWidget(f_StatusMove, u8"允许移动“状态”窗口.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"Info窗口");
		{
			ConfigWidget(f_InfoShow);
			ConfigWidget(f_InfoMove, u8"允许移动“信息”窗口.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"FPS窗口");
		{
			ConfigWidget(f_FpsShow);
			ConfigWidget(f_FpsMove, u8"允许移动“FPS指示器”窗口.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"显示通知");
		{
			ConfigWidget(f_NotificationsShow, u8"窗口右下角将显示通知.");
			ConfigWidget(f_NotificationsDelay, 1, 1, 10000, u8"通知之间的延迟（毫秒）.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"快速退出");
		{
			ConfigWidget(u8"开/关",
				f_FastExitEnable,
				u8"启用快速退出.\n"
			);
			if (!f_FastExitEnable)
				ImGui::BeginDisabled();

			ConfigWidget(u8"键", f_HotkeyExit, true,
				u8"退出游戏的键.");

			if (!f_FastExitEnable)
				ImGui::EndDisabled();
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"自定义主题");
		{
			if (ConfigWidget(f_FontSize, 1, 8, 64, u8"调整界面字体大小."))
			{
				f_FontSize = std::clamp(f_FontSize.value(), 8, 64);
				renderer::SetGlobalFontSize(static_cast<float>(f_FontSize));
			}
			ImGui::Spacing();

			ConfigWidget(f_ShowStyleEditor, u8"在自定义窗口中显示颜色.");
			ImGui::Spacing();

			ImGui::Text(u8"保存自定义颜色");
			static std::string nameBuffer_;
			ImGui::InputText(u8"颜色", &nameBuffer_);
			if (ImGui::Button(u8"保存"))
				Colors_Export(nameBuffer_);
				ImGui::SameLine();

			if (std::filesystem::exists(themesDir / (nameBuffer_ + ".json")))
			{
				if (this->f_DefaultTheme.value() != nameBuffer_)
				{
					if (ImGui::Button(u8"设置为默认值"))
					{
						f_DefaultTheme = nameBuffer_;
					}
					ImGui::SameLine();
					if (ImGui::Button(u8"加载"))
					{
						Colors_Import(nameBuffer_);
					}
				}
			}
			else
			{
				ImGui::Text(u8"请输入要保存的名称.");
			}
		}
		ImGui::EndGroupPanel();
	}

	Settings& Settings::GetInstance()
	{
		static Settings instance;
		return instance;
	}

	void Settings::OnExitKeyPressed()
	{
		if (!f_FastExitEnable || CheatManagerBase::IsMenuShowed())
			return;

		ExitProcess(0);
	}
}
