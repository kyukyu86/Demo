// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_ScoreBoard.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacter.h"
#include "Enum/EWRCharacter.h"
#include "Manager/WRUIManager.h"
#include "UI/Slot/WRUISlot_ScoreBoardList.h"
#include "Actor/Character/WRCharacterMine.h"
#include "VerticalBox.h"

void UWRUIPanel_ScoreBoard::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UVerticalBox, VerticalBox_List);

	Init();
}

void UWRUIPanel_ScoreBoard::Init()
{
	int8 Rank = 0;
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		UWRUISlot_ScoreBoardList* Slot_ScoreBoardList = Cast<UWRUISlot_ScoreBoardList>(WRUIManager::Get()->CreateUISlot(EWRUISlotKind::ScoreBoardList));
		if (Slot_ScoreBoardList)
		{
			VerticalBox_List->AddChild(Slot_ScoreBoardList);
			++Rank;
			Slot_ScoreBoardList->SetData(Rank, MyCharacter->GetCharacterName(), GetScore(Rank));
		}	
	}

	for (int32 i = 0; i < 4; ++i)
	{
		UWRUISlot_ScoreBoardList* Slot_ScoreBoardList = Cast<UWRUISlot_ScoreBoardList>(WRUIManager::Get()->CreateUISlot(EWRUISlotKind::ScoreBoardList));
		if (Slot_ScoreBoardList)
		{
			VerticalBox_List->AddChild(Slot_ScoreBoardList);
			++Rank;
			Slot_ScoreBoardList->SetData(Rank, GetName(Rank), GetScore(Rank));
		}
	}
}

int32 UWRUIPanel_ScoreBoard::GetScore(const int8 IN InRank)
{
	int32 Score = (10 - InRank) * 10000;
	Score += FMath::Rand() % 10000;

	return Score;
}

FName UWRUIPanel_ScoreBoard::GetName(const int8 IN InRank)
{
	switch (InRank)
	{
	case 2: return PlayerName0;
	case 3: return PlayerName1;
	case 4: return PlayerName2;
	case 5: return PlayerName3;
	default:
		break;
	}

	return "";
}
