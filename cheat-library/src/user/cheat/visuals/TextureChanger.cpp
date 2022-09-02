#include "pch-il2cpp.h"
#include "TextureChanger.h"

#include <helpers.h>
#include <cheat/events.h>
#include <misc/cpp/imgui_stdlib.h>
#include <fstream>

namespace cheat::feature
{
    namespace GameObject {
        app::GameObject* AvatarRoot = nullptr;
    }
 
    TextureChanger::TextureChanger() : Feature(),
        NF(f_Enabled, u8"�����滻", "Visuals::TextureChanger", false),
        NF(f_HeadPath, u8"ͷ", "Visuals::TextureChanger", false),
        NF(f_BodyPath, u8"��", "Visuals::TextureChanger", false),
        NF(f_DressPath, u8"ȹ��", "Visuals::TextureChanger", false),
        NF(f_GliderPath, "Glider", "Visuals::TextureChanger", false),
        toBeUpdate(), nextUpdate(0)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(TextureChanger::OnGameUpdate);
    }

    const FeatureGUIInfo& TextureChanger::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"�����滻", "Visuals", true };
        return info;
    }

    void TextureChanger::DrawMain()
    {
        ConfigWidget(f_Enabled, "Texture Changer.");
        ImGui::Text(u8"��ǰ��ɫ: %s", ActiveHero.c_str());
        ImGui::Text("Active Glider: %s", ActiveGlider.c_str());
 
        ConfigWidget(f_HeadPath, u8"ͷ.\n" \
            "Example path: C:\\Head.png");

        ConfigWidget(f_BodyPath, u8"��.\n" \
            "Example path: C:\\Body.png");

        ConfigWidget(f_DressPath, u8"ȹ��.\n" \
            "Example path: C:\\Dress.png");

        if (ImGui::Button(u8"Ӧ��"))
            "Example path: C:\\Glider.png");

        if (ImGui::Button("Apply"))
            ApplyTexture = true;
    }

    bool TextureChanger::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void TextureChanger::DrawStatus()
    {
        ImGui::Text(u8"�����滻");
    }

    TextureChanger& TextureChanger::GetInstance()
    {
        static TextureChanger instance;
        return instance;
    }

    void TextureChanger::OnGameUpdate()
    {
        if (!f_Enabled)
            return;

        auto currentTime = util::GetCurrentTimeMillisec();
        if (currentTime < nextUpdate)
            return;

        if (ApplyTexture)
        { 
            GameObject::AvatarRoot = app::GameObject_Find(string_to_il2cppi("/EntityRoot/AvatarRoot"), nullptr);

            if (GameObject::AvatarRoot != nullptr)
            {
                auto Transform = app::GameObject_GetComponentByName(GameObject::AvatarRoot, string_to_il2cppi("Transform"), nullptr);
                auto HeroCount = app::Transform_get_childCount(reinterpret_cast<app::Transform*>(Transform), nullptr);

                for (int i = 0; i <= HeroCount - 1; i++)
                {
                    auto HeroComponent = app::Transform_GetChild(reinterpret_cast<app::Transform*>(Transform), i, nullptr);
                    auto HeroGameObject = app::Component_1_get_gameObject(reinterpret_cast<app::Component_1*>(HeroComponent), nullptr);
                    auto isActiveHero = app::GameObject_get_active(HeroGameObject, nullptr);

                    if (isActiveHero)
                    {
                        auto GameObjectName = app::Object_1_get_name(reinterpret_cast<app::Object_1*>(HeroGameObject), nullptr);
                        ActiveHero = il2cppi_to_string(GameObjectName);
                        auto OffsetDummy = app::GameObject_Find(string_to_il2cppi("/EntityRoot/AvatarRoot/" + il2cppi_to_string(GameObjectName) + "/OffsetDummy"), nullptr);
                        auto TransformOffsetDummy = app::GameObject_GetComponentByName(OffsetDummy, string_to_il2cppi("Transform"), nullptr);
                        auto TransformChildOffsetDummy = app::Transform_GetChild(reinterpret_cast<app::Transform*>(TransformOffsetDummy), 0, nullptr);
                        auto OffsetGameObject = app::Component_1_get_gameObject(reinterpret_cast<app::Component_1*>(TransformChildOffsetDummy), nullptr);
                        auto OffsetGameObjectName = app::Object_1_get_name(reinterpret_cast<app::Object_1*>(OffsetGameObject), nullptr);

                        //Specific to Avatar
                        auto AvatarBody = app::GameObject_Find(string_to_il2cppi("/EntityRoot/AvatarRoot/" + il2cppi_to_string(GameObjectName) + "/OffsetDummy/" + il2cppi_to_string(OffsetGameObjectName) + "/Body"), nullptr);
                        auto AvatarSkinnedMeshRenderer = app::GameObject_GetComponentByName(reinterpret_cast<app::GameObject*>(AvatarBody), string_to_il2cppi("SkinnedMeshRenderer"), nullptr);
                        auto AvatarMaterial = app::Renderer_GetMaterialArray(reinterpret_cast<app::Renderer*>(AvatarSkinnedMeshRenderer), nullptr);

                        //Specific to Glider
                        auto GliderRoot = app::GameObject_Find(string_to_il2cppi("/EntityRoot/AvatarRoot/" + il2cppi_to_string(GameObjectName) + "/OffsetDummy/" + il2cppi_to_string(OffsetGameObjectName) + "/Bip001/Bip001 Pelvis/Bip001 Spine/Bip001 Spine1/Bip001 Spine2/+FlycloakRootB CB A01"), nullptr);
                        auto TransformGliderRoot = app::GameObject_GetComponentByName(GliderRoot, string_to_il2cppi("Transform"), nullptr);
                        auto TransformGlider = app::Transform_GetChild(reinterpret_cast<app::Transform*>(TransformGliderRoot), 0, nullptr);
                        auto Glider = app::Component_1_get_gameObject(reinterpret_cast<app::Component_1*>(TransformGlider), nullptr);
                        auto GliderName = app::Object_1_get_name(reinterpret_cast<app::Object_1*>(Glider), nullptr);
                        ActiveGlider = il2cppi_to_string(GliderName);
                        auto TransformGliderModelRoot = app::Transform_GetChild(reinterpret_cast<app::Transform*>(TransformGlider), 1, nullptr);
                        auto TransformGliderModel = app::Transform_GetChild(reinterpret_cast<app::Transform*>(TransformGliderModelRoot), 0, nullptr);
                        auto GliderModel = app::Component_1_get_gameObject(reinterpret_cast<app::Component_1*>(TransformGliderModel), nullptr);
                        auto GliderSkinnedMeshRenderer = app::GameObject_GetComponentByName(reinterpret_cast<app::GameObject*>(GliderModel), string_to_il2cppi("SkinnedMeshRenderer"), nullptr);
                        auto GliderMaterial = app::Renderer_GetMaterialArray(reinterpret_cast<app::Renderer*>(GliderSkinnedMeshRenderer), nullptr);

                        // 0 - Hair, 1 - Body, 2 - Dress
                        if (f_HeadPath && CheckFile(f_HeadPath)) {
                            auto HeadTexture = app::NativeGallery_LoadImageAtPath(string_to_il2cppi(f_HeadPath), 100, false, false, false, nullptr);
                            app::Material_set_mainTexture(AvatarMaterial->vector[0], reinterpret_cast<app::Texture*>(HeadTexture), nullptr);
                        }
                        if (f_BodyPath && CheckFile(f_BodyPath)) {
                            auto BodyTexture = app::NativeGallery_LoadImageAtPath(string_to_il2cppi(f_BodyPath), 100, false, false, false, nullptr);
                            app::Material_set_mainTexture(AvatarMaterial->vector[1], reinterpret_cast<app::Texture*>(BodyTexture), nullptr);
                        }
                        if (f_DressPath && CheckFile(f_DressPath)) {
                            auto DressTexture = app::NativeGallery_LoadImageAtPath(string_to_il2cppi(f_DressPath), 100, false, false, false, nullptr);
                            
                            if (AvatarMaterial->vector[2] != nullptr)
                                app::Material_set_mainTexture(AvatarMaterial->vector[2], reinterpret_cast<app::Texture*>(DressTexture), nullptr);
                        }         

                        //Glider
                        if (f_GliderPath && CheckFile(f_GliderPath))
                        {
                            auto GliderTexture = app::NativeGallery_LoadImageAtPath(string_to_il2cppi(f_GliderPath), 100, false, false, false, nullptr);
                            app::Material_set_mainTexture(GliderMaterial->vector[0], reinterpret_cast<app::Texture*>(GliderTexture), nullptr);
                        }
                        ApplyTexture = false;
                    }
                }
            }
        }         
        nextUpdate = currentTime + (int)f_DelayUpdate;
    }

    bool TextureChanger::CheckFile(const std::string& Filename) {
        struct stat buffer;
        return (stat(Filename.c_str(), &buffer) == 0);
    }     
}