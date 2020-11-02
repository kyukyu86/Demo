// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIPanel_CharacterCreate.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Network/IMMessageSender.h"
#include "Manager/WRCharacterManager.h"
#include "Table/WRCharacterPresetTable.h"
#include "Table/Base/WRTableManager.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/PostProcessVolume.h"
#include "Actor/Character/WRCharacter.h"
#include <Engine/TextureRenderTarget2D.h>
#include <Engine/Texture.h>
#include "Slate/WidgetRenderer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Image.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "EngineUtils.h"
//#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Manager/WRSFXManager.h"
#if 0
#include "Manager/WRNoticeManager.h"
#endif

#define DEFAULT_MALE_START_TID 1
#define DEFAULT_FEMALE_START_TID 10
#define MAX_PRESET_MALE_CHARACTER_NUMBER 3
#define MAX_PRESET_FEMALE_CHARACTER_NUMBER 12
#define BLUR_OFF_APETURE_VALUE 32
#define WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME "00.ClothChangingMain"

void UWRUIPanel_CharacterCreate::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_CharacterName);
	BIND_WIDGET(UImage, Image_L1);
	BIND_WIDGET(UImage, Image_R1);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_CreateButton);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_CancelButton);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_ChangeName);
	BIND_WIDGET(UWidgetSwitcher, WidgetSwitcher_Mode);
	BIND_WIDGET(UImage, Image_Male);
	BIND_WIDGET_WITH_IMPLEMENTATION(UImage, Image_Female, Image_Female->SetRenderOpacity(0.1f); );

	IMMessageSender::RequestSelectPlayerName();

	SetFocusableInputMode(true, this);

	InitializeDOF();
}

void UWRUIPanel_CharacterCreate::NativeDestruct()
{
	SetFocusableInputMode(false);

	if (SpawnedPresetCharacter)
	{
		WRCharacterManager::Get()->Remove(SpawnedPresetCharacter, false);
	}
	//EquipCompleteDelegate.Unbind();
	//UnloadEffectCurve();
	Super::NativeDestruct();
}

void UWRUIPanel_CharacterCreate::OnAnimationFinished(const UWidgetAnimation* Animation)
{
	/*if (Animation == FadeInAnimation)
	{
		WRCharacterManager::Get()->Remove(SpawnedPresetCharacter, false);
		WidgetSwitcher_Mode->SetActiveWidgetIndex(int32(GenderSelect));
		CanvasPanel_CancelButton->SetVisibility(ESlateVisibility::Collapsed);
		Mode = GenderSelect;
	}*/
}

void UWRUIPanel_CharacterCreate::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

// 	if (bButtonClickable)
// 	{
// 		UE_LOG(WRLog, Warning, TEXT("ChracterCreate : bButtonClickable = true"));
// 	}
// 	else
// 	{
// 		UE_LOG(WRLog, Warning, TEXT("ChracterCreate : bButtonClickable = false"));
// 	}

	if (bButtonSecondClickable == false)
	{
		EnableClickTimeLeft -= InDeltaTime;

		if (EnableClickTimeLeft <= 0.0f)
			bButtonSecondClickable = true;
	}
	
	BlurTime -= InDeltaTime;
	if (BlurTime > 0)
	{
		PostProcessSettings->DepthOfFieldFstop = BLUR_OFF_APETURE_VALUE - (BlurTime * BLUR_OFF_APETURE_VALUE);
	}
	else if (PostProcessSettings->DepthOfFieldFstop != BLUR_OFF_APETURE_VALUE)
	{
		PostProcessSettings->DepthOfFieldFstop = BLUR_OFF_APETURE_VALUE;
	}

	
}

void UWRUIPanel_CharacterCreate::InitializeDOF()
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	for (TActorIterator<APostProcessVolume> Iter(World); Iter; ++Iter)
	{
		APostProcessVolume* PostProcessVolume = *Iter;
		if (PostProcessVolume == nullptr)
			continue;

		FPostProcessVolumeProperties PostProcessVolumeProperties = PostProcessVolume->GetProperties();
		PostProcessSettings = const_cast<FPostProcessSettings*>(PostProcessVolumeProperties.Settings);
		BlurTime = 1.0f;
		break;
	}
}

FReply UWRUIPanel_CharacterCreate::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
// 	EWRInput Input = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
// 	if(AnyKeys.Contains(Input))

	if (IsAnyAnimationPlaying() == true)
		return Super::NativeOnKeyUp(InGeometry, InKeyEvent);

	if (OKkeys.Contains(InKeyEvent.GetKey()))
	{
		OnOKKeyClicked();
	}
	else if (CancelKeys.Contains(InKeyEvent.GetKey()))
	{
		OnCancelKeyClicked();
	}
	else if (ModifyNameKeys.Contains(InKeyEvent.GetKey()))
	{
		OnModifyNameKeyClicked();
	}
	else if (LeftShiftKeys.Contains(InKeyEvent.GetKey()))
	{
		OnL1KeyClicked();
	}
	else if (RightShiftKeys.Contains(InKeyEvent.GetKey()))
	{
		OnR1KeyClicked();
	}

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

FReply UWRUIPanel_CharacterCreate::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (IsAnyAnimationPlaying() == true)
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (OKkeys.Contains(InKeyEvent.GetKey()))
	{
		OnOKKeyPressed();
	}
	else if (CancelKeys.Contains(InKeyEvent.GetKey()))
	{
		OnCancelKeyPressed();
	}
	else if (ModifyNameKeys.Contains(InKeyEvent.GetKey()))
	{
		OnModifyNameKeyPressed();
	}
	else if (LeftShiftKeys.Contains(InKeyEvent.GetKey()))
	{
		OnL1KeyPressed();
	}
	else if (RightShiftKeys.Contains(InKeyEvent.GetKey()))
	{
		OnR1KeyPressed();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UWRUIPanel_CharacterCreate::OnOKKeyClicked()
{
	CanvasPanel_CreateButton->SetRenderOpacity(1);
	WRSFXManager::Get()->Spawn2D(TEXT("UI_OK.UI_OK"));

	if (Mode == GenderSelect)
	{
		if (Gender == Male)
		{
			CurrentPresetTID = DEFAULT_MALE_START_TID;
		}
		else if (Gender == Female)
		{
			CurrentPresetTID = DEFAULT_FEMALE_START_TID;
		}
#if 0		
		FWRCompleteAsyncLoad PresetCharacterSpawnedDelegate;

		//SpawnedPresetCharacter = WRCharacterManager::Get()->SpawnPresetCharacters(CurrentPresetTID);
		PresetCharacterSpawnedDelegate.BindUObject(this, &UWRUIPanel_CharacterCreate::OnPresetCharacterSpawned);
		WRCharacterManager::Get()->SpawnPresetCharacters(CurrentPresetTID, PresetCharacterSpawnedDelegate);
#endif
        SpawnedPresetCharacter = SpawnPresetCharacter(CurrentPresetTID);
		WidgetSwitcher_Mode->SetActiveWidgetIndex(int32(CharacterCreate));
		CanvasPanel_CancelButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		bButtonSecondClickable = false;
		EnableClickTimeLeft = 3.0f;
		Mode = CharacterCreate;
	}
	else if (Mode == CharacterCreate)
	{
#ifdef DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
#else // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		IMMessageSender::RequestCreatePlayer(TextBlock_CharacterName->GetText().ToString(), CurrentPresetTID);
#endif // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
	}
}

void UWRUIPanel_CharacterCreate::OnCancelKeyClicked()
{
	CanvasPanel_CancelButton->SetRenderOpacity(1);

	if (Mode == GenderSelect)
	{

	}
	else if (Mode == CharacterCreate)
	{
		//PlayAnimation(FadeInAnimation);
		WRCharacterManager::Get()->Remove(SpawnedPresetCharacter, false);
		SpawnedPresetCharacter = nullptr;
		WidgetSwitcher_Mode->SetActiveWidgetIndex(int32(GenderSelect));
		CanvasPanel_CancelButton->SetVisibility(ESlateVisibility::Collapsed);
		Mode = GenderSelect;
		WRSFXManager::Get()->Spawn2D(TEXT("UI_CANCEL.UI_CANCEL"));
	}
}

void UWRUIPanel_CharacterCreate::OnModifyNameKeyClicked()
{
	CanvasPanel_ChangeName->SetRenderOpacity(1);
	// Server 로 Name 변경 패킷 보내기

	if (Mode == GenderSelect)
	{
	}
	else if (Mode == CharacterCreate)
	{
		IMMessageSender::RequestSelectPlayerName();
	}
	
}

void UWRUIPanel_CharacterCreate::OnL1KeyClicked()
{
	Image_L1->SetRenderOpacity(1.0);
	WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));

	/*WRNoticeManager::Get()->AddStyleNotice(TEXT("The user has input L1 Pressed."));
	WRNoticeManager::Get()->AddNotice(TEXT("The user has input L1."));*/

// 	if (IsButtonClickable() == false)
// 		return;

	if (Mode == GenderSelect)
	{
		SwitchGender();
	}
	else if (Mode == CharacterCreate)
	{
		ChangeCostume(PreviousOrder);
	}
}

void UWRUIPanel_CharacterCreate::OnR1KeyClicked()
{
	Image_R1->SetRenderOpacity(1.0);
	WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));

	/*WRNoticeManager::Get()->AddStyleNotice(TEXT("The user has input R1 Pressed."));
WRNoticeManager::Get()->AddNotice(TEXT("The user has input R1."));*/

// 	if (IsButtonClickable() == false)
// 		return;

	if (Mode == GenderSelect)
	{
		SwitchGender();
	}
	else if (Mode == CharacterCreate)
	{
		ChangeCostume(NextOrder);
	}
}
	

void UWRUIPanel_CharacterCreate::OnOKKeyPressed()
{
	CanvasPanel_CreateButton->SetRenderOpacity(0.5);
}

void UWRUIPanel_CharacterCreate::ChangeCostume(EWRChangeOrder IN InOrder)
{
	if (SpawnedPresetCharacter == nullptr)
		return;

	if (bButtonSecondClickable == false)
		return;

	if (bIsProducing == true)
		return;

	UWRActorComponentEquipmentLook* EquipLook = SpawnedPresetCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipLook == nullptr)
		return;

	if (EquipLook->IsDirecting())
		return;

	WRTableID NextPresetTID = INVALID_TABLE_TID;
	if (InOrder == PreviousOrder)
	{
		NextPresetTID = CalculatePreviousPresetTID();
	}
	else if (InOrder == NextOrder)
	{
		NextPresetTID = CalculateNextPresetTID();
	}
	else
	{
		ensureMsgf(false, TEXT("Invalid Order entered."));
		return;
	}

	if (IsCharacterSpawnNeeded(CurrentPresetTID, NextPresetTID))
	{
		WRCharacterManager::Get()->Remove(SpawnedPresetCharacter, false);
		SpawnedPresetCharacter = SpawnPresetCharacter(NextPresetTID);
	}

	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(NextPresetTID);

	if (PresetTable != nullptr)
	{
		if (EquipLook)
		{
			//EquipCompleteDelegate.BindUObject(this, &UWRUIPanel_CharacterCreate::OnEquipCompleted);

			if (EquipLook->Equip(PresetTable->HoloEquipmentLookTID) == true)
			{
				UE_LOG(WRLog, Warning, TEXT("previous TID : %d, New TID : %d"), CurrentPresetTID, NextPresetTID);
				CurrentPresetTID = NextPresetTID;
				bButtonClickable = false;
			}
		}
	}
}

bool UWRUIPanel_CharacterCreate::TickProduceStartEffect(float InDeltaTime)
{
	if (CurveStartEffect == nullptr)
		return false;

	ProduceTime += InDeltaTime;

	float StartEffectMinTime, StartEffectMaxTime;
	CurveStartEffect->GetTimeRange(StartEffectMinTime, StartEffectMaxTime);

	if (CurveStartEffect->GetFloatValue(ProduceTime) <= 0.0f)
		return true;

	//EnvPresetCharacter->SetVisibility(true, true);

	if (ProduceTime >= StartEffectMaxTime)
	{
		// [ 2020-5-12 : vuvle ] : 만약 렉 현상으로 연출이 Skip 되었을 때에도 캐릭터가 보일수 있도록 추가 예외처리. 만약 해당 상황이 발생이 되지 않는다면 삭제하는게 낫다.
		if (SpawnedPresetCharacter->IsVisibility() == false)
			SpawnedPresetCharacter->SetVisibility(true, true);

		SetMaterialParameterValue(1.0f);
		ProduceTickHandle.Reset();
		bIsProducing = false;
		//UE_LOG(WRLog, Warning, TEXT("StartEffect Produce Ended"));
		return false;
	}

	if (SpawnedPresetCharacter->IsVisibility() == false)
		SpawnedPresetCharacter->SetVisibility(true, true);

	SetMaterialParameterValue(CurveStartEffect->GetFloatValue(ProduceTime));

	return true;
}

#if 0 // EndEffect 는 필요하지 않으므로 Comment화
bool UWRUIPanel_CharacterCreate::TickProduceEndEffect(float InDeltaTime)
{
	if (CurveEndEffect == nullptr)
		return false;

	ProduceTime += InDeltaTime;

	float EndEffectMinTime, EndEffectMaxTime;
	CurveEndEffect->GetTimeRange(EndEffectMinTime, EndEffectMaxTime);

	if (ProduceTime >= EndEffectMaxTime)
	{
		SetMaterialParameterValue(0.0f);
		UnloadEffectCurve();
		WRCharacterManager::Get()->Remove(SpawnedPresetCharacter);
		ProduceTickHandle.Reset();
		SpawnedPresetCharacter = SpawnPresetCharacter(CurrentPresetTID);
		//UE_LOG(WRLog, Warning, TEXT("EndEffect Produce Ended"));
		//bIsProducing = false;
		return false;
	}

	// 	if (CurveEndEffect->GetFloatValue(ProduceTime) >= 1.0f)
	// 		return true;
	SetMaterialParameterValue(CurveEndEffect->GetFloatValue(ProduceTime));

	return true;
}
#endif

void UWRUIPanel_CharacterCreate::OnCancelKeyPressed()
{
	CanvasPanel_CancelButton->SetRenderOpacity(0.5);
}

void UWRUIPanel_CharacterCreate::OnModifyNameKeyPressed()
{
	CanvasPanel_ChangeName->SetRenderOpacity(0.5);
}

void UWRUIPanel_CharacterCreate::OnL1KeyPressed()
{
	Image_L1->SetRenderOpacity(0.5);
}

void UWRUIPanel_CharacterCreate::OnR1KeyPressed()
{
	Image_R1->SetRenderOpacity(0.5);
}

bool UWRUIPanel_CharacterCreate::IsCharacterSpawnNeeded(const WRTableID InCurrentPresetTID, const WRTableID InPresetToChangeTID)
{
	FWRCharacterPresetTable* PreviousPresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InCurrentPresetTID);
	FWRCharacterPresetTable* NextPresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InPresetToChangeTID);

	if (PreviousPresetTable == nullptr || NextPresetTable == nullptr)
		return false;

	if (PreviousPresetTable->Gender != NextPresetTable->Gender)
		return true;

	return false;
}

void UWRUIPanel_CharacterCreate::OnEquipCompleted(AActor* IN InActor, EWREquipPart IN InEquippedPart)
{
	/*bButtonClickable = true;
	EquipCompleteDelegate.Unbind();*/
}

WRTableID UWRUIPanel_CharacterCreate::CalculateNextPresetTID()
{
	WRTableID StartTID = INVALID_TABLE_TID;
	WRTableID EndTID = INVALID_TABLE_TID;

	if (Gender == Male)
	{
		StartTID = DEFAULT_MALE_START_TID;
		EndTID = MAX_PRESET_MALE_CHARACTER_NUMBER;
	}
	else if (Gender == Female)
	{
		StartTID = DEFAULT_FEMALE_START_TID;
		EndTID = MAX_PRESET_FEMALE_CHARACTER_NUMBER;
	}


	WRTableID NextPresetTID = CurrentPresetTID + 1;
	if (NextPresetTID > EndTID)
	{
		NextPresetTID = StartTID;
	}

	return NextPresetTID;
}

WRTableID UWRUIPanel_CharacterCreate::CalculatePreviousPresetTID()
{
	WRTableID StartTID = INVALID_TABLE_TID;
	WRTableID EndTID = INVALID_TABLE_TID;

	if (Gender == Male)
	{
		StartTID = DEFAULT_MALE_START_TID;
		EndTID = MAX_PRESET_MALE_CHARACTER_NUMBER;
	}
	else if (Gender == Female)
	{
		StartTID = DEFAULT_FEMALE_START_TID;
		EndTID = MAX_PRESET_FEMALE_CHARACTER_NUMBER;
	}


	WRTableID PreviousPresetTID = CurrentPresetTID - 1;
	if (PreviousPresetTID < StartTID)
	{
		PreviousPresetTID = EndTID;
	}

	return PreviousPresetTID;
}

void UWRUIPanel_CharacterCreate::SwitchGender()
{
	if (Gender == Male)
	{
		Gender = Female;
		Image_Male->SetRenderOpacity(0.1f);
		Image_Female->SetRenderOpacity(1.0f);
	}
	else
	{
		Gender = Male;
		Image_Male->SetRenderOpacity(1.0f);
		Image_Female->SetRenderOpacity(0.1f);
	}
}

AWRCharacter* UWRUIPanel_CharacterCreate::SpawnPresetCharacter(const WRTableID IN InTableID)
{
	UE_LOG(WRLog, Warning, TEXT("Spawn Table ID : %d"), InTableID);

	AWRCharacter* SpawnedCharacter = WRCharacterManager::Get()->SpawnPresetCharacters(InTableID);

	ProduceTime = 0.0f;

	// [ 2020-4-29 : vuvle ] : BP 비동기 로드 이후 잠깐 보이는 현상이 생겨 안보이게 만드는 구문 추가
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	LoadEffectCurve();

	SpawnedCharacter->SetVisibility(false, true);
	ProduceTickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UWRUIPanel_CharacterCreate::TickProduceStartEffect));
	//SetMaterialParameterValue(0.0f);
	bIsProducing = true;
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_

	return SpawnedCharacter;
}

void UWRUIPanel_CharacterCreate::SetCharacterName(const FString& IN InString)
{
	WRUIUtility::SetTextSafe(TextBlock_CharacterName, InString);
}

#if 0
void UWRUIPanel_CharacterCreate::OnPresetCharacterSpawned(AWRCharacter* IN InSpawnedCharacter)
{
	if(InSpawnedCharacter)
		SpawnedPresetCharacter = InSpawnedCharacter;
}
#endif

void UWRUIPanel_CharacterCreate::TurnDOFoff()
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	for (TActorIterator<APostProcessVolume> Iter(World); Iter; ++Iter)
	{
		APostProcessVolume* PostProcessVolume = *Iter;
		if (PostProcessVolume == nullptr)
			continue;

		FPostProcessVolumeProperties PostProcessVolumeProperties = PostProcessVolume->GetProperties();
		FPostProcessSettings* Settings = const_cast<FPostProcessSettings*>(PostProcessVolumeProperties.Settings);
		if (Settings)
			Settings->DepthOfFieldFstop = BLUR_OFF_APETURE_VALUE;
		break;
	}
}

void UWRUIPanel_CharacterCreate::SetMaterialParameterValue(const float InValue)
{
	if (SpawnedPresetCharacter == nullptr)
		return;

	// UE_LOG(WRLog, Warning, TEXT("Param : %f"), InValue);

	UWRActorComponentEquipmentLook* EquipmentLookComponent = SpawnedPresetCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent)
	{
		EquipmentLookComponent->SetMaterialParameterValue(WR_CHANGE_DIRECT_MATERIALPARAMETER_NAME, InValue);
	}
}

void UWRUIPanel_CharacterCreate::LoadEffectCurve()
{
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(CurrentPresetTID);

	if (PresetTable == nullptr)
		return;

	FWREquipmentLookTable* EquipmentLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(PresetTable->HoloEquipmentLookTID);

	CurveEndEffect = EquipmentLookTable->ChangeCurveSRC.LoadSynchronous();
	if (CurveEndEffect->IsValidLowLevel() == true)
	{
		CurveEndEffect->AddToRoot();
	}

	CurveStartEffect = EquipmentLookTable->ChangeCurveDEST.LoadSynchronous();
	if (CurveStartEffect->IsValidLowLevel() == true)
	{
		CurveStartEffect->AddToRoot();
	}
}

void UWRUIPanel_CharacterCreate::UnloadEffectCurve()
{
	if (CurveStartEffect->IsValidLowLevel() == true)
	{
		CurveStartEffect->RemoveFromRoot();
		CurveStartEffect = nullptr;
	}

	if (CurveEndEffect->IsValidLowLevel() == true)
	{
		CurveEndEffect->RemoveFromRoot();
		CurveEndEffect = nullptr;
	}
}