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
	// �ߺ����� ���� ���ڸ� �����ȿ��� �����ϰ� ã���ش�.
	// OutRandNums : ������ ���� �����ϱ� ���� ����
	// InMin : �ּҰ�, InMax : �ִ밪
	// InCount : ��� ������ ���� �ʿ�����
	// InCount�� �����ȿ� ������ ������ ���ٸ� �����ȿ� ������ ����ŭ���� �����ȴ�. ex) InMin = 3, InMax = 4, InCount = 5 -> 4 - 3 + 1 = 2
	void GetRand(TArray<int32>& OUT OutRandNums, const int32 IN InMin, const int32 IN InMax, uint8 IN InCount = 1);
private:
	class UVerticalBox* VerticalBox_List;

};
