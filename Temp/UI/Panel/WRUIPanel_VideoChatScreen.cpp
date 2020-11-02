// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_VideoChatScreen.h"

#include "Manager/WRVoiceContentsManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRSFXManager.h"

#include "UI/Slot/WRUISlot_Preview.h"
#include "UI/Slot/WRUISlot_VoiceCommand.h"
#include "UI/Slot/WRUISlot_VoiceCommandText.h"
#include "UI/Slot/WRUISlot_VideoChatBubble.h"

#include "Table/WRCharacterPresetTable.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRVoiceRecognitionTable.h"
#include "Table/WRPlayerTable.h"

#include "Actor/Character/WRCharacter.h"
#include "Utility/WRUIUtility.h"
#include "Components/AudioComponent.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRActorSpawnUtility.h"
#include "Actor/StaticMeshActor/WRStaticMeshActor.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "Utility/WRActorUtility.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/PointLight.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRUIPanel_VideoChatScreen::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UCanvasPanel, CanvasPanel_Cyber);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_NoCyber);

	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommand, Slot_VoiceCommand, Slot_VoiceCommand->SetVoiceWaitUIType(UWRUISlot_VoiceCommand::EWRVoiceWaitUIType::Visible);
																				Slot_VoiceCommand->SetVoiceContentType(EWRVoiceContentType::SocialMenu););
	/*BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_VoiceCommandText, Slot_VoiceCommandText, Slot_VoiceCommandText->SetJustification(ETextJustify::Center);
																							Slot_VoiceCommandText->SetVoiceWaitUIType(UWRUISlot_VoiceCommandText::EWRVoiceWaitUIType::Visible););*/

	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_Preview, Slot_Preview, Slot_Preview->SetVisibility(ESlateVisibility::Collapsed););
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Msg);
	BIND_WIDGET(UWRTextBlock, WRTextBlock_PlayerName);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Calling);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Reject);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_CallingIcon);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_RejectIcon);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_RejectArrow);
	BIND_WIDGET_WITH_IMPLEMENTATION(UCanvasPanel, CanvasPanel_Translation, CanvasPanel_Translation->SetVisibility(ESlateVisibility::Collapsed););
	BIND_WIDGET(UWRUISlot_VideoChatBubble, Slot_VideoChatBubble);
	BIND_WIDGET(UImage, Image_TranslatingLoop);
	BIND_WIDGET_WITH_IMPLEMENTATION(UImage, Image_SoundWave, Image_SoundWave->SetVisibility(ESlateVisibility::Collapsed););
	if (Image_SoundWave)
	{
		Mat_SoundWave = WRUIUtility::GetDynamicMaterial(Image_SoundWave);
		if (Mat_SoundWave)
		{
			Mat_SoundWave->SetScalarParameterValue("CircleMain", 0);
		}
	}

	PlayAnimationByName("OpenAnimation");

	if (WRInputSystemManager::Get()->IsVoiceRecording())
		WRInputSystemManager::Get()->CancelRecording();

	if (bCyberUI)
	{
		CanvasPanel_Cyber->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CanvasPanel_NoCyber->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		CanvasPanel_Cyber->SetVisibility(ESlateVisibility::Hidden);
		CanvasPanel_NoCyber->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWRUIPanel_VideoChatScreen::NativeDestruct()
{
	if (ReceivingLoopSoundIndex != INDEX_NONE)
	{
		WRSFXManager::Get()->Remove(ReceivingLoopSoundIndex);
		ReceivingLoopSoundIndex = INDEX_NONE;
	}

	if (WRInputSystemManager::Get()->IsVoiceRecording())
		WRInputSystemManager::Get()->CancelRecording();

	if (AsyncLoadKeyForPreviewFloor.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadKeyForPreviewFloor);
		AsyncLoadKeyForPreviewFloor.Empty();
	}

	/*if (PreviewFloorActor != nullptr)
	{
		PreviewFloorActor->RemoveFromRoot();
		PreviewFloorActor->Destroy();
		PreviewFloorActor = nullptr;
	}*/

	WRPreviewManager::Get()->ReleasePreview(Slot_Preview, true);

	Super::NativeDestruct();
}

void UWRUIPanel_VideoChatScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	/*if (PreviewFloorTIme > 0)
	{
		PreviewFloorTIme -= InDeltaTime;
		if (PreviewFloorTIme <= 0)
		{
			StopPreviewFloorProdution();
		}
	}*/

	if (SoundWaveTime > 0)
	{
		SoundWaveTime -= InDeltaTime;
		if (SoundWaveTime <= 0)
		{
			StopSoundWave();
		}
	}
}

void UWRUIPanel_VideoChatScreen::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation->MovieScene->GetFName() == FName("CloseAnimation"))
	{
		// 닫힘 연출 후 끔
		WRUIManager::Get()->CloseUI(UIHandle, true);
	}
	else if (Animation->MovieScene->GetFName() == FName("OpenAnimation"))
	{
		PlayAnimationByName("FeedbackAnimation", 0.f, 0);
		if (bCyberUI)
			StartVoiceRecognition();
	}
	else if (Animation->MovieScene->GetFName() == FName("CallingAnimation"))
	{
		if(bCyberUI)
			WRVoiceContentsManager::Get()->GetVideoChatController().OnCallingEventReceivedFromUI(PlayerAID);
	}
	else if (Animation->MovieScene->GetFName() == FName("RejectAnimation"))
	{
		if (bCyberUI)
			WRVoiceContentsManager::Get()->GetVideoChatController().OnRejectEventReceivedFromUI(PlayerAID);
	}
	else if (Animation->MovieScene->GetFName() == FName("OpenConnectedAnimation"))
	{
		ShowPreviewCharacter();
	}
	else if (Animation->MovieScene->GetFName() == FName("CloseConnectedAnimation"))
	{
		PlayAnimationByName("CloseAnimation");
	}
}

void UWRUIPanel_VideoChatScreen::ShowCallingUI(const FWRVideoChatUserData IN InVideoChatUserData)
{
	SetVideoChatUserData(InVideoChatUserData);
	SpawnPreviewCharacter(InVideoChatUserData);
	CanvasPanel_Calling->SetVisibility(ESlateVisibility::Collapsed);
	CanvasPanel_Reject->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CanvasPanel_RejectArrow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Slot_VoiceCommand->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CanvasPanel_Translation->SetVisibility(ESlateVisibility::Collapsed);
	WRTextBlock_Msg->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_VidieoChatCallingKey"));

	if (bCyberUI)
		WRSFXManager::Get()->Spawn2D(TEXT("VideoChatCallstart.VideoChatCallstart"));

	CurrentUIType = EWRUIType::Calling;
}

void UWRUIPanel_VideoChatScreen::ShowReceivingUI(const FWRVideoChatUserData IN InVideoChatUserData)
{
	SetVideoChatUserData(InVideoChatUserData);
	SpawnPreviewCharacter(InVideoChatUserData);
	CanvasPanel_Calling->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CanvasPanel_Reject->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CanvasPanel_RejectArrow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Slot_VoiceCommand->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CanvasPanel_Translation->SetVisibility(ESlateVisibility::Collapsed);
	WRTextBlock_Msg->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_VideoChatReceivingKey"));

	if (bCyberUI)
	{
		UAudioComponent* AudioComponent = WRSFXManager::Get()->Spawn2D(TEXT("VideoChatReceiving.VideoChatReceiving"));
		if (AudioComponent)
			ReceivingLoopSoundIndex = AudioComponent->GetUniqueID();
	}	

	CurrentUIType = EWRUIType::Receiving;
}

void UWRUIPanel_VideoChatScreen::ShowConnectedUI()
{
	if (ReceivingLoopSoundIndex != INDEX_NONE)
	{
		WRSFXManager::Get()->Remove(ReceivingLoopSoundIndex);
		ReceivingLoopSoundIndex = INDEX_NONE;
	}

	PlayAnimationByName("ResetAnimation");
	PlayAnimationByName("OpenConnectedAnimation");
	//SetVideoChatUserData(InVideoChatUserData);
	CanvasPanel_Calling->SetVisibility(ESlateVisibility::Collapsed);
	CanvasPanel_Reject->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CanvasPanel_RejectArrow->SetVisibility(ESlateVisibility::Collapsed);
	Slot_VoiceCommand->SetVisibility(ESlateVisibility::Collapsed);
	CanvasPanel_Translation->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Image_SoundWave->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	DetermineTranslationUIState();
	WRTextBlock_Msg->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_VideoChatConnectedKey"));

	if (bCyberUI)
		WRSFXManager::Get()->Spawn2D(TEXT("VideoChatCalling.VideoChatCalling"));

	ShowPreviewCharacter();
	CurrentUIType = EWRUIType::Connected;
}

void UWRUIPanel_VideoChatScreen::ShowDisConnectedUI()
{
	if (ReceivingLoopSoundIndex != INDEX_NONE)
	{
		WRSFXManager::Get()->Remove(ReceivingLoopSoundIndex);
		ReceivingLoopSoundIndex = INDEX_NONE;
	}

	WRTextBlock_Msg->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_VideoChatDisconnectedKey"));
	// 연출?

	WRSFXManager::Get()->Spawn2D(TEXT("VideoChatCallend.VideoChatCallend"));
	

	//if (CurrentUIType == EWRUIType::Connected)
	//{
	//	PlayAnimationByName("CloseConnectedAnimation");
	//	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(Slot_Preview);
	//	if (PreviewStudio && PreviewStudio->IsCompleted())
	//	{
	//		AWRCharacter* PlayerCharacter = Cast<AWRCharacter>(PreviewStudio->GetCustomActor());
	//		if (PlayerCharacter)
	//			PlayerCharacter->PlayClothChangeDirection(0.3f, true);
	//	}		
	//}
	//else
	//{
	//	PlayAnimationByName("CloseAnimation");
	//}	

	PlayAnimationByName("CloseAnimation");
	CurrentUIType = EWRUIType::Disconnected;
}

void UWRUIPanel_VideoChatScreen::PlayCallingAnimation()
{
	PlayAnimationByName("CallingAnimation");
}

void UWRUIPanel_VideoChatScreen::PlayRejectAnimation()
{
	PlayAnimationByName("RejectAnimation");
}

void UWRUIPanel_VideoChatScreen::PlaySpeakGesture()
{
	/*if (PreviewFloorTIme <= 0)
	{
		PlayPreviewFloorProdution();
	}

	PreviewFloorTIme = 0.5f;*/

	if (SoundWaveTime <= 0)
	{
		PlaySoundWave();
	}

	SoundWaveTime = 0.5f;
	

	/*if (FlowCoolTimeForSpeakAni < WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_SpeakAni_CoolTime))
		return;*/

	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(Slot_Preview);
	if (PreviewStudio == nullptr)
		return;

	AWRCharacter* PlayerCharacter = Cast<AWRCharacter>(PreviewStudio->GetCustomActor());
	if (PlayerCharacter == nullptr)
		return;

	if (PlayerCharacter->GetCurrentFSMType() != EWRFSM::Idle)
		return;

	if (PlayerCharacter->GetCharacterType() != EWRCharacterType::Player)
		return;

	const FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(PlayerCharacter->GetCharacterTableID());
	if (PlayerTable == nullptr)
		return;

	if (PlayerTable->VoiceChatAnis.Num() <= 0)
		return;

	WRFSMInfo FSMInfo;
	FSMInfo.FSMType = EWRFSM::VoiceChatMotion;
	//FSMInfo.JumpToSectionName = InAniID.ToString();
	
	int32 Count = 0;
	for (int32 i = 0; i < PlayerTable->VoiceChatAnis.Num(); ++i)
	{
		if (PlayerTable->VoiceChatAnis[i].IsNone())
			break;

		++Count;
	}

	int32 SectionNameIndex = FMath::Rand() % Count;

	FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(PlayerTable->VoiceChatAnis[SectionNameIndex]);
	PlayerCharacter->DetermineFSMSignal(FSMInfo);
	//FlowCoolTimeForSpeakAni = 0.f;
}

void UWRUIPanel_VideoChatScreen::PlayPreviewFloorProdution()
{
	/*if (PreviewFloorActor == nullptr)
		return;

	PreviewFloorActor->SetMaterialParameterValue<float>("circlemain", 1);*/
}

void UWRUIPanel_VideoChatScreen::StopPreviewFloorProdution()
{
	/*if (PreviewFloorActor == nullptr)
		return;

	PreviewFloorActor->SetMaterialParameterValue<float>("circlemain", 0);*/
}

void UWRUIPanel_VideoChatScreen::PlaySoundWave()
{
	if (Mat_SoundWave)
	{
		Mat_SoundWave->SetScalarParameterValue("CircleMain", 1);
	}
}

void UWRUIPanel_VideoChatScreen::StopSoundWave()
{
	if (Mat_SoundWave)
	{
		Mat_SoundWave->SetScalarParameterValue("CircleMain", 0);
	}
}

void UWRUIPanel_VideoChatScreen::StartVoiceRecognition()
{
	if (bCyberUI == false)
		return;

	if (WRInputSystemManager::Get()->IsImpossibleVoiceRecording())
		return;

	UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatVoiceSubCommandTable>();
	if (DataTable == nullptr)
		return;

	TArray<FWRSubCommandData> SubCommandDatas;
	const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

	for (auto& iter : CurrencyData)
	{
		FWRVideoChatVoiceSubCommandTable* VideoChatVoiceSubCommandTable = reinterpret_cast<FWRVideoChatVoiceSubCommandTable*>(iter.Value);
		if (VideoChatVoiceSubCommandTable == nullptr)
			continue;

		if (CurrentUIType == EWRUIType::Calling)
		{
			if (VideoChatVoiceSubCommandTable->ConditionType != EWRVideoChatSubCommandConditionType::Calling)
				continue;
		}
		else if (CurrentUIType == EWRUIType::Receiving)
		{
			if (VideoChatVoiceSubCommandTable->ConditionType != EWRVideoChatSubCommandConditionType::Receiving)
				continue;
		}
		else 
		{
			continue;
		}

		FWRVoiceSubCommandTable* VoiceSubCommandTable = WRTableManager::Get()->FindRow<FWRVoiceSubCommandTable>(VideoChatVoiceSubCommandTable->VoiceSubCommandTID);
		if (VoiceSubCommandTable == nullptr)
			continue;

		FWRSubCommandData TempData;
		TempData.Tid = VideoChatVoiceSubCommandTable->VoiceSubCommandTID;
		TempData.NounWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->NounWord);
		TempData.VerbWord = WRUIUtility::ConvertFNameToFString(VoiceSubCommandTable->VerbWord);
		SubCommandDatas.Add(TempData);
	}

	if (SubCommandDatas.Num() == 0)
		return;

	WRInputSystemManager::Get()->StartRecording(EWRVoiceContentType::VideoChatScreen, SubCommandDatas, FWRVoiceEventDelegate::CreateUObject(this, &UWRUIPanel_VideoChatScreen::OnResultVoiceRecognitionReceived));
}

void UWRUIPanel_VideoChatScreen::OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID)
{
	if (bCyberUI == false)
		return;

	// [SeungYeop 2020/07/22] : 청자 기준 번역이 들어가면서 음성명령 종료시 번역을 켜줄건지 판단해야한다.
	WRVoiceContentsManager::Get()->StartChatRecording();

	if (InVoiceSubCommandTID == INVALID_TABLE_TID)
		return;

	if (CurrentUIType == EWRUIType::Calling)
	{
		const FWRVideoChatVoiceSubCommandTable* FoundVideoChatVoiceSubCommandTable = FWRVideoChatVoiceSubCommandTable::FindVideoChatVoiceSubCommandTable(InVoiceSubCommandTID);
		if (FoundVideoChatVoiceSubCommandTable == nullptr)
			return;

		if (FoundVideoChatVoiceSubCommandTable->ActionType == EWRVideoChatSubCommandAction::Reject)
		{
			WRVoiceContentsManager::Get()->GetVideoChatController().RequestEndVideoChat(PlayerAID);
		}
	}
	else if (CurrentUIType == EWRUIType::Receiving)
	{
		const FWRVideoChatVoiceSubCommandTable* FoundVideoChatVoiceSubCommandTable = FWRVideoChatVoiceSubCommandTable::FindVideoChatVoiceSubCommandTable(InVoiceSubCommandTID);
		if (FoundVideoChatVoiceSubCommandTable == nullptr)
			return;

		if (FoundVideoChatVoiceSubCommandTable->ActionType == EWRVideoChatSubCommandAction::Calling)
		{
			WRVoiceContentsManager::Get()->GetVideoChatController().RequestAgree2BeginVideoChat(true, PlayerAID);
		}
		else if (FoundVideoChatVoiceSubCommandTable->ActionType == EWRVideoChatSubCommandAction::Reject)
		{
			WRVoiceContentsManager::Get()->GetVideoChatController().RequestAgree2BeginVideoChat(false, PlayerAID);
		}
	}
	else if (CurrentUIType == EWRUIType::Connected)
	{

	}
}

void UWRUIPanel_VideoChatScreen::OnChatRecordingPressed()
{
	if (bCyberUI == false)
		return;



	bool bEnable = WRVoiceContentsManager::Get()->ContainsInMyTranslatingPlayerList(PlayerAID);
	if (bEnable)
	{
		WRVoiceContentsManager::Get()->RequestDeregistSpeaker(PlayerAID);
	}
	else
	{
		WRVoiceContentsManager::Get()->RequestRegistSpeaker(PlayerAID);
	}
}

void UWRUIPanel_VideoChatScreen::AddChatBubble(const FText& IN InContent, const float IN InTime /*= 0.f*/)
{
	if (bCyberUI == false)
		return;

	Slot_VideoChatBubble->AddVideoChatBubble(InContent, InTime);
}

void UWRUIPanel_VideoChatScreen::SetVideoChatUserData(const FWRVideoChatUserData& IN InVideoChatUserData)
{
	PlayerAID = InVideoChatUserData.PlayerAID;

	WRTextBlock_PlayerName->SetText(InVideoChatUserData.CharacterName);
}

void UWRUIPanel_VideoChatScreen::OnPreviewCharacterSpawCompleted(AActor* IN InCharacter)
{
	if (InCharacter == nullptr)
		return;

	AWRCharacter* SelectedComponentOwner = Cast<AWRCharacter>(InCharacter);
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	UMeshComponent* MeshComponent = SelectedComponentOwner->GetMesh();
	if (MeshComponent == nullptr)
	{
		return;
	}

	SelectedComponentOwner->SetMaterialParameterValue("00.ClothChangingMain", 0.f, MeshComponent);

	if (WRVoiceContentsManager::Get()->GetVideoChatController().GetCurrentVideoChatState() == WRVideoChatController::EWRVideoChatState::Connected)
		ShowPreviewCharacter();
}

void UWRUIPanel_VideoChatScreen::SpawnPreviewCharacter(const FWRVideoChatUserData& IN InVideoChatUserData)
{
	if (bCyberUI)
	{
		return;
	}
	// Preview Info
	FWRPreviewInfo PreviewInfo;
	PreviewInfo.Type = EWRPreviewType::CustomActor;
	PreviewInfo.ManualVisible = true;
	PreviewInfo.AutoScaling = false;
	PreviewInfo.SetPreviewWidget(Slot_Preview);
	PreviewInfo.PreviewLoadCompletedDelegate.BindUObject(this, &UWRUIPanel_VideoChatScreen::OnPreviewCharacterSpawCompleted);

	// + Custom Actor Data
	FWRPreviewCustomActorData CustomActorData;
	CustomActorData.PresetTID = InVideoChatUserData.CharacterPresetTID;
	CustomActorData.CostumeList = InVideoChatUserData.CostumeList;
	CustomActorData.CustomizeInfo = InVideoChatUserData.CustomizeInfo;
	PreviewInfo.SetCustomActorData(CustomActorData);

	WRPreviewManager::Get()->SetPreview(PreviewInfo);


	//// Preview Info
	//FWRPreviewInfo PreviewInfo;
	//PreviewInfo.Type = EWRPreviewType::CustomActor;
	//PreviewInfo.SetPreviewWidget(Slot_Preview);
	//PreviewInfo.PreviewLoadCompletedDelegate = FWRPreviewLoadCompletedDelegate::CreateUObject(this, &UWRUISlot_InventoryItemInfo::OnActorSpawnCompleted);
	//// Preview Base Data Set
	//FWRPreviewDataBase PreviewBaseData;
	//PreviewBaseData.UseRotate = true;
	//PreviewBaseData.UseZoom = true;
	//PreviewBaseData.ActorRotationOffset = FRotator(0, 90, 0);
	//PreviewBaseData.CameraOffset = FVector(0, -80, 0);
	//PreviewBaseData.CameraOffsetPivotSocket = FName("head");
	//PreviewInfo.ModifyBaseData(PreviewBaseData);
	//// + Custom Actor Data
	//FWRPreviewCustomActorData CustomActorData;
	//CustomActorData.PresetTID = WRAccountManager::Get()->GetMyCharacterPresetTID();
	//CustomActorData.AddItem(FoundItem->GetTID());
	//PreviewInfo.SetCustomActorData(CustomActorData);
	//WRPreviewManager::Get()->SetPreview(PreviewInfo);
}

void UWRUIPanel_VideoChatScreen::ShowPreviewCharacter()
{
	AWRPreviewStudio* PreviewStudio = WRPreviewManager::Get()->GetPreviewStudio(Slot_Preview);
	if (PreviewStudio == nullptr)
		return;

	if (PreviewStudio->IsCompleted() == false)
		return;

	AWRCharacter* PlayerCharacter = Cast<AWRCharacter>(PreviewStudio->GetCustomActor());
	if (PlayerCharacter == nullptr)
		return;

	if (Slot_Preview->IsVisible())
		return;

	//if (CurrentUIType != EWRUIType::Connected)
	//	return;

	//if (IsAnimationPlayingByName("OpenConnectedAnimation"))
	//	return;

	FVector MeshLocation = WRActorUtility::GetBoneTransform(PlayerCharacter, "root").GetLocation();
	MeshLocation.Z += PreviewFloorZOffset;

	// Target Async Lambda
	/*auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([=](UObject* InObject, FString InKey)
	{
		AsyncLoadKeyForPreviewFloor.Empty();

		PreviewFloorActor = Cast<AWRStaticMeshActor>(InObject);
		if (PreviewFloorActor == nullptr)
			return;

		PreviewFloorActor->AddToRoot();
		PreviewFloorActor->SetActorLocation(MeshLocation);
		PreviewFloorActor->SetActorScale3D(FVector(PreviewFloorScale));
		if (PreviewStudio->IsValidLowLevel())
		{
			PreviewStudio->AddActorInShowList(Cast<AActor>(PreviewFloorActor));
		}
	});

	AsyncLoadKeyForPreviewFloor = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(TEXT("/Game/Asset/BluePrint/StaticProp/PreviewFloor.PreviewFloor"), AsyncCreateComplete);*/
	
	PlayerCharacter->PlayClothChangeDirection();
	Slot_Preview->OnShow();
}

void UWRUIPanel_VideoChatScreen::DetermineTranslationUIState()
{
	bool bEnable = WRVoiceContentsManager::Get()->ContainsInMyTranslatingPlayerList(PlayerAID);
	
	if (bEnable)
	{
		//CanvasPanel_Translation->SetRenderOpacity(1.f);
		Image_TranslatingLoop->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		PlayAnimationByName("TranslatingLoopAnimation", 0.f, 0);
	}
	else
	{
		//CanvasPanel_Translation->SetRenderOpacity(1.f);
		Image_TranslatingLoop->SetVisibility(ESlateVisibility::Collapsed);
		StopAnimationByName("TranslatingLoopAnimation");
	}
}
