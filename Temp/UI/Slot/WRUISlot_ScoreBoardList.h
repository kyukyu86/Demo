// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_ScoreBoardList.generated.h"

/**
 * 
 */
class UTextBlock;
UCLASS()
class WR_API UWRUISlot_ScoreBoardList : public UWRUISlot
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
public:
	void SetData(const int32 IN InRank, const FName& IN InName, const int32 IN InScore);
private:
	UTextBlock* TextBlock_Rank;
	UTextBlock* TextBlock_Name;
	UTextBlock* TextBlock_Score;
};
