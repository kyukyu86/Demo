// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_DisplaySkillCoolTime.h"
#include "TextBlock.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRUIManager.h"

#define DEF_DISPLAY_SKILL_COOLTIME_TIME 3

void UWRUIPanel_DisplaySkillCoolTime::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Content);
	DisplayTime = 0.f;
}

void UWRUIPanel_DisplaySkillCoolTime::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (DisplayTime > 0.f)
	{
		DisplayTime -= InDeltaTime;
		if (DisplayTime <= 0)
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::DisplaySkillCoolTime);
		}
	}
}

void UWRUIPanel_DisplaySkillCoolTime::DisplayCoolTime(const float IN InRemainTIme)
{
	if (DisplayTime != 0)
		return;

	int32 RemainTIme = (InRemainTIme + 1.f);
	FString DisplayContent = CoolTimeText.ToString();
	DisplayContent.ReplaceInline(TEXT("<time>"), *FString::FromInt(RemainTIme));

	TextBlock_Content->SetText(FText::FromString(DisplayContent));
	DisplayTime = DEF_DISPLAY_SKILL_COOLTIME_TIME;
}