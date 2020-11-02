// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_Test.generated.h"



class UTextBlock;
UCLASS()
class WR_API UWRUISlot_Test : public UWRUISlot
{
	GENERATED_BODY()
	
public:
	UTextBlock* TextBlock_Num;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetNum(const int32 IN InNum);
};