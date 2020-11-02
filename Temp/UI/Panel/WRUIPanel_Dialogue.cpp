// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Dialogue.h"
#include "WRUIPanel_SelectDialogue.h"

#include "Table/WRDialogueTable.h"
#include "Table/WREnpcTable.h"

#include "Manager/WRDialogueManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRSFXManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRAsyncLoadUObjectManager.h"

#include "Component/Character/WRActorComponentLipsync.h"
#include "Component/Character/WRActorComponentStateMark.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Utility/WRUIUtility.h"
#include "Define/WRUIDefine.h"
#include "Network/IMMessageSender.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRUIPanel_Dialogue::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Content);

	NextDialogueType = EWRDialogueType::None;
	NextDialogueTID = INVALID_TABLE_TID;
	PreviousDialogueData = nullptr;
	RemainPlayTime = 0.f;
	bAutoSkip = false;
}

void UWRUIPanel_Dialogue::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bAutoSkip)
	{
		RemainPlayTime -= InDeltaTime;
		if(RemainPlayTime <= 0.f)
			NextDialogue();
	}
}

void UWRUIPanel_Dialogue::OpenDialgoue(const FWRDialogueTable* IN InDialogueTable)
{
	if (InDialogueTable == nullptr)
		return;

	PreviousDialogueData = nullptr;
	SetDialogueData(InDialogueTable);
}

void UWRUIPanel_Dialogue::OnDialogueSetSelected(const EWRDialogueType IN InNextDialogueType, const WRTableID IN InNextDialogueTID)
{
	if (InNextDialogueTID == INVALID_TABLE_TID)
	{
		FinishDialogue();
		return;
	}

	const FWRDialogueTable* DialogueTable = WRDialogueManager::Get()->GetDialogueTable(InNextDialogueType, InNextDialogueTID);
	if (DialogueTable == nullptr)
		FinishDialogue();
	else
		SetDialogueData(DialogueTable);
}

void UWRUIPanel_Dialogue::ForcedFinishDialogue()
{
	FinishDialogue();
}

void UWRUIPanel_Dialogue::SetDialogueData(const FWRDialogueTable* IN InDialogueTable, const bool IN bInCheckAsynLoadData /*= true*/)
{	
	if (InDialogueTable == nullptr)
		return;

	if (bInCheckAsynLoadData && (InDialogueTable->VoiceSound.IsNone() == false || InDialogueTable->DialogueLipsyncCurve.IsNone() == false))
	{
		StartAsyncLoadData(InDialogueTable);
		return;
	}
	else
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	NextDialogueType = InDialogueTable->GetDialogueType();
	NextDialogueTID = InDialogueTable->NextDialogueTID;
	RemainPlayTime = InDialogueTable->DialoguePlayTime / 1000.f;
	bAutoSkip = true;

	FString DialogueLocalKey = InDialogueTable->DialogueLocalkey.ToString();
	FText Content = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::NPCStringTable, DialogueLocalKey);

	if (PreviousDialogueData)
		ClearPreviousCharacterData(InDialogueTable->SpeakNpcTID);

	// InDialogueTable->SpeakNpcTID 가 1이면 플레이어이다.(자기자신)
	if (InDialogueTable->SpeakNpcTID == 1)
	{
		// 아직 미구현
	}
	else
	{
		SetDialogueDataRelatedToCharacter(InDialogueTable);	
	}
	
	ProgressDialogueAction(InDialogueTable);
	PlayVoice(InDialogueTable);
	
	if (InDialogueTable->DialogueLocalkey.IsNone())
	{
		SetRenderOpacity(0.f);
	}
	else
	{
		SetRenderOpacity(1.f);
		WRTextBlock_Content->SetText(Content);
	}

	if (InDialogueTable->DBPosiable)
	{
		IMMessageSender::RequestDBSaveAnswerToNPC(InDialogueTable->GetTableID());
	}

	PreviousDialogueData = InDialogueTable;
}

void UWRUIPanel_Dialogue::SetDialogueDataRelatedToCharacter(const FWRDialogueTable* IN InDialogueTable)
{
	if (InDialogueTable == nullptr)
		return;

	AWRCharacter* SpeakCharacter = WRCharacterManager::Get()->FindByTID(InDialogueTable->SpeakNpcTID);
	if (SpeakCharacter == nullptr)
		return;

	AWRCharacterNPC* NpcCharacter = Cast<AWRCharacterNPC>(SpeakCharacter);
	if (NpcCharacter == nullptr)
		return;

	if (NpcCharacter->GetCharacterType() != EWRCharacterType::NPC)
		return;

	const FWRNpcTable* NpcTable = static_cast<const FWRNpcTable*>(NpcCharacter->GetNpcBaseTable());
	if (NpcTable == nullptr)
		return;

	if (PreviousDialogueData == nullptr || (PreviousDialogueData->SpeakNpcTID != InDialogueTable->SpeakNpcTID))
	{
		IMMessageSender::RequestBeginTalking(SpeakCharacter->GetActorID(), InDialogueTable->RoamingPossible);

		int32 RandIndex = -1;

		if (InDialogueTable->SpeechAniID.Num() > 0)
			RandIndex = FMath::Rand() % InDialogueTable->SpeechAniID.Num();

		if (NpcTable && NpcTable->Rotatable)
		{
			if (RandIndex >= 0)
				NpcCharacter->SetLookTargetForDialog(Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter()), FWRRotationEndDelegate::CreateUObject(this, &UWRUIPanel_Dialogue::PlayTalkAni, SpeakCharacter, InDialogueTable->SpeechAniID[RandIndex]));
			else
				NpcCharacter->SetLookTargetForDialog(Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter()));
		}
		else if (RandIndex >= 0)
		{
			PlayTalkAni(SpeakCharacter, InDialogueTable->SpeechAniID[RandIndex]);
		}
	}

	if (InDialogueTable->PlayerLookIK)
		SpeakCharacter->SetLookIKTarget(Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter()), "head");

	DetermineToCreateIcon(InDialogueTable, SpeakCharacter);
	OpenSelectDialogueUI(InDialogueTable, SpeakCharacter);
}

void UWRUIPanel_Dialogue::NextDialogue()
{
	if (NextDialogueTID != INVALID_TABLE_TID)
	{
		const FWRDialogueTable* DialogueTable = WRDialogueManager::Get()->GetDialogueTable(NextDialogueType, NextDialogueTID);
		if (DialogueTable == nullptr)
			FinishDialogue();
		else
			SetDialogueData(DialogueTable);
		return;
	}
	
	FinishDialogue();
}

void UWRUIPanel_Dialogue::FinishDialogue()
{
	ClearDialogue();
	WRDialogueManager::Get()->OnDialogueFinished();
}

void UWRUIPanel_Dialogue::ClearDialogue()
{
	if (PreviousDialogueData)
		ClearPreviousCharacterData();

	StopVoice();

	NextDialogueType = EWRDialogueType::None;
	NextDialogueTID = INVALID_TABLE_TID;
	PreviousDialogueData = nullptr;
	RemainPlayTime = 0.f;
	bAutoSkip = false;
	VoiceHandle = 0;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_Dialogue::ClearPreviousCharacterData(const WRTableID IN InCurrentCharacterTID /*= INVALID_TABLE_TID*/)
{
	if (PreviousDialogueData == nullptr)
		return;

	if (PreviousDialogueData->SpeakNpcTID == INVALID_TABLE_TID)
		return;

	if (InCurrentCharacterTID != INVALID_TABLE_TID && InCurrentCharacterTID == PreviousDialogueData->SpeakNpcTID)
		return;
	
	AWRCharacter* SpeakCharacter = WRCharacterManager::Get()->FindByTID(PreviousDialogueData->SpeakNpcTID);
	if (SpeakCharacter == nullptr)
		return;

	
	SpeakCharacter->SetLookIKTarget(nullptr);

	if (PreviousDialogueData->SpeakNpcTID != 1)
	{
		UWRActorComponentStateMark* StateMarkComponent = SpeakCharacter->GetComponent<UWRActorComponentStateMark>();
		if (StateMarkComponent)
		{
			StateMarkComponent->RemoveMark(UWRActorComponentStateMark::EWRStateMarkType::Talking);
		}
	}

	IMMessageSender::RequestEndTalking(SpeakCharacter->GetActorID());
	
	if (PreviousDialogueData->PlayerLookIK)
		SpeakCharacter->SetLookIKTarget(nullptr);
}


void UWRUIPanel_Dialogue::PlayVoice(const FWRDialogueTable* IN InDialogueTable)
{
	if (InDialogueTable == nullptr)
		return;

	if (InDialogueTable->VoiceSound.IsNone() && InDialogueTable->DialogueLipsyncCurve.IsNone())
		return;

	if (InDialogueTable->DialogueLipsyncCurve.IsNone())
	{
		if (InDialogueTable->Use3DSound)
		{
			AWRCharacter* SpeakCharacter = WRCharacterManager::Get()->FindByTID(InDialogueTable->SpeakNpcTID);
			if (SpeakCharacter == nullptr)
				return;

			//UAudioComponent* AudioComponent = WRSFXManager::Get()->SpawnAttached(WRUIUtility::ConvertFNameToFString(InDialogueTable->VoiceSound), SpeakCharacter->GetRootComponent());
			UAudioComponent* AudioComponent = WRSFXManager::Get()->SpawnAttached(VoiceSound, SpeakCharacter->GetRootComponent());
			if (AudioComponent)
				VoiceHandle = AudioComponent->GetUniqueID();
		}

		else
		{
			//UAudioComponent* AudioComponent = WRSFXManager::Get()->Spawn2D(WRUIUtility::ConvertFNameToFString(InDialogueTable->VoiceSound));
			UAudioComponent* AudioComponent = WRSFXManager::Get()->Spawn2D(VoiceSound);
			if (AudioComponent)
				VoiceHandle = AudioComponent->GetUniqueID();
		}
	}
	else
	{
		if (InDialogueTable->SpeakNpcTID == INVALID_TABLE_TID)
			return;

		AWRCharacter* SpeakCharacter = WRCharacterManager::Get()->FindByTID(InDialogueTable->SpeakNpcTID);
		if (SpeakCharacter == nullptr)
			return;

		UWRActorComponentLipsync* ActorComponentLipsync = SpeakCharacter->GetComponent<UWRActorComponentLipsync>();
		if (ActorComponentLipsync == nullptr)
			return;

		/*USoundBase* SoundBase = LoadObject<USoundBase>(nullptr, *WRUIUtility::ConvertFNameToFString(InDialogueTable->VoiceSound));
		UCurveFloat* CurveFloat = LoadObject<UCurveFloat>(nullptr, *WRUIUtility::ConvertFNameToFString(InDialogueTable->DialogueLipsyncCurve));*/
		ActorComponentLipsync->Start(VoiceSound, LipsyncCurve);
	}
}

void UWRUIPanel_Dialogue::PlayTalkAni(AWRCharacter* IN InCharacter, const FName IN InAniID)
{
	if (InCharacter == nullptr)
		return;

	WRFSMInfo FSMInfo;
	FSMInfo.FSMType = EWRFSM::Talk;
	FSMInfo.JumpToSectionName = InAniID.ToString();
	InCharacter->DetermineFSMSignal(FSMInfo);
}

void UWRUIPanel_Dialogue::StopVoice()
{
	if (PreviousDialogueData == nullptr)
		return;

	if (PreviousDialogueData->VoiceSound.IsNone() && PreviousDialogueData->DialogueLipsyncCurve.IsNone())
		return;

	if (PreviousDialogueData->DialogueLipsyncCurve.IsNone())
	{
		WRSFXManager::Get()->Remove(VoiceHandle);
		VoiceHandle = 0;
	}
	else
	{
		if (PreviousDialogueData->SpeakNpcTID == INVALID_TABLE_TID)
			return;

		AWRCharacter* SpeakCharacter = WRCharacterManager::Get()->FindByTID(PreviousDialogueData->SpeakNpcTID);
		if (SpeakCharacter == nullptr)
			return;

		UWRActorComponentLipsync* ActorComponentLipsync = SpeakCharacter->GetComponent<UWRActorComponentLipsync>();
		if (ActorComponentLipsync == nullptr)
			return;

		ActorComponentLipsync->Stop();
	}
}

void UWRUIPanel_Dialogue::ProgressDialogueAction(const FWRDialogueTable* IN InDialogueTable)
{
	if (InDialogueTable == nullptr)
		return;

	int32 MaxNum = InDialogueTable->DialogueActions.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		WRDialogueManager::Get()->ProgressDialogueAction(InDialogueTable->DialogueActions[i], InDialogueTable->GetTableID());
	}
}

void UWRUIPanel_Dialogue::DetermineToCreateIcon(const FWRDialogueTable* IN InDialogueTable, AWRCharacter* IN InSpeakCharacter)
{
	bool bCreateIcon = false;
	if (PreviousDialogueData)
	{
		if (PreviousDialogueData->SpeakNpcTID != InDialogueTable->SpeakNpcTID)
		{
			bCreateIcon = true;
		}
	}
	else
	{
		bCreateIcon = true;
	}

	if (bCreateIcon == false)
		return;

	UWRActorComponentStateMark* StateMarkComponent = InSpeakCharacter->GetComponent<UWRActorComponentStateMark>();
	if (StateMarkComponent)
	{
		StateMarkComponent->AddMark(UWRActorComponentStateMark::EWRStateMarkType::Talking);
	}
}

void UWRUIPanel_Dialogue::OpenSelectDialogueUI(const FWRDialogueTable* IN InDialogueTable, AWRCharacter* IN InSpeakCharacter)
{
	if (InDialogueTable == nullptr)
		return;

	if (InSpeakCharacter == nullptr)
		return;

	if (InDialogueTable->NextDialogueSetGroupTID == INVALID_TABLE_TID)
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("DialogueSetGroup")));
			UWRUIPanel_SelectDialogue* Panel_SelectDialogue = Cast<UWRUIPanel_SelectDialogue>(InCombinedPanelData.UIPanel);
			if (Panel_SelectDialogue)
				Panel_SelectDialogue->SetDialogueSetGroupData(InDialogueTable->NextDialogueSetGroupTID);
		}
	});

	if (InSpeakCharacter->GetCharacterTableID() == DEF_AIDA_TABLIE_ID)
	{
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::SelectDialogue, EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::WidgetInteractionable | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
		WidgetInfo.SaveCreatingState = true;
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
	else
	{
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::SelectDialogue, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::WidgetInteractionable, &Delegate, InSpeakCharacter);
		WidgetInfo.Location = FVector::ZeroVector;
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}

	bAutoSkip = false;
}

void UWRUIPanel_Dialogue::StartAsyncLoadData(const FWRDialogueTable* IN InDialogueTable)
{
	if (InDialogueTable == nullptr)
		return;

	SaveDialogueTable = InDialogueTable;
	VoiceSound = nullptr;
	LipsyncCurve = nullptr;

	if (AsyncLoadHashForVoiceSound.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadHashForVoiceSound);
		AsyncLoadHashForVoiceSound.Empty();
	}
	if (AsyncLoadHashForLipsyncCurve.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadHashForLipsyncCurve);
		AsyncLoadHashForLipsyncCurve.Empty();
	}

	if(InDialogueTable->VoiceSound.IsNone() == false)
		AsyncLoadHashForVoiceSound = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(WRUIUtility::ConvertFNameToFString(InDialogueTable->VoiceSound), FWRCompleteAsyncLoad::CreateUObject(this, &UWRUIPanel_Dialogue::OnAsyncLoadCompleted));
	if(InDialogueTable->DialogueLipsyncCurve.IsNone() == false)
		AsyncLoadHashForLipsyncCurve = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(WRUIUtility::ConvertFNameToFString(InDialogueTable->DialogueLipsyncCurve), FWRCompleteAsyncLoad::CreateUObject(this, &UWRUIPanel_Dialogue::OnAsyncLoadCompleted));
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_Dialogue::OnAsyncLoadCompleted(UObject* IN InLoadedAsset, FString IN InHash)
{
	if (InLoadedAsset == nullptr)
		return;

	if (InHash == AsyncLoadHashForLipsyncCurve)
	{
		LipsyncCurve = Cast<UCurveFloat>(InLoadedAsset);
	}
	else if (InHash == AsyncLoadHashForVoiceSound)
	{
		VoiceSound = Cast<USoundBase>(InLoadedAsset);
	}

	if (VoiceSound && (AsyncLoadHashForLipsyncCurve.IsEmpty() || LipsyncCurve != nullptr))
	{
		SetDialogueData(SaveDialogueTable, false);
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}
