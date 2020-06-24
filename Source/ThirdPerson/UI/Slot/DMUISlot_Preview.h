// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUISlot.h"
#include "../../PreviewStudio/DMPreviewStruct.h"
#include "DMUISlot_Preview.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUISlot_Preview : public UDMUISlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FDMPreviewBaseInfo	PreviewBaseInfo;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FORCEINLINE FDMPreviewBaseInfo& GetPreviewBaseInfo() { return PreviewBaseInfo; }

	void OnShow();
};
