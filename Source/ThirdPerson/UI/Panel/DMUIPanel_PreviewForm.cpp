// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_PreviewForm.h"
#include "../Slot/DMUISlot_Preview.h"
#include "../../Manager/DMPathManager.h"
#include "../../Manager/DMPreviewManager.h"
#include "Components/CanvasPanel.h"
#include "../../Util/DMActorUtil.h"

void UDMUIPanel_PreviewForm::NativeConstruct()
{
	Super::NativeConstruct();

	// Loading
	CanvasPanel_Loading->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimationByName("Loading", 0.f, 0);

	// Preview
	Slot_Preview->SetVisibility(ESlateVisibility::Collapsed);
	FDMPreviewInfo PreviewInfo;
	PreviewInfo.Type = EDMPreviewType::CustomActor;
	PreviewInfo.BPPath = DMPathManager::Get()->GetPath(EDMPath::Actor) + "TestActor.TestActor";
	PreviewInfo.SetPreviewWidget(Slot_Preview);
	PreviewInfo.PreviewLoadCompletedDelegate.BindUObject(this, &UDMUIPanel_PreviewForm::OnPreviewLoadCompleted);
	PreviewInfo.ManualVisible = true;
	DMPreviewManager::Get()->SetPreview(PreviewInfo);
}

void UDMUIPanel_PreviewForm::NativeDestruct()
{
	if (TimerHandle.IsValid())
	{
		DMActorUtil::ClearTimer(TimerHandle);
	}

	Super::NativeDestruct();
}

void UDMUIPanel_PreviewForm::OnPreviewLoadCompleted(class AActor* IN InActor)
{
	TimerHandle = DMActorUtil::SetTimer(FTimerDelegate::CreateUObject(this, &UDMUIPanel_PreviewForm::OnTimer), 2.f, false);
}

void UDMUIPanel_PreviewForm::OnTimer()
{
	CanvasPanel_Loading->SetVisibility(ESlateVisibility::Collapsed);
	StopAnimationByName("Loading");

	Slot_Preview->OnShow();
	PlayAnimationByName("Show");
}
