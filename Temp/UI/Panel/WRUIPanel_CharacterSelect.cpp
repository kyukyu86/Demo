// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIPanel_CharacterSelect.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Network/IMMessageSender.h"
#include "Manager/WRAccountManager.h"


void UWRUIPanel_CharacterSelect::NativeConstruct()
{
	Super::NativeConstruct();
	
	
	SetFocusableInputMode(true, this);
}

void UWRUIPanel_CharacterSelect::NativeDestruct()
{
	SetFocusableInputMode(false);

	Super::NativeDestruct();
}

void UWRUIPanel_CharacterSelect::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UWRUIPanel_CharacterSelect::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (CharacterSelectKeys.Contains(InKeyEvent.GetKey()))
	{
		OnCharacterSelectKeyClicked();
	}
	else if (DeleteKeys.Contains(InKeyEvent.GetKey()))
	{
		OnDeleteKeyClicked();
	}
	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

void UWRUIPanel_CharacterSelect::OnCharacterSelectKeyClicked()
{
	IMMessageSender::RequestSelectPlayer();
}

void UWRUIPanel_CharacterSelect::OnDeleteKeyClicked()
{
	IMMessageSender::RequestDeletePlayer(WRAccountManager::Get()->GetPSN());
}