// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_BattleResultBoard.generated.h"


class UTextBlock;
UCLASS()
class WR_API UWRUIPanel_BattleResultBoard : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
private:
	void Init();
private:
	UTextBlock* TextBlock_Name;
};