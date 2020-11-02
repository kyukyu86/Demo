// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_AvailableDialogueIcon.h"
#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacterNPC.h"

#include "Manager/WRCharacterManager.h"

#include "Table/WREnpcTable.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRActorUtility.h"
#include "Image.h"


void UWRUIPanel_AvailableDialogueIcon::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_Icon);

	OwnerNpcCharacter = nullptr;
}

void UWRUIPanel_AvailableDialogueIcon::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateUI();
}

void UWRUIPanel_AvailableDialogueIcon::SetOwnerCharacter(const AWRCharacterNPC* IN InNpcCharacter)
{
	OwnerNpcCharacter = InNpcCharacter;
}

void UWRUIPanel_AvailableDialogueIcon::UpdateUI()
{
	if (OwnerNpcCharacter->IsValidLowLevel() == false)
		return;

	if (WRUIManager::Get()->IsWidgetOpened(OwnerNpcCharacter, EWRUIPanelKind::CompassMark))
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	const FWRNpcTable* NpcTable = static_cast<const FWRNpcTable*>(OwnerNpcCharacter->GetNpcBaseTable());
	if (NpcTable == nullptr)
		return;

	float Dist = WRActorUtility::GetDistance(MyCharacter, Cast<AWRCharacter>(OwnerNpcCharacter));

	if (NpcTable->NpcTalkRange < Dist)
	{
		Image_Icon->SetOpacity(0.3f);
	}
	else
	{
		Image_Icon->SetOpacity(1.f);
	}
}