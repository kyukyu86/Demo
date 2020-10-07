// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Custom/TreeView/DMUITreeViewElement.h"
#include "DMUISlot_CustomTreeViewElement.generated.h"


enum class EDMCustomTreeViewType
{
	Category,
	Children,
};

UCLASS()
class THIRDPERSON_API UDMUISlot_CustomTreeViewElement : public UDMUITreeViewElement
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWidgetSwitcher* WidgetSwitcher_Type;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_CategoryIcon;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UTextBlock* TextBlock_Category;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UTextBlock* TextBlock_Element;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float LevelSpace = 20.f;

private:
	void SetCategory(const FString IN InString);
	void SetElement(const FString IN InString, const int32 IN InLevel);	

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

};
