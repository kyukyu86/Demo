// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUserWidget.h"
#include "../../GameInstance/DMGameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UDMUserWidget::SetFocusableInputMode(const bool IN InUIMode, UWidget* IN InFocusWidget /*= nullptr*/)
{
	if (UDMGameInstance::GetGameInstance() == nullptr)
		return;

	APlayerController* PlayerController = UDMGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	if (InUIMode)
	{
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, InFocusWidget);
// #ifdef UE_EDITOR
// 		WRUIManager::Get()->SaveFocusedUI(InFocusWidget);
// #endif // UE_EDITOR
	}
	else
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
// #ifdef UE_EDITOR
// 		WRUIManager::Get()->ReleaseFocusedUI();
// #endif // UE_EDITOR
	}
}
