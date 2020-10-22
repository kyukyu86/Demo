// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMCharacterBase.h"
#include "DMCharacterMonster.generated.h"


UCLASS()
class THIRDPERSON_API ADMCharacterMonster : public ADMCharacterBase
{
	GENERATED_BODY()
	
public:
	ADMCharacterMonster();


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BuildCustomComponents() override;


};
