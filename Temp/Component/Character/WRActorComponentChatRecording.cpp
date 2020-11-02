// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentChatRecording.h"
#include "WidgetComponent/WRWidgetComponentBase.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRVoiceContentsManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRNoticeManager.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRPlayerTable.h"
#include "Table/WRCustomTextTable.h"

#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacter.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WRUIUtility.h"

#include "Enum/EWRCharacter.h"
#include "UI/Panel/WRUIPanel_ChatRecordingGaugeIcon.h"
#include "Network/IMMessageSender.h"
#include "Camera/CameraComponent.h"

void UWRActorComponentChatRecording::OnCreateComponent()
{
	Super::OnCreateComponent();

}

void UWRActorComponentChatRecording::OnDestroyComponent()
{	
	Super::OnDestroyComponent();
}

void UWRActorComponentChatRecording::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bLookingTarget == false && bActiveGaugeIcon == false && bActiveDisabledIcon == false)
	{
		CheckLookingTargetHead();
	}
	else if (bLookingTarget == true)
	{
		FlowLookingTargetTime += DeltaTime;
		if (FlowLookingTargetTime > 1.f)
		{
			OnLookTargetCompleted();
		}
	}

	CheckMyTranslatingPlayerDistance();
}

void UWRActorComponentChatRecording::OnMyCharacterTargetChanged(const AWRCharacter* IN InTargetCharacter)
{
	CheckTickNeeded(InTargetCharacter);
}

void UWRActorComponentChatRecording::OnGaugingCompleted()
{
	bActiveGaugeIcon = false;
	WRUIManager::Get()->CloseUI(EWRUIPanelKind::ChatRecordingGaugeIcon, true);

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
		return;

	const AWRCharacter* TargetCharacter = OwnerCharacter->GetTargetCharacter();
	if (TargetCharacter == nullptr)
		return;

	WRVoiceContentsManager::Get()->RequestRegistSpeaker(TargetCharacter->GetActorID());

	CheckTickNeeded();
}

void UWRActorComponentChatRecording::CheckTickNeeded(const AWRCharacter* IN InTargetCharacter)
{
	if (WRVoiceContentsManager::Get()->IsEmptyMyTranslatingPlayerList() == false)
	{
		SetEnableTick(true);
		return;
	}

	if (InTargetCharacter == nullptr)
	{
		if (bActiveGaugeIcon)
			DeActiveGuageIcon();
		else if (bLookingTarget)
			StopLookingTarget();
		else if (bActiveDisabledIcon)
			DeActiveDisabledIcon();
		
		SetEnableTick(false);
	}
	else if (InTargetCharacter->GetCharacterType() != EWRCharacterType::Player)
	{
		if (bActiveGaugeIcon)
			DeActiveGuageIcon();
		else if (bLookingTarget)
			StopLookingTarget();
		else if (bActiveDisabledIcon)
			DeActiveDisabledIcon();

		SetEnableTick(false);
	}
	else if (WRVoiceContentsManager::Get()->ContainsInMyTranslatingPlayerList(InTargetCharacter->GetActorID()))
	{
		if (bActiveGaugeIcon)
			DeActiveGuageIcon();
		else if (bLookingTarget)
			StopLookingTarget();
		else if (bActiveDisabledIcon)
			DeActiveDisabledIcon();

		SetEnableTick(false);
	}
	else if (WRVoiceContentsManager::Get()->ContainsInWaitTranslatingPacketList(InTargetCharacter->GetActorID()))
	{
		if (bActiveGaugeIcon)
			DeActiveGuageIcon();
		else if (bLookingTarget)
			StopLookingTarget();
		else if (bActiveDisabledIcon)
			DeActiveDisabledIcon();

		SetEnableTick(false);
	}
	else
	{
		SetEnableTick(true);
	}
}

void UWRActorComponentChatRecording::CheckTickNeeded()
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
		return;

	CheckTickNeeded(OwnerCharacter->GetTargetCharacter());
}

void UWRActorComponentChatRecording::CheckLookingTargetHead()
{
	if (WRVoiceContentsManager::Get()->CheckLookingTargetHeadForTranslating() == false)
	{
		if (bActiveGaugeIcon)
			DeActiveGuageIcon();
		else if (bLookingTarget)
			StopLookingTarget();
		else if (bActiveDisabledIcon)
			DeActiveDisabledIcon();

		return;
	}

	// 레이 결과 아이콘을 돌려라
	StartLookingTarget();
}

void UWRActorComponentChatRecording::CheckMyTranslatingPlayerDistance()
{
	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
		return;

	FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(OwnerCharacter->GetCharacterTableID());
	if (PlayerTable == nullptr)
		return;

	const TDoubleLinkedList<AID_t>& MyTranslatingPlayerList = WRVoiceContentsManager::Get()->GetMyTranslatingPlayerList();
	const TDoubleLinkedList<AID_t>::TDoubleLinkedListNode* Node = MyTranslatingPlayerList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		AID_t PlayerAID = Node->GetValue();
		if(PlayerAID == INVALID_AID)
			continue;

		AWRCharacter* PlayerCharacter = WRCharacterManager::Get()->FindbyAID(PlayerAID);
		if(PlayerCharacter == nullptr)
			continue;

		// 화상 채팅 대상자가 번역이 켜지면 거리가 멀어져도 끄지 않는다.
		if (WRVoiceContentsManager::Get()->GetVideoChatController().GetCurrentVideoChatState() == WRVideoChatController::EWRVideoChatState::Connected
			&& PlayerAID == WRVoiceContentsManager::Get()->GetVideoChatController().GetVideoChatPlayerAID())
		{
			continue;
		}
		
		float Dist = WRActorUtility::GetDistance(OwnerCharacter, PlayerCharacter);
		if (Dist > PlayerTable->VoiceChatRange)
			WRVoiceContentsManager::Get()->RequestDeregistSpeaker(PlayerAID);
	}
}

void UWRActorComponentChatRecording::ActiveGuageIcon()
{
	if (bActiveGaugeIcon)
		return;

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
		return;

	AWRCharacter* TargetCharacter = const_cast<AWRCharacter*>(OwnerCharacter->GetTargetCharacter());
	if (TargetCharacter == nullptr)
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		CreatingGuageIconHash.Empty();

		if (InCombinedPanelData.Is3DWidget())
		{
			InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("Speaker")));
			UWRUIPanel_ChatRecordingGaugeIcon* Panel_ChatRecordingGaugeIcon = Cast<UWRUIPanel_ChatRecordingGaugeIcon>(InCombinedPanelData.UIPanel);
			if (Panel_ChatRecordingGaugeIcon)
			{
				Panel_ChatRecordingGaugeIcon->ShowEnabledIcon();
				Panel_ChatRecordingGaugeIcon->StartGauging(3.f, FWRGaugingCompletedDelegate::CreateUObject(this, &UWRActorComponentChatRecording::OnGaugingCompleted));
			}
		}
	});


	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::ChatRecordingGaugeIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter |  EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, TargetCharacter);
	CreatingGuageIconHash = WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

	bActiveGaugeIcon = true;

	//Panel_CrossHair->StartLoading(MAX_PLAYER_SOCIAL_MAX_LOADING_TIME, FWRTargetLoadingCompletedDelegate::CreateUObject(this, &UWRActorComponentPlayerSocial::OnLoadingCompleted));

}

void UWRActorComponentChatRecording::DeActiveGuageIcon()
{
	if (bActiveGaugeIcon == false)
		return;

	bActiveGaugeIcon = false;

	if (CreatingGuageIconHash.IsEmpty() == false)
	{
		WRUIManager::Get()->CancelUIAsync(CreatingGuageIconHash);
		CreatingGuageIconHash.Empty();
		return;
	}

	WRUIManager::Get()->CloseUI(EWRUIPanelKind::ChatRecordingGaugeIcon, true);
}

void UWRActorComponentChatRecording::ActiveDisabledIcon()
{
	if (bActiveDisabledIcon)
		return;

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
		return;

	AWRCharacter* TargetCharacter = const_cast<AWRCharacter*>(OwnerCharacter->GetTargetCharacter());
	if (TargetCharacter == nullptr)
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		CreatingGuageIconHash.Empty();

		if (InCombinedPanelData.Is3DWidget())
		{
			InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("Speaker")));
			UWRUIPanel_ChatRecordingGaugeIcon* Panel_ChatRecordingGaugeIcon = Cast<UWRUIPanel_ChatRecordingGaugeIcon>(InCombinedPanelData.UIPanel);
			if (Panel_ChatRecordingGaugeIcon)
			{
				Panel_ChatRecordingGaugeIcon->ShowDisabledIcon();
			}
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::ChatRecordingGaugeIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, TargetCharacter);
	CreatingGuageIconHash = WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

	/*FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(43);
	if (CustomTextTable)
	{
		WRNoticeManager::Get()->AddNotice(WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey));
	}*/

	bActiveDisabledIcon = true;
}

void UWRActorComponentChatRecording::DeActiveDisabledIcon()
{
	if (bActiveDisabledIcon == false)
		return;

	bActiveDisabledIcon = false;

	if (CreatingGuageIconHash.IsEmpty() == false)
	{
		WRUIManager::Get()->CancelUIAsync(CreatingGuageIconHash);
		CreatingGuageIconHash.Empty();
		return;
	}

	WRUIManager::Get()->CloseUI(EWRUIPanelKind::ChatRecordingGaugeIcon, true);
}

void UWRActorComponentChatRecording::StartLookingTarget()
{
	bLookingTarget = true;
	FlowLookingTargetTime = 0.f;
}

void UWRActorComponentChatRecording::StopLookingTarget()
{
	bLookingTarget = false;
	FlowLookingTargetTime = 0.f;
}

void UWRActorComponentChatRecording::OnLookTargetCompleted()
{
	bLookingTarget = false;

	AWRCharacterMine* OwnerCharacter = Cast<AWRCharacterMine>(GetOwnerCharacter());
	if (OwnerCharacter)
	{
		const AWRCharacterPlayer* MyTargetCharacter = Cast<const AWRCharacterPlayer>(OwnerCharacter->GetTargetCharacter());
		if (MyTargetCharacter && (MyTargetCharacter->GetLanguageType() == OwnerCharacter->GetLanguageType()))
		{
			ActiveDisabledIcon();
			return;
		}
	}

	ActiveGuageIcon();
}