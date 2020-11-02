// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WRUISlot_RecommendedVoiceCommandList.generated.h"

UCLASS()
class WR_API UWRUISlot_RecommendedVoiceCommandList : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
public:
	void SetContentsText(const FName& IN InContents);
private:
	class UWRTextBlock* WRTextBlock_Contents;
};
