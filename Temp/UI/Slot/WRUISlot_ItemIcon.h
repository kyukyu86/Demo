// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WRUISlot_ItemIcon.generated.h"


struct FWRItemIcon
{
	FWRItemIcon() : strIconPath(""), ItemCount(0), bUseCount(false), bSpawned(false)
	{

	}

	FString strIconPath;
	int32 ItemCount;
	bool bUseCount;
	bool bSpawned;
};

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_ItemIcon : public UWRUISlot
{
	GENERATED_BODY()
	
private:
	class UImage* Image_Icon;
	class UWRTextBlock* WRTextBlock_Count;
	class UBorder* Border_Select;
	class UBorder* Border_Hover;

	FString strImageAsyncLoadKey;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetIcon(FWRItemIcon& IN InInfo);
	void SetSelect(const bool IN InSet);
	void SetHover(const bool IN InSet);

	void OnLoadCompletedForIconTexture(UObject* IN InIconTexture, FString IN InHash);
};
