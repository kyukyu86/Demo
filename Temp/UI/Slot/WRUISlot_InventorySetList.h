// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "Enum/EWRItem.h"
#include "WRUISlot_InventorySetList.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_InventorySetList : public UWRUISlot
{
	GENERATED_BODY()
	
private:
	TArray<class UWRFocusButton*> FocusButton_List;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION(BlueprintCallable)
		void OnClicked_List(int32 InIndex);

};
