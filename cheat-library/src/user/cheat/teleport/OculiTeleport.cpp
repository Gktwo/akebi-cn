#include "pch-il2cpp.h"
#include "OculiTeleport.h"

#include <helpers.h>
#include <cheat/game/filters.h>

namespace cheat::feature 
{
    OculiTeleport::OculiTeleport() : ItemTeleportBase("OculiTeleport", u8"ÉñÍ«")
    { }

	const FeatureGUIInfo& OculiTeleport::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ u8"ÉñÍ«´«ËÍ", "Teleport", true };
		return info;
	}

    OculiTeleport& OculiTeleport::GetInstance()
	{
		static OculiTeleport instance;
		return instance;
	}

	bool OculiTeleport::IsValid(game::Entity* entity) const
	{
		return game::filters::combined::Oculies.IsValid(entity);
	}

}

