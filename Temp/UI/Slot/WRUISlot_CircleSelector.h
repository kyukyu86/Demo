// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "Enum/EWRCharacter.h"
#include "Enum/EWRUIEnum.h"
#include "WRUISlot_CircleSelector.generated.h"



class UCanvasPanel;
class UImage;
class UWRUISlot;
class UMaterialInstanceDynamic;
UCLASS()
class WR_API UWRUISlot_CircleSelector : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void NativeDestruct() override;

	void SetChildNum(const int32 IN InNum);
	void OnKeyEventReceived(const EWRInput IN InInput);
	void FocusElement(const int32 IN InIndex);
	void FocusElementByDegree(const float IN InDegree);
private:
	void SetElementPosition(UWRUISlot* IN InSlot, const int32 IN InIndex);
	void CreateSlotAync(const int32 IN InIndex);
	void OnCreateSlotCompleted();
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UCanvasPanel* CanvasPanel_ChildList;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_SelectCircle;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_OnlyVisibleCircle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FSlateBrush OnlyVisibleCircleBrush;

	UPROPERTY(EditAnywhere, Category = "Setting")
		EWRUISlotKind ElementSlotKind = EWRUISlotKind::None;
	UPROPERTY(EditAnywhere, Category = "Setting")
		int32 CircleRadius = 500;
	UPROPERTY(EditAnywhere, Category="Setting")
		int32 InnerCircleRadius = 300;
	UPROPERTY(EditAnywhere, Category = "Setting")
		int32 SelectCircleRadius = 540;
	UPROPERTY(EditAnywhere, Category = "Setting")
		int32 NumOfDivision = 8;	

	UMaterialInstanceDynamic* Mat_Circle;
	TDoubleLinkedList<FString> AsyncKeyList;
	int32 FocusedSlotIndex = -1;
};