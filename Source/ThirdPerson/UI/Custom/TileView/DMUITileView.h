// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TileView.h"
#include "DMUITileView.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUITileView : public UTileView
{
	GENERATED_BODY()
	
public:
	bool AddScrollOffset(const float IN InAddOffset);
};
