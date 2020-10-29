// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_Main.h"
#include "Components/TextBlock.h"
#include "../Slot/DMUISlot_Preview.h"
#include "../../Manager/DMPreviewManager.h"
#include "../../Manager/DMPathManager.h"
#include "../../GameInstance/DMGameInstance.h"
#include <GameFramework/PlayerController.h>
#include <Components/Image.h>
#include "../../Util/DMUIUtil.h"


void UDMUIPanel_Main::NativeConstruct()
{
	Super::NativeConstruct();

}

void UDMUIPanel_Main::NativeDestruct()
{
	
	Super::NativeDestruct();
}

void UDMUIPanel_Main::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateMouseCursor();
}

void UDMUIPanel_Main::UpdateMouseCursor()
{
	FVector2D MouseScreenPosition = FVector2D::ZeroVector;
	if (DMUIUtil::GetMouseScreenPosition(MouseScreenPosition))
	{
		Image_MouseCursor->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Image_MouseCursor->SetRenderTranslation(MouseScreenPosition);
	}
	else
	{
		Image_MouseCursor->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDMUIPanel_Main::UpdatePreview()
{

}
