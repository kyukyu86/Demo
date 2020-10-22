// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DMIOBase.h"
#include "DMIOSkeletal.generated.h"

class USkeletalMeshComponent;
class UAnimSequenceBase;

UCLASS()
class THIRDPERSON_API ADMIOSkeletal: public ADMIOBase
{
	GENERATED_UCLASS_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BuildCustomComponents() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual class UMeshComponent* GetMeshComponent() override { return SkeletalMeshComponent; }
	void PlayAnimationSequence(UAnimSequenceBase* IN InAnimSequence);
};
