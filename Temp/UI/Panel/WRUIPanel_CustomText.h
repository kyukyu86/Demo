// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_CustomText.generated.h"


UCLASS()
class WR_API UWRUIPanel_CustomText : public UWRUIPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
public:
	virtual void ThrowParam(const int32 IN InParam) override;
public:
	class UWRTextBlock* WRTextBlock_Content;
};