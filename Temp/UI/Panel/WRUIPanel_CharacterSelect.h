// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"

#include "WRUIPanel_CharacterSelect.generated.h"

UCLASS()
class WR_API UWRUIPanel_CharacterSelect : public UWRUIPanel
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> CharacterSelectKeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> DeleteKeys;
	
private:
	virtual void	NativeConstruct() override;
	virtual void	NativeDestruct() override;
	virtual void	NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply	NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	void OnCharacterSelectKeyClicked();
	void OnDeleteKeyClicked();
	void OnModifyNameKeyClicked();
};