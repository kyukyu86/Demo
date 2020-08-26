// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUIPanel.h"
#include "DMUIPanel_PreviewForm.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUIPanel_PreviewForm : public UDMUIPanel
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UDMUISlot_Preview* Slot_Preview;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel* CanvasPanel_Loading;

	FTimerHandle TimerHandle;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnPreviewLoadCompleted(class AActor* IN InActor);
	void OnTimer();
};
