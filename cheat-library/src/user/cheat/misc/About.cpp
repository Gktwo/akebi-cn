#include "pch-il2cpp.h"
#include "About.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature 
{
    const FeatureGUIInfo& About::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", u8"����", false };
        return info;
    }

    void About::DrawMain()
    {
        ImGui::TextColored(ImColor(199, 16, 162, 255), u8"���ĸ���: ");
        TextURL(u8"����qqȺ", "https://jq.qq.com/?_wv=1027&k=JNXeTQFV", true, false);

        ImGui::TextColored(ImColor(28, 202, 214, 255), "Akebi����Github:");
        TextURL(u8"����Github����", "https://github.com/Akebi-Group/Akebi-GC", true, false);

		ImGui::TextColored(ImColor(28, 202, 214, 255), "Akebi����discord:");
		TextURL(u8"����Discord ��������", "https://discord.com/invite/MmV8hNZB9S", true, false);

        ImGui::Text(u8"���ĺ���:");
        ImGui::SameLine();
        ImGui::TextColored(ImColor(0, 102, 255, 255), "gktwo,dongfang");

        ImGui::Text(u8"��ʼ��:");
        ImGui::SameLine();
        ImGui::TextColored(ImColor(0, 102, 255, 255), "Callow");
        
		ImGui::Text(u8"��Ҫ������:");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(0, 102, 255, 255), "Taiga");

		ImGui::Text(u8"��Ҫ������:");
		ImGui::TextColored(ImColor(0, 102, 255, 255), "RyujinZX, WitchGod, m0nkrel, Shichiha, harlanx, andiabrudan, hellomykami");

		ImGui::Text(u8"ȫ������������:");
		TextURL("Github link", "https://github.com/Akebi-Group/Akebi-GC/graphs/contributors", true, false);

        ImGui::TextColored(ImColor(199, 16, 162, 255), u8"������ת��׼��:");
        ImGui::SameLine();
        ImGui::TextColored(ImColor(255, 255, 0, 255), u8"�����Դ��ѣ�������������������ߣ�ת��ǰ����ϵ������ߣ���ֹһ���漰������Ϊ");

        ImGui::TextColored(ImColor(199, 16, 162, 255), u8"������δ��������˹�ʾ:");
        ImGui::SameLine();
        ImGui::TextColored(ImColor(255, 0, 0, 255), u8"ƫ��������̳��֮ǰ�Ұ��˹������ģ�����������в��������������˵�Һ�������Ҳ�а��ˣ�\n������̳ע����Ǯ�����ҿ����͵�����Ϊû������");
    }


    About& About::GetInstance()
    {
        static About instance;
        return instance;
    }
}