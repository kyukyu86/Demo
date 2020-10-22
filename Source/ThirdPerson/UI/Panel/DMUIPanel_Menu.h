// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUIPanel.h"
#include "DMUIPanel_Menu.generated.h"

class UButton;

UCLASS()
class THIRDPERSON_API UDMUIPanel_Menu : public UDMUIPanel
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UButton* Button_Inventory;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		UButton* Button_Debug;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintCallable)
		void OnClicked_Inventory();
	UFUNCTION(BlueprintCallable)
		void OnClicked_Debug();
};
