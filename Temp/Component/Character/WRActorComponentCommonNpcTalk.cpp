// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentCommonNpcTalk.h"

#include "Actor/Character/WRCharacterNPC.h"
#include "Actor/Character/WRCharacterMine.h"

#include "Table/WREnpcTable.h"
#include "UI/Panel/WRUIPanel_CrossHair.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRDialogueManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRConsoleManager.h"

#include "WRActorComponentStateMark.h"
#include "UI/Panel/WRUIPanel_InputIcon.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRCustomTextTable.h"
#include "Utility/WRActorUtility.h"
#include "Utility/WRUIUtility.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRActorComponentCommonNpcTalk::OnCreateComponent()
{
	Super::OnCreateComponent();

	MyCharacterStartMoveDelegateHandle = WRCharacterManager::Get()->AddMyCharacterStartMoveDelegate(FSimpleDelegate::CreateUObject(this, &UWRActorComponentCommonNpcTalk::OnMyCharacterMoveStarted));
	MyCharacterStopMoveDelegateHandle = WRCharacterManager::Get()->AddMyCharacterStopMoveDelegate(FSimpleDelegate::CreateUObject(this, &UWRActorComponentCommonNpcTalk::OnMyCharacterMoveStoped));
	MyCharacterTargetChangedDelegateHandle = WRCharacterManager::Get()->AddMyCharacterTargetChangedDelegate(FSimpleDelegate::CreateUObject(this, &UWRActorComponentCommonNpcTalk::OnMyCharacterTargetChanged));

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter && MyCharacter->IsInputPad())
	{
		SetEnableTick(true);
	}
}

void UWRActorComponentCommonNpcTalk::OnDestroyComponent()
{
	if (WRCharacterManager::Get())
	{
		WRCharacterManager::Get()->RemoveMyCharacterStartMoveDelegate(MyCharacterStartMoveDelegateHandle);
		WRCharacterManager::Get()->RemoveMyCharacterStopMoveDelegate(MyCharacterStopMoveDelegateHandle);
		WRCharacterManager::Get()->RemoveMyCharacterTargetChangedDelegate(MyCharacterTargetChangedDelegateHandle);
	}

	Super::OnDestroyComponent();
}

void UWRActorComponentCommonNpcTalk::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckTime += DeltaTime;
	if (CheckTime > 0.5f)
	{
		//UpdateLoading();
		//UpdateIcon();
	}	
}

void UWRActorComponentCommonNpcTalk::OnLoadingCompleted()
{
	bLoading = false;
	
	ProgressCommonNpcTalk();
}

void UWRActorComponentCommonNpcTalk::OnMyCharacterMoveStarted()
{
	//UpdateLoading();
	UpdateIcon();
	SetEnableTick(true);
}

void UWRActorComponentCommonNpcTalk::OnMyCharacterMoveStoped()
{
	//UpdateLoading();
	UpdateIcon();
	SetEnableTick(false);
}

void UWRActorComponentCommonNpcTalk::OnDialogueOpend()
{
	//UpdateLoading();
	UpdateIcon();
}

void UWRActorComponentCommonNpcTalk::OnDialogueFinished()
{
	//UpdateLoading();
	UpdateIcon();
}

void UWRActorComponentCommonNpcTalk::OnMyCharacterTargetChanged()
{
	//UpdateLoading();
	UpdateIcon();
}

void UWRActorComponentCommonNpcTalk::UpdateLoading()
{
	IsAvailableCommonNpcTalk();
	/*if (bLoading == false)
	{
		if (IsAvailableCommonNpcTalk() == false)
			return;

		StartLoading();
	}
	else
	{
		if (IsAvailableCommonNpcTalk() == true)
			return;

		StopLoading();
	}*/
}

void UWRActorComponentCommonNpcTalk::UpdateIcon()
{
	bool bActiveTemp = IsAvailableCommonNpcTalk();
	if (bActiveTemp)
		ActiveIcon();
	else
		DeActiveIcon();
}

bool UWRActorComponentCommonNpcTalk::IsAvailableCommonNpcTalk()
{
	EWRDialogueType OpenedDialogueType = WRDialogueManager::Get()->GetOpenedDialogueType();
	if (OpenedDialogueType == EWRDialogueType::NpcTalk || OpenedDialogueType == EWRDialogueType::MainQuest)
		return false;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return false;

	if (MyCharacter->IsDanceModeOn())
		return false;

	AWRCharacterNPC* NpcCharacter = Cast<AWRCharacterNPC>(GetOwnerCharacter());
	if (NpcCharacter == nullptr)
		return false;

	if (MyCharacter->GetTargetAID() != GetOwnerCharacter()->GetActorID())
	{
		UWRActorComponentStateMark* StateMarkComponent = NpcCharacter->GetComponent<UWRActorComponentStateMark>();
		if (StateMarkComponent && StateMarkComponent->ContainsMark(UWRActorComponentStateMark::EWRStateMarkType::AvailableTalk))
		{
			StateMarkComponent->RemoveMark(UWRActorComponentStateMark::EWRStateMarkType::AvailableTalk);
		}
		return false;
	}	

	if (CheckRange() == false)
		return false;

	return true;
}

void UWRActorComponentCommonNpcTalk::ProgressCommonNpcTalk()
{
	AWRCharacterNPC* NpcCharacter = Cast<AWRCharacterNPC>(GetOwnerCharacter());
	if (NpcCharacter == nullptr)
		return;

	if (NpcCharacter->GetCharacterType() != EWRCharacterType::NPC)
		return;

	const FWRNpcTable* NpcTable = static_cast<const FWRNpcTable*>(NpcCharacter->GetNpcBaseTable());
	if (NpcTable == nullptr)
		return;

	WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::NpcTalk, NpcTable->NpcTalkDialogueTID);
}

bool UWRActorComponentCommonNpcTalk::CheckRange()
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return false;

	AWRCharacterNPC* NpcCharacter = Cast<AWRCharacterNPC>(GetOwnerCharacter());
	if (NpcCharacter == nullptr)
		return false;

	if (NpcCharacter->GetCharacterType() != EWRCharacterType::NPC)
		return false;

	const FWRNpcTable* NpcTable = static_cast<const FWRNpcTable*>(NpcCharacter->GetNpcBaseTable());
	if (NpcTable == nullptr)
		return false;

	float Dist = WRActorUtility::GetDistance(MyCharacter, NpcCharacter);
	if (NpcTable->NpcTalkViewRange > Dist)
	{
		UWRActorComponentStateMark* StateMarkComponent = NpcCharacter->GetComponent<UWRActorComponentStateMark>();
		if (StateMarkComponent)
		{
			StateMarkComponent->AddMark(UWRActorComponentStateMark::EWRStateMarkType::AvailableTalk);
		}
	}
	else
	{
		UWRActorComponentStateMark* StateMarkComponent = NpcCharacter->GetComponent<UWRActorComponentStateMark>();
		if (StateMarkComponent && StateMarkComponent->ContainsMark(UWRActorComponentStateMark::EWRStateMarkType::AvailableTalk))
		{
			StateMarkComponent->RemoveMark(UWRActorComponentStateMark::EWRStateMarkType::AvailableTalk);
		}
	}

	if (NpcTable->NpcTalkRange < Dist)
		return false;

	return true;
}

void UWRActorComponentCommonNpcTalk::StartLoading()
{
	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::CrossHair);
	if (CombinedPanelData == nullptr)
		return;

	UWRUIPanel_CrossHair* Panel_CrossHair = Cast<UWRUIPanel_CrossHair>(CombinedPanelData->UIPanel);
	if (Panel_CrossHair == nullptr)
		return;

	float LoadingTime = WRConsoleManager::Get()->GetFloat(EWRConsole::CommonNPCTalk_LoadingTime);
	Panel_CrossHair->StartLoading(LoadingTime, FWRTargetLoadingCompletedDelegate::CreateUObject(this, &UWRActorComponentCommonNpcTalk::OnLoadingCompleted));

	bLoading = true;	
}

void UWRActorComponentCommonNpcTalk::StopLoading()
{
	bLoading = false;

	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::CrossHair);
	if (CombinedPanelData == nullptr)
		return;

	UWRUIPanel_CrossHair* Panel_CrossHair = Cast<UWRUIPanel_CrossHair>(CombinedPanelData->UIPanel);
	if (Panel_CrossHair == nullptr)
		return;

	Panel_CrossHair->StopLoading();
}

void UWRActorComponentCommonNpcTalk::ActiveIcon()
{
	if (bActive)
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (bActive == false)
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::InputIcon);
			return;
		}

		if (InCombinedPanelData.Is3DWidget())
		{
			//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
			//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::CommonUI_FixedSize) * 0.00015f);
			InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("DialogueSetGroup")));
			UWRUIPanel_InputIcon* Panel_InputIcon = Cast<UWRUIPanel_InputIcon>(InCombinedPanelData.UIPanel);
			if (Panel_InputIcon)
			{
				Panel_InputIcon->SetInputKey(EWRInput::Triangle);
				FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(17);
				if (CustomTextTable)
				{
					FString LocalKey = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
					Panel_InputIcon->SetContent(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey));
				}
			}
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::InputIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCharacter, &Delegate, GetOwnerCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::InputIcon, GetOwnerCharacter(), FTransform::Identity, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCharacter, &Delegate);

	bActive = true;
}

void UWRActorComponentCommonNpcTalk::DeActiveIcon()
{
	if (bActive == false)
		return;

	bActive = false;
	WRUIManager::Get()->CloseUI(EWRUIPanelKind::InputIcon);
}
