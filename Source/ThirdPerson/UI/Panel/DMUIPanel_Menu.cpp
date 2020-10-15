// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_Menu.h"
#include "../../Manager/DMUIManager.h"

void UDMUIPanel_Menu::NativeConstruct()
{
	Super::NativeConstruct();

	SetFocusableInputMode(true, this);
}

void UDMUIPanel_Menu::NativeDestruct()
{
	SetFocusableInputMode(false);

	Super::NativeDestruct();
}

FReply UDMUIPanel_Menu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
		return FReply::Unhandled();

	if (InKeyEvent.GetKey() == EKeys::One)
	{
		DMUIManager::Get()->ClosePanel(EDMPanelKind::Menu);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}
