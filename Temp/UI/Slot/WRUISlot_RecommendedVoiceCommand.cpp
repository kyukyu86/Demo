// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_RecommendedVoiceCommand.h"
#include "WRUISlot_RecommendedVoiceCommandList.h"
#include "VerticalBox.h"

#include "Define/WRUIDefine.h"
#include "Define/WRTypeDefine.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRVoiceRecognitionTable.h"

#include "Manager/WRVoiceContentsManager.h"


#define DEF_SLOT_RECOMMEND_VOICE_COMMAND_LIST_NUM 3 
void UWRUISlot_RecommendedVoiceCommand::NativeConstruct()
{
	Super::NativeConstruct();
	
	BIND_WIDGET(UVerticalBox, VerticalBox_List);
}

void UWRUISlot_RecommendedVoiceCommand::UpdateRecommendedVoiceCommandData()
{
	TArray<WRTableID> OutRecommendedVoiceCommandTIDs;
	WRVoiceContentsManager::Get()->GetRemmendedVoiceCommand(OutRecommendedVoiceCommandTIDs);

	TArray<int32> OutRandNums;
	GetRand(OutRandNums, 0, OutRecommendedVoiceCommandTIDs.Num() - 1, DEF_SLOT_RECOMMEND_VOICE_COMMAND_LIST_NUM);

	int8 Count = 0;
	for (int32 i = 0; i < OutRandNums.Num(); ++i)
	{
		WRTableID RecommendedVoiceCommandTID = OutRecommendedVoiceCommandTIDs[OutRandNums[i]];
		
		if (RecommendedVoiceCommandTID == INVALID_TABLE_TID)
			continue;

		FWRRecommendedVoiceCommandTable* RecommendedVoiceCommandTable = WRTableManager::Get()->FindRow<FWRRecommendedVoiceCommandTable>(RecommendedVoiceCommandTID);
		if (RecommendedVoiceCommandTable == nullptr)
			continue;

		UWRUISlot_RecommendedVoiceCommandList* Slot_RecommendedVoiceCommandList = Cast<UWRUISlot_RecommendedVoiceCommandList>(VerticalBox_List->GetChildAt(Count++));
		if (Slot_RecommendedVoiceCommandList == nullptr)
			continue;

		Slot_RecommendedVoiceCommandList->SetContentsText(RecommendedVoiceCommandTable->ContentsText);
	}

	// 데이터가 부족했다면 Default Recommend Voice Command 사용
	if (Count < DEF_SLOT_RECOMMEND_VOICE_COMMAND_LIST_NUM)
	{
		UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRDefaultRecommendedVoiceCommandTable>();
		if (DataTable == nullptr)
			return;

		TArray<WRTableID> DefaultRecommendedVoiceCommandTIDs;
		for (auto& Iter : DataTable->GetRowMap())
		{
			const FName& TIDName = Iter.Key;
			DefaultRecommendedVoiceCommandTIDs.Add(FCString::Atoi(*(TIDName.ToString())));
		}

		GetRand(OutRandNums, 0, DefaultRecommendedVoiceCommandTIDs.Num() - 1, DEF_SLOT_RECOMMEND_VOICE_COMMAND_LIST_NUM - Count);
		for (int32 i = 0; i < OutRandNums.Num(); ++i)
		{
			WRTableID DefaultRecommendedVoiceCommandTID = DefaultRecommendedVoiceCommandTIDs[OutRandNums[i]];

			if (DefaultRecommendedVoiceCommandTID == INVALID_TABLE_TID)
				continue;

			FWRDefaultRecommendedVoiceCommandTable* DefaultRecommendedVoiceCommandTable = WRTableManager::Get()->FindRow<FWRDefaultRecommendedVoiceCommandTable>(DefaultRecommendedVoiceCommandTID);
			if (DefaultRecommendedVoiceCommandTable == nullptr)
				continue;

			UWRUISlot_RecommendedVoiceCommandList* Slot_RecommendedVoiceCommandList = Cast<UWRUISlot_RecommendedVoiceCommandList>(VerticalBox_List->GetChildAt(Count++));
			if (Slot_RecommendedVoiceCommandList == nullptr)
				continue;

			Slot_RecommendedVoiceCommandList->SetContentsText(DefaultRecommendedVoiceCommandTable->ContentsText);
		}
	}
}

void UWRUISlot_RecommendedVoiceCommand::GetRand(TArray<int32>& OUT OutRandNums, const int32 IN InMin, const int32 IN InMax, uint8 IN InCount /*= 1*/)
{
	OutRandNums.Empty();

	if (InMax < InMin)
		return;

	if (InMax - InMin + 1 < InCount)
	{
		for (int32 i = InMin; i <= InMax; ++i)
		{
			OutRandNums.Add(i);
		}
		
		return;
	}

	int32 AddedNumCount = 0;
	for (int32 i = 0; i < 1000; ++i)
	{
		if (i == 999)
		{
			ensureMsgf(false, TEXT("Infinite Loop"));
		}

		int32 RandNum = FMath::RandRange(InMin, InMax);
		if(OutRandNums.Contains(RandNum))
			continue;

		OutRandNums.Add(RandNum);
		if (++AddedNumCount >= InCount)
			break;
	}
}
