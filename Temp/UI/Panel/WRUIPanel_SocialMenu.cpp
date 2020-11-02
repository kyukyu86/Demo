// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_SocialMenu.h"
#include "TextBlock.h"
#include "Button.h"
#include "UI/CustomWidget/WRFocusButton.h"
#include "UI/Slot/WRUISlot_VoiceCommand.h"
#include "UI/Slot/WRUISlot_VoiceCommandText.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRSocialTable.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterMine.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRCharacterManager.h"
#include "Network/IMMessageSender.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "Manager/UWRVoiceController.h"
#include "Table/WRVoiceRecognitionTable.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRCheersManager.h"
#include "Manager/WRPopupManager.h"
#include "Utility/WRActorUtility.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Manager/WRVoiceContentsManager.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "Manager/WRSFXManager.h"

#include "Localization/WRLocalizationProcessor.h"


#define DEF_MAX_SOCIAL_MENU_NUM 8
#define DEF_SOCIAL_MENU_ROW_NUM 4
#define SOCIAL_MENU_ASSET_PATH TEXT("/Game/Asset/Data/Asset/Widget/SocialMenu.SocialMenu")

void UWRUIPanel_SocialMenu::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Name);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Title1);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Title2);
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommand, Slot_VoiceCommand, Slot_VoiceCommand->SetVoiceWaitUIType(UWRUISlot_VoiceCommand::EWRVoiceWaitUIType::Visible);
																				 Slot_VoiceCommand->SetVoiceContentType(EWRVoiceContentType::SocialMenu););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommandText, Slot_VoiceCommandText, Slot_VoiceCommandText->SetJustification(ETextJustify::Center); 
																						Slot_VoiceCommandText->SetVoiceWaitUIType(UWRUISlot_VoiceCommandText::EWRVoiceWaitUIType::Visible););
	SocialAnimationTIDs.Reserve(DEF_MAX_SOCIAL_MENU_NUM);

	OwnerCharacter = nullptr;
	for (int32 i = 0; i < DEF_MAX_SOCIAL_MENU_NUM; i++)
	{
		FString ButtonName = FString::Printf(TEXT("FocusButton_Menu%d"), i);
		UWRFocusButton* FocusButton = Cast<UWRFocusButton>(GetWidgetFromName(*ButtonName));
		if (FocusButton)
		{
			FocusButton->SetTickFocusCheck(false);
			FocusButton->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_SocialMenu::OnClickedMenuButton);
			FocusButtonMenus.Add((FocusButton));
		}
	}

	if (WRInputSystemManager::Get()->IsVoiceRecording() || WRInputSystemManager::Get()->IsChatRecording())
		WRInputSystemManager::Get()->CancelRecording();

	// + UI Exception
	if (WRCheersManager::Get()->PauseCheersContents())
	{
		// [ 2020-4-20 : kyu ] 건배상태 체크 및 해제
	}

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn() == false && DanceModeComponent->IsEnterDanceArea())
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::DanceMode);
	}

	WRPopupManager::Get()->ClosePopup();

	SetVisibility(ESlateVisibility::Collapsed);

	FWRSocialMenuSettingTable* SocialMenuSettingTable = WRTableManager::Get()->FindRow<FWRSocialMenuSettingTable>(1);
	if (SocialMenuSettingTable)
	{
		WRSFXManager::Get()->Spawn2D(WRUIUtility::ConvertFNameToFString(SocialMenuSettingTable->OpenUISoundPath));
	}
}

void UWRUIPanel_SocialMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	CheckTime += InDeltaTime;
	if (CheckTime > 0.5f)
	{
		CheckAutoCloseSocialMenu();
		CheckTime = 0.f;
	}
}

void UWRUIPanel_SocialMenu::NativeDestruct()
{
	FWRSocialMenuSettingTable* SocialMenuSettingTable = WRTableManager::Get()->FindRow<FWRSocialMenuSettingTable>(1);
	if (SocialMenuSettingTable)
	{
		WRSFXManager::Get()->Spawn2D(WRUIUtility::ConvertFNameToFString(SocialMenuSettingTable->CloseUISoundPath));
	}

	if (WRInputSystemManager::Get()->IsVoiceRecording())
		WRInputSystemManager::Get()->CancelRecording();

	// + UI Exception
	if (WRCheersManager::Get()->AttemptToOpenCheersContents())
	{
		// [ 2020-4-20 : kyu ] 건배상태 체크 및 설정
	}

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn() == false && DanceModeComponent->IsEnterDanceArea())
			DanceModeComponent->ShowStartDanceUI();
	}

	Super::NativeDestruct();
}

FReply UWRUIPanel_SocialMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	int32 nIndex = 0;
	if (CloseKey.Find(InKeyEvent.GetKey(), nIndex))
	{
		WRUIManager::Get()->CloseUI(OwnerCharacter ,EWRUIPanelKind::SocialMenu);
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UWRUIPanel_SocialMenu::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == DisappearAnimation)
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::SocialMenu);
	}
}

void UWRUIPanel_SocialMenu::SetOwnerCharacter(AWRCharacter* IN InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;

	if (OwnerCharacter == nullptr)
		return;
	
	TextBlock_Name->SetText(WRUIUtility::ConvertFNameToFText(OwnerCharacter->GetCharacterName()));
}

void UWRUIPanel_SocialMenu::StartVoiceRecognition()
{
	if (WRInputSystemManager::Get()->IsImpossibleVoiceRecording())
		return;

	TArray<FWRSubCommandData> SubCommandDatas;
	int32 MaxNum = SocialAnimationTIDs.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		FWRSocialAnimationTable* SocialAnimationTable = WRTableManager::Get()->FindRow<FWRSocialAnimationTable>(SocialAnimationTIDs[i]);
		if(SocialAnimationTable == nullptr)
			continue;

		if (SocialAnimationTable->VoiceSubCommandTID == INVALID_TABLE_TID)
			continue;

		if (CurrentCharacterType != SocialAnimationTable->TargetCharacterType)
			continue;

		FWRVoiceSubCommandTable* VoiceSubCommandTable = WRTableManager::Get()->FindRow<FWRVoiceSubCommandTable>(SocialAnimationTable->VoiceSubCommandTID);
		if (VoiceSubCommandTable == nullptr)
			continue;

		FWRSubCommandData TempData;
		TempData.Tid = SocialAnimationTable->VoiceSubCommandTID;
		TempData.NounWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->NounWord);
		TempData.VerbWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->VerbWord);
		SubCommandDatas.Add(TempData);
	}
	if (SubCommandDatas.Num() == 0)
		return;

	WRInputSystemManager::Get()->StartRecording(EWRVoiceContentType::SocialMenu, SubCommandDatas, FWRVoiceEventDelegate::CreateUObject(this, &UWRUIPanel_SocialMenu::OnResultVoiceRecognitionReceived));
}

void UWRUIPanel_SocialMenu::OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID)
{
	// [SeungYeop 2020/07/22] : 청자 기준 번역이 들어가면서 음성명령 종료시 번역을 켜줄건지 판단해야한다.
	WRVoiceContentsManager::Get()->StartChatRecording();

	if (IsFadingOut())
		return;

	WRTableID VoiceSubCommandTID = WRConsoleManager::Get()->GetInt(EWRConsole::Cheat_VoiceTID);
	if (VoiceSubCommandTID == 0)
	{
		VoiceSubCommandTID = InVoiceSubCommandTID;
	}

	if (VoiceSubCommandTID == INVALID_TABLE_TID)
		return;

	FWRSocialAnimationTable* SocialAnimationTable = nullptr;
	int32 MaxNum = SocialAnimationTIDs.Num();
	for (int32 i = 0; i < MaxNum; ++i)
	{
		FWRSocialAnimationTable* SocialAnimationTableTemp = WRTableManager::Get()->FindRow<FWRSocialAnimationTable>(SocialAnimationTIDs[i]);
		if (SocialAnimationTableTemp == nullptr)
			continue;

		if (SocialAnimationTableTemp->VoiceSubCommandTID != VoiceSubCommandTID)
			continue;
		
		SocialAnimationTable = SocialAnimationTableTemp;
		break;
	}

	if (SocialAnimationTable == nullptr)
		return;

	if (SocialAnimationTable->SocialType == EWRSocialActionType::Animation)
	{
		if(OwnerCharacter != nullptr)
			IMMessageSender::RequestSocialAction(SocialAnimationTable->GetTableID(), OwnerCharacter->GetActorID());
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::Dance)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		// 이동 중이면 댄스 안되게 처리
		if (MyCharacter && MyCharacter->IsInputPad() == false)
		{		
			IMMessageSender::RequestBeginDance(SocialAnimationTable->DanceGroupTID);
		}
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::SecretDialog)
	{
		if (OwnerCharacter != nullptr)
			IMMessageSender::RequestAgreeToSecretDialog(OwnerCharacter->GetActorID());
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::Translation)
	{
		if (OwnerCharacter != nullptr)
			WRVoiceContentsManager::Get()->RequestRegistSpeaker(OwnerCharacter->GetActorID());
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::FocusTalk)
	{
		if (OwnerCharacter != nullptr)
			IMMessageSender::RequestSetFocusedAID(OwnerCharacter->GetActorID());
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::VideoChatList)
	{
		WRVoiceContentsManager::Get()->GetVideoChatController().OpenUI_VideoChatUserList();
	}
	WRUIManager::Get()->CloseUI(UIHandle);
}

bool UWRUIPanel_SocialMenu::OnInputEventReceived(const EWRInput IN InInputType)
{
	if (InInputType == EWRInput::Circle)
	{
		OnClickedMenuButton();
		return true;
	}
	else if (InInputType == EWRInput::DpadUP
			|| InInputType == EWRInput::DpadDown
			|| InInputType == EWRInput::DpadLeft
			|| InInputType == EWRInput::DpadRight)
	{
		DetermineFocusButton(InInputType);
		return true;
	}
	
	return false;
}

void UWRUIPanel_SocialMenu::InitSocialMenuInfo(const EWRCharacterType IN InCharacterType)
{
	CurrentCharacterType = InCharacterType;
	SocialAnimationTIDs.Empty(DEF_MAX_SOCIAL_MENU_NUM);

	UDataTable* SocialAnimationTables_Internal = WRTableManager::Get()->GetTable<FWRSocialAnimationTable>();
	const TMap<FName, uint8*>& SocialAnimationTableMap = SocialAnimationTables_Internal->GetRowMap();
	int32 Count = 0;
	for (auto& Data : SocialAnimationTableMap)
	{
		if (Count >= DEF_MAX_SOCIAL_MENU_NUM)
			break;

		FWRSocialAnimationTable* SocialAnimationTable = reinterpret_cast<FWRSocialAnimationTable*>(Data.Value);
		if (SocialAnimationTable == nullptr)
			continue;

		if (SocialAnimationTable->TargetCharacterType != InCharacterType)
			continue;
		
		FString NameKey = WRUIUtility::ConvertFNameToFString(SocialAnimationTable->Name);
		FocusButtonMenus[Count]->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, NameKey).ToString());

		FocusButtonMenus[Count]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SocialAnimationTIDs.Add(WRUIUtility::ConvertFNameToInt(Data.Key));

		/*if (SocialAnimationTable->IconPath.IsNone() == false)
		{
			UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *WRUIUtility::ConvertFNameToFString(SocialAnimationTable->IconPath));
			FocusButtonMenus[Count]->SetFocusUIType(EWRFocusUITypeInFocusButton::Add);
			FocusButtonMenus[Count]->SetNotFocusImg(Texture);
			FocusButtonMenus[Count]->SetSelectImg(Texture);
		}*/
		
		++Count;
	}

	UsingFocusButtonNum = Count;
	if (Count > 0)
	{
		CurrentFocusButtonIndex = 0;
		FocusButtonMenus[CurrentFocusButtonIndex]->OnHoveredButton();
	}

	for (int32 i = Count; i < DEF_MAX_SOCIAL_MENU_NUM; ++i)
	{
		FocusButtonMenus[i]->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// [SeungYeop 20/08/20] : 8월 빌드 임시용으로 타이틀은 하드코딩함
	if (InCharacterType == EWRCharacterType::Player)
	{
		WRTextBlock_Title1->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_BabelKey"));
		WRTextBlock_Title2->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_EmotionKey"));
		WRTextBlock_Title2->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (InCharacterType == EWRCharacterType::NPC)
	{
		WRTextBlock_Title1->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_EmotionKey"));
		WRTextBlock_Title2->SetVisibility(ESlateVisibility::Collapsed);
	}

	StartVoiceRecognition();
}

void UWRUIPanel_SocialMenu::CheckAutoCloseSocialMenu()
{
	if (OwnerCharacter == nullptr)
		return;

	if (OwnerCharacter->IsMe())
		return;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	float Dist =  WRActorUtility::GetDistance(MyCharacter, OwnerCharacter);
	if (Dist > WRConsoleManager::Get()->GetInt(EWRConsole::UI_AutoCloseSocialMenu_Distance))
	{
		WRUIManager::Get()->CloseUI(UIHandle);
		return;
	}

	FVector DirVec = OwnerCharacter->GetActorLocation() - MyCharacter->GetActorLocation();
	float Degree = WRActorUtility::GetDegreesBetween2Vectors(MyCharacter->GetActorForwardVector(), DirVec);
	Degree = FMath::Abs(Degree);
	if (Degree > WRConsoleManager::Get()->GetFloat(EWRConsole::UI_AutoCloseSocialMenu_Degree))
	{
		WRUIManager::Get()->CloseUI(UIHandle);
		return;
	}
}

void UWRUIPanel_SocialMenu::DetermineFocusButton(const EWRInput IN InInputType)
{
	int32 PreviousFocusButtonIndex = CurrentFocusButtonIndex;
	if (InInputType == EWRInput::DpadUP)
	{
		if (CurrentFocusButtonIndex % DEF_SOCIAL_MENU_ROW_NUM == 0)
		{
			CurrentFocusButtonIndex += DEF_SOCIAL_MENU_ROW_NUM - 1;
			if (CurrentFocusButtonIndex >= UsingFocusButtonNum)
				CurrentFocusButtonIndex = UsingFocusButtonNum - 1;
		}
		else
		{
			--CurrentFocusButtonIndex;
		}
	}
	else if (InInputType == EWRInput::DpadDown)
	{
		if (CurrentFocusButtonIndex % DEF_SOCIAL_MENU_ROW_NUM == DEF_SOCIAL_MENU_ROW_NUM - 1)
		{
			CurrentFocusButtonIndex -= DEF_SOCIAL_MENU_ROW_NUM - 1;
		}
		else 
		{
			++CurrentFocusButtonIndex;
		}
		if (CurrentFocusButtonIndex >= UsingFocusButtonNum)
		{
			CurrentFocusButtonIndex -= CurrentFocusButtonIndex % DEF_SOCIAL_MENU_ROW_NUM;
		}
	}
	else if (InInputType == EWRInput::DpadLeft)
	{
		CurrentFocusButtonIndex -= DEF_SOCIAL_MENU_ROW_NUM;
		if (CurrentFocusButtonIndex < 0)
		{
			CurrentFocusButtonIndex += DEF_SOCIAL_MENU_ROW_NUM;
			CurrentFocusButtonIndex = int32((UsingFocusButtonNum - 1) / DEF_SOCIAL_MENU_ROW_NUM) * DEF_SOCIAL_MENU_ROW_NUM + (CurrentFocusButtonIndex % DEF_SOCIAL_MENU_ROW_NUM);
		}
	}
	else if (InInputType == EWRInput::DpadRight)
	{
		CurrentFocusButtonIndex += DEF_SOCIAL_MENU_ROW_NUM;
		if (CurrentFocusButtonIndex >= UsingFocusButtonNum)
		{
			CurrentFocusButtonIndex = CurrentFocusButtonIndex % DEF_SOCIAL_MENU_ROW_NUM;
		}
	}

	if (PreviousFocusButtonIndex == CurrentFocusButtonIndex)
		return;

	FocusButtonMenus[PreviousFocusButtonIndex]->OnUnHoveredButton();
	FocusButtonMenus[CurrentFocusButtonIndex]->OnHoveredButton();

	FWRSocialMenuSettingTable* SocialMenuSettingTable = WRTableManager::Get()->FindRow<FWRSocialMenuSettingTable>(1);
	if (SocialMenuSettingTable)
	{
		WRSFXManager::Get()->Spawn2D(WRUIUtility::ConvertFNameToFString(SocialMenuSettingTable->ChangeMenuSoundPath));
	}
}

void UWRUIPanel_SocialMenu::OnClickedMenuButton()
{
	if (IsFadingOut())
		return;	

	int32 Index = -1;
	Index = CurrentFocusButtonIndex;
	/*for (int32 i = 0; i < DEF_MAX_SOCIAL_MENU_NUM; ++i)
	{
		if (FocusButtonMenus[i]->GetButton()->HasAnyUserFocus() == false)
			continue;

		Index = i;
		break;
	}*/
	
	if (Index == -1)
		return;

	if (Index >= SocialAnimationTIDs.Num())
		return;

	FWRSocialAnimationTable* SocialAnimationTable = WRTableManager::Get()->FindRow<FWRSocialAnimationTable>(SocialAnimationTIDs[Index]);
	if (SocialAnimationTable == nullptr)
		return;

	if (SocialAnimationTable->SocialType == EWRSocialActionType::Animation)
	{
		if (OwnerCharacter != nullptr)
			IMMessageSender::RequestSocialAction(SocialAnimationTIDs[Index], OwnerCharacter->GetActorID());
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::Dance)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		// 이동 중이면 댄스 안되게 처리
		if (MyCharacter && MyCharacter->IsInputPad() == false)
		{
			IMMessageSender::RequestBeginDance(SocialAnimationTable->DanceGroupTID);
		}
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::SecretDialog)
	{
		if (OwnerCharacter != nullptr)
			IMMessageSender::RequestAgreeToSecretDialog(OwnerCharacter->GetActorID());
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::Translation)
	{
		if (OwnerCharacter != nullptr)
			WRVoiceContentsManager::Get()->RequestRegistSpeaker(OwnerCharacter->GetActorID());
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::FocusTalk)
	{
		if (OwnerCharacter != nullptr)
			IMMessageSender::RequestSetFocusedAID(OwnerCharacter->GetActorID());
	}
	else if (SocialAnimationTable->SocialType == EWRSocialActionType::VideoChatList)
	{
		WRVoiceContentsManager::Get()->GetVideoChatController().OpenUI_VideoChatUserList();
	}

	WRUIManager::Get()->CloseUI(UIHandle);
}
