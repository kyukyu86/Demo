// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_DrinkIcon.generated.h"


enum class EWRDrinkIconType : uint8 
{
	None,
	AnswerWaiting,
	AnswerYes,
	AnswerNo,
	Suggester,
	Possible,
};

UCLASS()
class WR_API UWRUIPanel_DrinkIcon : public UWRUIPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetIconType(const EWRDrinkIconType IN InType);

private:
	class UWidgetSwitcher* WidgetSwitcher_Icons;
};
