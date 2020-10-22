// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_Menu.h"
#include "../../Manager/DMUIManager.h"
#include <Components/Button.h>

void UDMUIPanel_Menu::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetFocusableInputMode(true, this);

	Button_Inventory->OnClicked.AddDynamic(this, &UDMUIPanel_Menu::OnClicked_Inventory);
	Button_Debug->OnClicked.AddDynamic(this, &UDMUIPanel_Menu::OnClicked_Debug);
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

//====================================================================================

void UDMUIPanel_Menu::OnClicked_Inventory()
{

}

void UDMUIPanel_Menu::OnClicked_Debug()
{
	if (DMUIManager::Get()->IsOpenedPanel(EDMPanelKind::Debug))
	{
		DMUIManager::Get()->ClosePanel(EDMPanelKind::Debug);
	}
	else
	{
		DMUIManager::Get()->OpenPanel(EDMPanelKind::Debug);
	}
}
