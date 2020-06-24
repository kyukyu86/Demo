// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUISlot_Preview.h"
#include "../../Manager/DMPreviewManager.h"

void UDMUISlot_Preview::NativeConstruct()
{
	Super::NativeConstruct();

}

void UDMUISlot_Preview::NativeDestruct()
{
	DMPreviewManager::Get()->ReleasePreview(this, true);

	Super::NativeDestruct();
}

void UDMUISlot_Preview::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UDMUISlot_Preview::OnShow()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	//SetFocusableInputMode(true, this);
}
