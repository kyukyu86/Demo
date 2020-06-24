// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_Main.h"
#include "Components/TextBlock.h"
#include "../Slot/DMUISlot_Preview.h"
#include "../../Manager/DMPreviewManager.h"
#include "../../Manager/DMPathManager.h"


void UDMUIPanel_Main::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Name);
	BIND_WIDGET(UDMUISlot_Preview, Slot_Preview);

	UpdatePreview();
}

void UDMUIPanel_Main::NativeDestruct()
{

	Super::NativeDestruct();
}

void UDMUIPanel_Main::UpdatePreview()
{
	Slot_Preview->SetVisibility(ESlateVisibility::Collapsed);

	FDMPreviewInfo PreviewInfo;
	PreviewInfo.Type = EDMPreviewType::CustomActor;
	PreviewInfo.BPPath = DMPathManager::Get()->GetPath(EDMPath::Actor) + "TestActor.TestActor";
	PreviewInfo.SetPreviewWidget(Slot_Preview);
	DMPreviewManager::Get()->SetPreview(PreviewInfo);
}
