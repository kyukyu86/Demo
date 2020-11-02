// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_Compass.generated.h"


class UOverlay;
class UImage;
class UWRUISlot_CompassMark;
struct FWRCompassMarkInfoBase;
UCLASS()
class WR_API UWRUISlot_Compass : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	UOverlay* Overlay_Marks;
	UImage* Image_Compass;
	UMaterialInstanceDynamic* DynamicMaterial_Compass;
public:
	void InitCompassUI();
	void ClearCompassUI();
	void AddMark(const WRCompassMarkHandle IN InCompassMarkHandle, const FWRCompassMarkInfoBase* IN InCompassMarkInfo);
	void RemoveMark(const WRCompassMarkHandle IN InCompassMarkHandle);
private:
	void UpdateCompassUI();
	UWRUISlot_CompassMark* FindCompassMark(const WRCompassMarkHandle IN InCompassMarkHandle);
};