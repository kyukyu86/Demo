// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentPlayerSocial.h"

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


#define MAX_PLAYER_SOCIAL_MAX_LOADING_TIME 2

void UWRActorComponentPlayerSocial::OnCreateComponent()
{
	Super::OnCreateComponent();
}

void UWRActorComponentPlayerSocial::OnDestroyComponent()
{
	Super::OnDestroyComponent();
}

void UWRActorComponentPlayerSocial::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckTime += DeltaTime;
	if (CheckTime > 0.5f)
	{
		bool bActiveTemp = IsAvailablePlayerSocial();
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

void UWRActorComponentPlayerSocial::OnLoadingCompleted()
{
	//bLoading = false;
	
	ProgressPlayerSocial();
}

bool UWRActorComponentPlayerSocial::IsAvailablePlayerSocial()
{
	// 플레이어 타입으로 설치된 캐릭터 때문에 임시 처리(제 생각에는 플레이어 타입은 유저라고 생각이 듭니다)
	if (GetOwnerCharacter()->GetCharacterName().IsEqual(""))
		return false;

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

	if (MyCharacter->GetBoardingVehicle() != nullptr)
		return false;

	if (GetOwnerCharacter()->GetBoardingVehicle() != nullptr)
		return false;

	if (MyCharacter->IsDanceModeOn())
		return false;
	
	if (MyCharacter->GetTargetAID() != GetOwnerCharacter()->GetActorID())
		return false;

	if (MyCharacter->IsInputPad())
		return false;

	if (GetOwnerCharacter()->IsVisibilityBits(EWRVisibility::SpecialConversation))
		return false;

	if (MyCharacter->GetChatType() == EWRChatType::Secret)
	{
		if (MyCharacter->GetDialogGroupID() != GetOwnerCharacter()->GetDialogGroupID())
			return false;
	}

	float Dist = WRActorUtility::GetDistance(MyCharacter, GetOwnerCharacter());
	if (Dist > WRConsoleManager::Get()->GetInt(EWRConsole::UI_AutoCloseSocialMenu_Distance))
		return false;

	return true;
}


void UWRActorComponentPlayerSocial::ProgressPlayerSocial()
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
		{
			UWRUIPanel_SocialMenu* Panel_SocialMenu = Cast<UWRUIPanel_SocialMenu>(InCombinedPanelData.UIPanel);
			if (Panel_SocialMenu)
			{
				Panel_SocialMenu->InitSocialMenuInfo(EWRCharacterType::Player);
				Panel_SocialMenu->SetOwnerCharacter(const_cast<AWRCharacter*>(GetOwnerCharacter()));
			}
			InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::SocialMenu, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::WidgetInteractionable | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, GetOwnerCharacter());
	WidgetInfo.SaveCreatingState = true;
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::SocialMenu);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::SocialMenu, GetOwnerCharacter(), FTransform(FRotator::ZeroRotator, vLocation, FVector::OneVector), EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::WidgetInteractionable | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, nullptr, true);
}

void UWRActorComponentPlayerSocial::ActiveIcon()
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
			InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("View_Target")));
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
	
	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::InputIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, GetOwnerCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::InputIcon, GetOwnerCharacter(), FTransform::Identity, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::WidgetInteractionable, &Delegate);

	bActive = true;
}

void UWRActorComponentPlayerSocial::DeActiveIcon()
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
