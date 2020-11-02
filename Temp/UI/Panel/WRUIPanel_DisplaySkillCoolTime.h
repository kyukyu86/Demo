// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_DisplaySkillCoolTime.generated.h"


class UTextBlock;
UCLASS()
class WR_API UWRUIPanel_DisplaySkillCoolTime : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	void DisplayCoolTime(const float IN InRemainTIme);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText CoolTimeText;
private:
	UTextBlock* TextBlock_Content;

	float DisplayTime;
};