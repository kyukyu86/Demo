// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRChatBubbleManager.h"
#include "WRUIManager.h"
#include "WRConsoleManager.h"
#include "WRCharacterManager.h"
#include "WRInputSystemManager.h"
#include "Actor/Character/WRCharacter.h"
#include "UI/Panel/WRUIPanel_ChatBubble.h"


//========================================================================================================================================================================
WRChatBubbleManager::WRChatBubbleManager()
{
}

WRChatBubbleManager::~WRChatBubbleManager()
{
}
	
void WRChatBubbleManager::OnInitialize()
{
	
}

void WRChatBubbleManager::OnShutdown()
{
	
}

void WRChatBubbleManager::OnLoadLevelStart()
{
	
}

void WRChatBubbleManager::OnLoadLevelComplete()
{
		
}

void WRChatBubbleManager::OnRenderStart()
{
	
}

bool WRChatBubbleManager::OnTick(float InDeltaTime)
{
	return true;
}

void WRChatBubbleManager::OpenChatBubble(const AWRCharacter* IN InCharacter, const FText& IN InContent, const float IN InTime /*= 0.f*/)
{
	if (InCharacter->IsValidLowLevel() == false)
		return;

	UWRUIPanel_ChatBubble* Panel_ChatBubble = WRUIManager::Get()->FindUI<UWRUIPanel_ChatBubble>(InCharacter, EWRUIPanelKind::ChatBubble);
	if (Panel_ChatBubble)
	{
		Panel_ChatBubble->AddChatBubble(InContent);
	}
	else
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::CommonUI_FixedSize) * 0.0002f);
				InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("Dialogue")));
				Cast<UWRUIPanel_ChatBubble>(InCombinedPanelData.UIPanel)->AddChatBubble(InContent, InTime);
			}
		});
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::ChatBubble, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, const_cast<AWRCharacter*>(InCharacter));
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::ChatBubble, InCharacter, FTransform::Identity, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	}
}

void WRChatBubbleManager::CloseChatBubble(const AWRCharacter* IN InCharacter)
{
	if (InCharacter->IsValidLowLevel() == false)
		return;

	WRUIManager::Get()->CloseUI(InCharacter, EWRUIPanelKind::ChatBubble);
}

void WRChatBubbleManager::OnInformSpeechBubbleReceived(const AID_t IN InAID, const EWRLanguageType::en IN InLanguageType, const FString& InStr)
{
	if (InLanguageType == EWRLanguageType::en(WRConsoleManager::Get()->GetInt(EWRConsole::LanguageType)))
	{
		AWRCharacter* Character = WRCharacterManager::Get()->FindbyAID(InAID);
		if (Character)
		{
			OpenChatBubble(Character, FText::FromString(InStr));
		}
	}
	else
	{
		RequestTranslationChat(InAID, InLanguageType, EWRLanguageType::en(WRConsoleManager::Get()->GetInt(EWRConsole::LanguageType)), InStr);
	}
}

void WRChatBubbleManager::OnResponseTranslation(const int32 IN InIdentify, const FString& InStr)
{
	AID_t* PlayerAID = TranslationList.Find(InIdentify);
	if (PlayerAID == nullptr)
		return;

	TranslationList.Remove(InIdentify);
	if (InStr.IsEmpty())
		return;

	AWRCharacter* Character = WRCharacterManager::Get()->FindbyAID(*PlayerAID);
	if (Character == nullptr)
		return;

	OpenChatBubble(Character, FText::FromString(InStr));
}

void WRChatBubbleManager::RequestTranslationChat(const AID_t IN InAID, const EWRLanguageType::en IN InSrcLanguageType, const EWRLanguageType::en IN InDestLanguageType, const FString& IN InStr)
{
	if (InAID == INVALID_AID)
		return;

	int32 Identify = WRInputSystemManager::Get()->Translation(InSrcLanguageType, InDestLanguageType, InStr);

	if (Identify == 0)
		return;

	TranslationList.Add(Identify, InAID);
}
