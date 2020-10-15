// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DMIOBase.h"
#include "DMIOStatic.generated.h"

UCLASS()
class THIRDPERSON_API ADMIOStatic : public ADMIOBase
{
	GENERATED_UCLASS_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* StaticMeshComponent = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BuildCustomComponents();

public:	
	virtual void Tick(float DeltaTime) override;

};
