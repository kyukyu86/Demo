// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMUserWidget.h"
#include "DMUIPanel.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUIPanel : public UDMUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowOnAnimation();
	void ShowOffAnimation();
};
