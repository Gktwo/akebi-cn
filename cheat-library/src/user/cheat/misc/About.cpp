#include "pch-il2cpp.h"
#include "About.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature 
{
    const FeatureGUIInfo& About::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", u8"关于", false };
        return info;
    }

    void About::DrawMain()
    {
        ImGui::TextColored(ImColor(199, 16, 162, 255), u8"中文更新: ");
        TextURL(u8"中文qq群", "https://jq.qq.com/?_wv=1027&k=JNXeTQFV", true, false);

        ImGui::TextColored(ImColor(28, 202, 214, 255), "Akebi官网Github:");
        TextURL(u8"官网Github链接", "https://github.com/Akebi-Group/Akebi-GC", true, false);

		ImGui::TextColored(ImColor(28, 202, 214, 255), "Akebi官网discord:");
		TextURL(u8"官网Discord 邀请链接", "https://discord.com/invite/MmV8hNZB9S", true, false);

        ImGui::Text(u8"中文汉化:");
        ImGui::SameLine();
        ImGui::TextColored(ImColor(0, 102, 255, 255), "gktwo,dongfang");

        ImGui::Text(u8"创始人:");
        ImGui::SameLine();
        ImGui::TextColored(ImColor(0, 102, 255, 255), "Callow");
        
		ImGui::Text(u8"主要开发者:");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(0, 102, 255, 255), "Taiga");

		ImGui::Text(u8"主要贡献者:");
		ImGui::TextColored(ImColor(0, 102, 255, 255), "RyujinZX, WitchGod, m0nkrel, Shichiha, harlanx, andiabrudan, hellomykami");

		ImGui::Text(u8"全部贡献者名单:");
		TextURL("Github link", "https://github.com/Akebi-Group/Akebi-GC/graphs/contributors", true, false);

        ImGui::TextColored(ImColor(199, 16, 162, 255), u8"倒卖，转载准则:");
        ImGui::SameLine();
        ImGui::TextColored(ImColor(255, 255, 0, 255), u8"软件开源免费，搬运请标明出处，作者，转载前请联系相关作者，禁止一切涉及交易行为");

        ImGui::TextColored(ImColor(199, 16, 162, 255), u8"倒卖，未经允许搬运公示:");
        ImGui::SameLine();
        ImGui::TextColored(ImColor(255, 0, 0, 255), u8"偏爱技术论坛（之前我搬运过东方的，这个翻译他有参与过，现在有人说我后来发的也有搬运，\n你这论坛注册掏钱，在我看来和倒卖行为没有区别）");
    }


    About& About::GetInstance()
    {
        static About instance;
        return instance;
    }
}