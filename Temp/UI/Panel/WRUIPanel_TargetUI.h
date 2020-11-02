// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_TargetUI.generated.h"


/**
 * 
 */
class UImage;
UCLASS()
class WR_API UWRUIPanel_TargetUI : public UWRUIPanel
{
	GENERATED_BODY()

private:
	UImage* Image_Lock;
	UImage* Image_Circle;

	bool bMulti;
	bool bLock;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetAnimation* LockAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetAnimation* UnLockAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor NormalColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor MultiColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor LockColor;
public:
	virtual void NativeConstruct() override;

	void OnTargetLockChanged(const bool IN bInLcok);
	void SetMulti(const bool IN bInMulti);
	
private:
	void UpdateColor();
};
