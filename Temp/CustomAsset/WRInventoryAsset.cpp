// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRInventoryAsset.h"

FName UWRInventoryAsset::GetSound(const EWRInventorySoundType IN InType)
{
	FWRInventorySound* SoundData = InventorySoundMap.Find(InType);
	if (SoundData == nullptr)
		return NAME_None;

	return SoundData->SFX;
}
