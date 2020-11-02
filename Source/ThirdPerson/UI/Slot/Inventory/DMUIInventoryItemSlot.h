// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Base/DMUISlot.h"
#include <Blueprint/IUserObjectListEntry.h>
#include "DMUIInventoryItemSlot.generated.h"


class UImage;

UCLASS()
class THIRDPERSON_API UDMUIInventoryItemSlot : public UDMUISlot, public IUserObjectListEntry
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_Icon;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true", BindWidget))
		UImage* Image_Focus;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	// IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

	void SetFocusState(const bool IN InSet);
};
