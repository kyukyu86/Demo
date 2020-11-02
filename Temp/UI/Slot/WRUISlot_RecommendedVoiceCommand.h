// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WRUISlot_RecommendedVoiceCommand.generated.h"

UCLASS()
class WR_API UWRUISlot_RecommendedVoiceCommand : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

public:
	void UpdateRecommendedVoiceCommandData();
private:
	// 중복되지 않은 숫자를 범위안에서 랜덤하게 찾아준다.
	// OutRandNums : 랜덤한 값을 저장하기 위한 공간
	// InMin : 최소값, InMax : 최대값
	// InCount : 몇개의 랜덤한 값이 필요한지
	// InCount가 범위안에 숫자의 수보다 많다면 범위안에 숫자의 수만큼으로 조정된다. ex) InMin = 3, InMax = 4, InCount = 5 -> 4 - 3 + 1 = 2
	void GetRand(TArray<int32>& OUT OutRandNums, const int32 IN InMin, const int32 IN InMax, uint8 IN InCount = 1);
private:
	class UVerticalBox* VerticalBox_List;

};
