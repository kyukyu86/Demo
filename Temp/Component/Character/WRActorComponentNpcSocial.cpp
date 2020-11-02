// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentNpcSocial.h"

#include "Actor/Character/WRCharacterMine.h"

#include "UI/Panel/WRUIPanel_CrossHair.h"
#include "Manager/WRCharacterManager.h"
#include "UI/Panel/WRUIPanel_SocialMenu.h"
#include "Manager/WRConsoleManager.h"
#include "UI/Panel/WRUIPanel_InputIcon.h"
#include "Table/WRCustomTextTable.h"
#include "Table/Base/WRTableManager.h"
#include "Utility/WRActorUtility.h"
#include "Utility/WRUIUtility.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRActorComponentNpcSocial::OnCreateComponent()
{
	Super::OnCreateComponent();
}

void UWRActorComponentNpcSocial::OnDestroyComponent()
{
	Super::OnDestroyComponent();
}

void UWRActorComponentNpcSocial::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckTime += DeltaTime;
	if (CheckTime > 0.5f)
	{
		bool bActiveTemp = IsAvailableNpcSocial();
		if (bActiveTemp)
			ActiveIcon();
		else
			DeActiveIcon();
	}

	/*if (bLoading == false)
	{
		if (IsAvailablePlayerSocial() == false)
			return;

		StartLoading();
	}
	else
	{
		if (IsAvailablePlayerSocial() == true)
			return;

		StopLoading();
	}*/
}

void UWRActorComponentNpcSocial::OnLoadingCompleted()
{
	//bLoading = false;
	
	ProgressNpcSocial();
}

bool UWRActorComponentNpcSocial::IsAvailableNpcSocial()
{
	if (WRMapManager::Get()->GetCurrentMapType() == EWRMapType::BattleField)
		return false;

	if (WRMapManager::Get()->GetCurrentMapType() == EWRMapType::Stage)
		return false;

	if (WRUIManager::Get()->IsWidgetCreating(EWRUIPanelKind::SocialMenu))
		return false;
	   
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
		return false;

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SelectDialogue))
		return false;

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
		return false;
	
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return false;

	if (MyCharacter->IsDanceModeOn())
		return false;
	
	if (MyCharacter->GetTargetAID() != GetOwnerCharacter()->GetActorID())
		return false;

	if (MyCharacter->IsInputPad())
		return false;

	float Dist = WRActorUtility::GetDistance(MyCharacter, GetOwnerCharacter());
	if (Dist > WRConsoleManager::Get()->GetInt(EWRConsole::UI_AutoCloseSocialMenu_Distance))
		return false;

	return true;
}


void UWRActorComponentNpcSocial::ProgressNpcSocial()
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
		{
			UWRUIPanel_SocialMenu* Panel_SocialMenu = Cast<UWRUIPanel_SocialMenu>(InCombinedPanelData.UIPanel);
			if (Panel_SocialMenu)
			{
				Panel_SocialMenu->InitSocialMenuInfo(EWRCharacterType::NPC);
				Panel_SocialMenu->SetOwnerCharacter(const_cast<AWRCharacter*>(GetOwnerCharacter()));
			}
				
			InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::SocialMenu, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, GetOwnerCharacter());
	WidgetInfo.SaveCreatingState = true;
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::SocialMenu);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::SocialMenu, GetOwnerCharacter(), FTransform(FRotator::ZeroRotator, vLocation, FVector::OneVector), EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::WidgetInteractionable | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, nullptr, true);
}

void UWRActorComponentNpcSocial::ActiveIcon()
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
				FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(18);
				if (CustomTextTable)
				{
					FString LocalKey = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
					Panel_InputIcon->SetContent(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable,LocalKey));
				}				
			}
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::InputIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCharacter, &Delegate, GetOwnerCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::InputIcon, GetOwnerCharacter(), FTransform::Identity, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCharacter, &Delegate);

	bActive = true;
}

void UWRActorComponentNpcSocial::DeActiveIcon()
{
	if (bActive == false)
		return;

	bActive = false;
	WRUIManager::Get()->CloseUI(EWRUIPanelKind::InputIcon);
}
//
//void UWRActorComponentPlayerSocial::StartLoading()
//{
//	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUI(EWRUIPanelKind::CrossHair);
//	if (CombinedPanelData == nullptr)
//		return;
//
//	UWRUIPanel_CrossHair* Panel_CrossHair = Cast<UWRUIPanel_CrossHair>(CombinedPanelData->UIPanel);
//	if (Panel_CrossHair == nullptr)
//		return;
//
//	Panel_CrossHair->StartLoading(MAX_PLAYER_SOCIAL_MAX_LOADING_TIME, FWRTargetLoadingCompletedDelegate::CreateUObject(this, &UWRActorComponentPlayerSocial::OnLoadingCompleted));
//
//	bLoading = true;	
//}
//
//void UWRActorComponentPlayerSocial::StopLoading()
//{
//	bLoading = false;
//
//	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUI(EWRUIPanelKind::CrossHair);
//	if (CombinedPanelData == nullptr)
//		return;
//
//	UWRUIPanel_CrossHair* Panel_CrossHair = Cast<UWRUIPanel_CrossHair>(CombinedPanelData->UIPanel);
//	if (Panel_CrossHair == nullptr)
//		return;
//
//	Panel_CrossHair->StopLoading();
//}
