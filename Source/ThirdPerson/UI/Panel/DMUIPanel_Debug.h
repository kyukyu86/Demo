// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUIPanel.h"
#include "DMUIPanel_Debug.generated.h"

class UTextBlock;

UCLASS()
class THIRDPERSON_API UDMUIPanel_Debug : public UDMUIPanel
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UTextBlock* TextBlock_Debug;

public:
	void SetDebugText(FString IN InText);
};
