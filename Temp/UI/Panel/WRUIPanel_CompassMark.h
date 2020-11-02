// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Manager/WRCompassManager.h"
#include "Define/WRTypeDefine.h"
#include "WRUIPanel_CompassMark.generated.h"


/**
 * 
 */
class UTextBlock;
class UImage;
UCLASS()
class WR_API UWRUIPanel_CompassMark : public UWRUIPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	void SetCompassMarkInfo(const WRCompassMarkHandle IN InCompassMarkHandle, const FWRCompassMarkInfoBase* IN InCompassMarkInfo);
private:
	void SetMarkUI(const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType);
	void UpdateMarkUI();
	void UpdateMarkUIByPosition();
	void UpdateMarkUIByActor();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TAssetPtr<UTexture2D> QuestionMarkUITexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TAssetPtr<UTexture2D> ExclamationMarkUITexture;
private:
	WRCompassMarkHandle CompassMarkHandle;
	FWRCompassMarkInfoBase::EWRCompassMarkType CompassMarkType;
	FVector TargetPosition;
	class AActor* OwnerActor;

	UTextBlock* TextBlock_Dist;
	UImage* Image_Mark;
	UMaterialInstanceDynamic* Mat_Mark;

	int32 PreviousMeterDist = -INT_MAX;
};
