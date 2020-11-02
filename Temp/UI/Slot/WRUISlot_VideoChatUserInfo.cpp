// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_VideoChatUserInfo.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRCharacterManager.h"

#include "Localization/WRLocalizationProcessor.h"

void UWRUISlot_VideoChatUserInfo::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Online);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_NickName);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_CharacterName);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_ShortcutNumber);
	
	BIND_WIDGET(UImage, Image_Profile);
}

void UWRUISlot_VideoChatUserInfo::SetUserInfo(const FWRVideoChatUserData& IN InVideoChatUserInfo)
{
	if (InVideoChatUserInfo.PlayerAID == INVALID_AID)
		return;

	WRTextBlock_CharacterName->SetText(InVideoChatUserInfo.CharacterName);

	if (InVideoChatUserInfo.NickName.IsNone())
	{
		WRTextBlock_NickName->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		WRTextBlock_NickName->SetText(InVideoChatUserInfo.NickName);
		WRTextBlock_NickName->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (InVideoChatUserInfo.bOnline)
	{
		WRTextBlock_Online->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_OnlineKey"));
	}
	else
	{
		WRTextBlock_Online->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_OfflineKey"));
	}

	if (InVideoChatUserInfo.ShortcutNumber == INVALID_SHORTCUT_NUMBER)
	{
		WRTextBlock_ShortcutNumber->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		WRTextBlock_ShortcutNumber->SetText(FText::AsNumber(InVideoChatUserInfo.ShortcutNumber));
		WRTextBlock_ShortcutNumber->SetVisibility(ESlateVisibility::SelfHitTestInvisible);		
	}
}