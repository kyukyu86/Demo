// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Character/WRCharacter.h"
#include "WRCharacterObjectBase.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRCharacterObjectBase : public AWRCharacter
{
	GENERATED_BODY()
	
public:
	AWRCharacterObjectBase();
	AWRCharacterObjectBase(const FObjectInitializer& ObjectInitializer);

public:
	FORCEINLINE class UStaticMeshComponent* GetStaticMesh() const {
		return StaticMesh;
	}

	class UMeshComponent* GetCurrentMesh() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(Category = "Object Setting", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;
};
