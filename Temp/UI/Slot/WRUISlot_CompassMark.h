// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "Manager/WRCompassManager.h"
#include "WRUISlot_CompassMark.generated.h"

struct FWRCompassMarkInfoBase;
class UTextBlock;
UCLASS()
class WR_API UWRUISlot_CompassMark : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	WRCompassMarkHandle GetCompassMarkHandle() { return CompassMarkHandle; }
	void SetCompassMarkInfo(const WRCompassMarkHandle IN InCompassMarkHandle, const FWRCompassMarkInfoBase* IN InCompassMarkInfo);
private:
	void UpdateMarkUI();
	void UpdateMarkUIByPosition();
	void UpdateMarkUIByActor();
private:
	WRCompassMarkHandle CompassMarkHandle;
	FWRCompassMarkInfoBase::EWRCompassMarkType CompassMarkType;
	FVector TargetPosition;
	class AActor* OwnerActor;

	UTextBlock* TextBlock_Dist;
};