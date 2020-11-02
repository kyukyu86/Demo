// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRDoubleClickManager.h"
#include "WRConsoleManager.h"
#include "WRUIManager.h"
#include "UI/Panel/WRUIPanel_SelectDialogue.h"
#include "UI/Panel/WRUIPanel_SocialMenu.h"
#include "UWRVoiceController.h"
#include "Enum/EWRUIEnum.h"
#include "WRInputSystemManager.h"
#include "Enum/EWRInputSystem.h"
#include "WRVoiceContentsManager.h"
#include "WRSpawnSimulationManager.h"
#include "WRCharacterManager.h"
#include "UI/Panel/WRUIPanel_VideoChatScreen.h"
#include "UI/Panel/WRUIPanel_VideoChatUserList.h"
#include "GameMode/WRGameModeBase.h"
#include "Kismet/GameplayStatics.h"


//========================================================================================================================================================================
WRDoubleClickManager::WRDoubleClickManager()
{
}

WRDoubleClickManager::~WRDoubleClickManager()
{
}
	
void WRDoubleClickManager::OnInitialize()
{
}

void WRDoubleClickManager::OnShutdown()
{
}

void WRDoubleClickManager::OnLoadLevelStart()
{
}

void WRDoubleClickManager::OnLoadLevelComplete()
{
	
}

void WRDoubleClickManager::OnRenderStart()
{
	
}

bool WRDoubleClickManager::OnTick(float InDeltaTime)
{
	if (InputForPressHolding != EWRInput::None)
	{
		FlowPressHoldingTime += InDeltaTime;
		if (FlowPressHoldingTime > WRConsoleManager::Get()->GetFloat(EWRConsole::Input_PressHoldingTime))
		{
			OnPressHolded(InputForPressHolding);
		}
	}

	if (bFlowDoubleClickTime && BeforeInputForDoubleClick != EWRInput::None)
	{
		FlowDoubleClickTime += InDeltaTime;
		if (FlowDoubleClickTime > WRConsoleManager::Get()->GetFloat(EWRConsole::Input_DoubleClickTime))
		{
			OnClicked(BeforeInputForDoubleClick);
			BeforeInputForDoubleClick = EWRInput::None;
			FlowDoubleClickTime = 0.f;
			bFlowDoubleClickTime = false;
		}
	}

	return true;
}

void WRDoubleClickManager::OnInputPressed(const EWRInput IN InInput)
{
	if (BeforeInputForDoubleClick == EWRInput::None)
	{
		BeforeInputForDoubleClick = InInput;
	}
	else if(BeforeInputForDoubleClick != InInput)
	{
		OnClicked(BeforeInputForDoubleClick);
		BeforeInputForDoubleClick = InInput;
		bFlowDoubleClickTime = false;
	}
	else if(bFlowDoubleClickTime)
	{
		OnDoubleClicked(BeforeInputForDoubleClick);
		BeforeInputForDoubleClick = EWRInput::None;
		bFlowDoubleClickTime = false;	
	}

	FlowDoubleClickTime = 0.f;
	StartPressHoding(InInput);
}

void WRDoubleClickManager::OnInputReleased(const EWRInput IN InInput)
{
	if (InInput == InputForPressHolding)
	{
		StopPressHoding();
	}

	if (InInput == BeforeInputForDoubleClick)
	{
		if (bFlowDoubleClickTime)
		{
			OnClicked(InInput);
			BeforeInputForDoubleClick = EWRInput::None;
			bFlowDoubleClickTime = false;
			bFlowDoubleClickTime = 0.f;
		}
		else
		{
			bFlowDoubleClickTime = true;
			FlowDoubleClickTime = 0.f;
		}
	}
}

void WRDoubleClickManager::OnClicked(const EWRInput IN InInput)
{
	UE_LOG(LogTemp, Display, TEXT("OnClicked : %s"), *WREnumUtility::EnumToString("EWRInput", InInput));

	if (InInput == EWRInput::Square)
	{
		if (WRInputSystemManager::Get()->IsChatRecording())
		{
			WRVoiceContentsManager::Get()->StopVoiceRecording();
		}

		if (WRInputSystemManager::Get()->IsVoiceRecording())
		{
			WRVoiceContentsManager::Get()->CancelRecording();
		}		
		else if (WRInputSystemManager::Get()->IsChatRecording() == false)
		{
			if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SelectDialogue))
			{
				UWRUIPanel_SelectDialogue* Panel_SelectDialogue = WRUIManager::Get()->FindUI<UWRUIPanel_SelectDialogue>(EWRUIPanelKind::SelectDialogue);
				if (Panel_SelectDialogue)
					Panel_SelectDialogue->StartVoiceRecognition();
			}
			else if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
			{
				UWRUIPanel_SocialMenu* Panel_SocialMenu = WRUIManager::Get()->FindUI<UWRUIPanel_SocialMenu>(EWRUIPanelKind::SocialMenu);
				if (Panel_SocialMenu)
					Panel_SocialMenu->StartVoiceRecognition();
			}
			else if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::VideoChatUserList))
			{
				UWRUIPanel_VideoChatUserList* Panel_VideoChatUserList = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatUserList>(EWRUIPanelKind::VideoChatUserList);
				if (Panel_VideoChatUserList)
					Panel_VideoChatUserList->StartVoiceRecognition();
			}
			else if (WRVoiceContentsManager::Get()->GetVideoChatController().IsPossibleVoiceRecognitionInVideoChatScreen())
			{
				UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
				if (Panel_VideoChatScreen)
					Panel_VideoChatScreen->StartVoiceRecognition();
			}
			else
			{
				WRVoiceContentsManager::Get()->StartDefaultVoiceRecording();
			}
		}
	}
}

void WRDoubleClickManager::OnDoubleClicked(const EWRInput IN InInput)
{
	UE_LOG(LogTemp, Display, TEXT("OnDoubleClicked : %s"), *WREnumUtility::EnumToString("EWRInput", InInput));
}

void WRDoubleClickManager::OnPressHolded(const EWRInput IN InInput)
{
	UE_LOG(LogTemp, Display, TEXT("OnPressHolded : %s"), *WREnumUtility::EnumToString("EWRInput", InInput));
	if (InInput == EWRInput::Square)
	{
		if (WRCharacterManager::IsValid() && WRVoiceContentsManager::IsValid())
		{
			const AWRCharacter* MyTargetCharacter = WRCharacterManager::Get()->GetMyTarget();
			if(WRVoiceContentsManager::Get()->GetVideoChatController().GetCurrentVideoChatState() == WRVideoChatController::EWRVideoChatState::Connected)
			{
				UWRUIPanel_VideoChatScreen* Panel_VideoChatScreen = WRUIManager::Get()->FindUI<UWRUIPanel_VideoChatScreen>(EWRUIPanelKind::VideoChatScreen);
				if (Panel_VideoChatScreen)
				{
					Panel_VideoChatScreen->OnChatRecordingPressed();
				}
			}
			else if (MyTargetCharacter && WRVoiceContentsManager::Get()->ContainsInMyTranslatingPlayerList(MyTargetCharacter->GetActorID()) == false)
			{
				WRVoiceContentsManager::Get()->RequestRegistSpeaker(MyTargetCharacter->GetActorID());
			}
		}
	}

	BeforeInputForDoubleClick = EWRInput::None;
	bFlowDoubleClickTime = false;
	FlowDoubleClickTime = 0.f;

	InputForPressHolding = EWRInput::None;
	FlowPressHoldingTime = 0.f;
}

void WRDoubleClickManager::StartPressHoding(const EWRInput IN InInput)
{
	InputForPressHolding = InInput;
	FlowPressHoldingTime = 0.f;
}

void WRDoubleClickManager::StopPressHoding()
{
	InputForPressHolding = EWRInput::None;
	FlowPressHoldingTime = 0.f;
}
