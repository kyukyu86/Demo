// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_Menu.h"
#include "../../Manager/DMUIManager.h"
#include <Components/Button.h>
#include "../../Manager/DMAsyncLoadManager.h"
#include "../../Actor/WidgetActor/DMWidgetActorInventory.h"
#include "../../Manager/DMCharacterManager.h"
#include "../../Actor/Character/Player/DMCharacterMine.h"

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
	if (strAsyncKey != "")
		return;

	auto AsyncCreateComplete = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
	{
		strAsyncKey = "";

		ADMWidgetActorInventory* CastedWidgetActorInventory = Cast<ADMWidgetActorInventory>(InObject);
		if (CastedWidgetActorInventory)
		{
			ADMCharacterMine* pMine = DMCharacterManager::Get()->GetMyCharacter();

			CastedWidgetActorInventory->SetActorLocation(pMine->GetActorLocation());
			CastedWidgetActorInventory->SetActorRotation(pMine->GetActorRotation());
		}
	});

	FString MyCharPath = "/Game/Blueprints/Actor/WidgetActor/InventoryActor_BP.InventoryActor_BP_C";
	strAsyncKey = DMAsyncLoadManager::Get()->AsyncSpawnActor(MyCharPath, AsyncCreateComplete);
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
