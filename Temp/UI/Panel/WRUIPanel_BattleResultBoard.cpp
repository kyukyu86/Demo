// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_BattleResultBoard.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Utility/WRUIUtility.h"


void UWRUIPanel_BattleResultBoard::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Name);

	Init();
}

void UWRUIPanel_BattleResultBoard::Init()
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		TextBlock_Name->SetText(WRUIUtility::ConvertFNameToFText(MyCharacter->GetCharacterName()));
	}
}