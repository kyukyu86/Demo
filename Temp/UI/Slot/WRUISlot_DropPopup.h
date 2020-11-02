// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Queue.h"

#include "UI/Base/WRUISlot.h"
#include "Define/WRTypeDefine.h"

#include "WRUISlot_DropPopup.generated.h"

struct FWRDropInfo
{
	FWRDropInfo() : TableID(INVALID_TABLE_TID), nCount(0)
	{

	}
	WRTableID TableID;
	int32 nCount;
};
/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_DropPopup : public UWRUISlot
{
	GENERATED_BODY()

public:
	class UImage* Image_Icon;
	class UTextBlock* TextBlock_Name;
	class UTextBlock* TextBlock_AddCount;
	class UTextBlock* TextBlock_Total;

	UPROPERTY(BlueprintReadWrite, Category = "UWRUISlot_DropPopup")
		UWidgetAnimation* Ani_ShowOn;
	UPROPERTY(BlueprintReadWrite, Category = "UWRUISlot_DropPopup")
		UWidgetAnimation* Ani_ShowOff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRUISlot_Notice")
		float ShowOnTime = 3.0f;

private:
	TQueue<FWRDropInfo> Drops;
	float fShowDuringTime = 0.0f;
	bool bShowOff = false;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	void AddItem(const WRTableID IN InTableID, const int32 IN InCount);

private:
	bool GetNextItem(FWRDropInfo& OUT OutInfo);
	void ShowOnItem(const FWRDropInfo IN InInfo);
	void ShowOffitem();
	
};
