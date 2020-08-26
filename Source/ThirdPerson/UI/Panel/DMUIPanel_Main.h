// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUIPanel.h"
#include "DMUIPanel_Main.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUIPanel_Main : public UDMUIPanel
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UTextBlock* TextBlock_Name;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void UpdatePreview();
};
