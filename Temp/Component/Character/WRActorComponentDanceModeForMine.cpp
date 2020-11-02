// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentDanceModeForMine.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacterNPC.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRSocialTable.h"
#include "Table/WRPlayerTable.h"

#include "Manager/WRUIManager.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRItemManager.h"
#include "Manager/WRSFXManager.h"

#include "Network/IMMessageSender.h"
#include "Utility/WRActorUtility.h"
#include "Define/WRPathDefine.h"
#include "Enum/EWRCharacter.h"
#include "UI/Panel/WRUIPanel_DanceMode.h"
#include "UI/Panel/WRUIPanel_DanceModeBoard.h"
#include "Manager/WRConsoleManager.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRCheersManager.h"
#include "UI/Panel/WRUIPanel_Drink.h"
#include "Manager/WRInventoryManager.h"


void UWRActorComponentDanceModeForMine::OnCreateComponent()
{
	Super::OnCreateComponent();


}

void UWRActorComponentDanceModeForMine::OnDestroyComponent()
{
	Super::OnDestroyComponent();
}

void UWRActorComponentDanceModeForMine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeForCheckDanceArea += DeltaTime;

	if (IsDanceModeOn())
	{
		if (NextStepType != EWRNextStepType::None)
			TickNextStep(DeltaTime);

		if (TimeForCheckDanceArea > 0.5f)
		{
			TimeForCheckDanceArea = 0.f;
			if (CheckDancingPlayerAround() || CheckDancerAround())
			{
				// 임의의 숫자 입력, 어차피 FX 출력이라 포인트 수치 자체는 필요없다. 추후를 위해 해둠
				IncreaseDancePoint(100);
			}
		}
	}
	else
	{
		if (TimeForCheckDanceArea > 0.5f)
		{
			TimeForCheckDanceArea = 0.f;
			bool bEnterDanceArea = IsEnterDanceArea();

			if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::DanceMode))
			{
				if (bEnterDanceArea == false)
				{
					WRUIManager::Get()->CloseUI(EWRUIPanelKind::DanceMode);

					// + UI Exception
					if (WRCheersManager::Get()->AttemptToOpenCheersContents())
					{
						// [ 2020-4-20 : kyu ] 건배상태 체크 및 설정
					}
				}
			}
			else
			{
				if (bEnterDanceArea)
				{
					ShowStartDanceUI();
					
					// + UI Exception					
					if ( WRCheersManager::Get()->PauseCheersContents())
					{
						// [ 2020-4-20 : kyu ] 건배상태 체크 및 해제
					}
				}
			}
		}
	}

	//if (CurDanceStep != EWRDanceStep::None)
	//{
	//	PointTime += DeltaTime;
	//	if (PointTime > 0.5f)
	//	{
	//		PointTime = 0.f;
	//		IncreaseDancePoint(10);
	//	}
	//}
}

void UWRActorComponentDanceModeForMine::GoNextStep()
{
	if (CurDanceGroupTable == nullptr)
		return;

	if (++CurIndex >= CurDanceGroupTable->DanceTIDs.Num())
		CurIndex = 0;

	IMMessageSender::RequestChangeDanceStep(CurDanceGroupTable->GetTableID(), CurDanceGroupTable->DanceTIDs[CurIndex]);
}

void UWRActorComponentDanceModeForMine::StartDanceMode(const WRTableID IN InDanceGroupTID, const bool IN bInRetry /*= false*/)
{
	const FWRDanceGroupTable* DanceGroupTable = WRTableManager::Get()->FindRow<FWRDanceGroupTable>(InDanceGroupTID);
	if (DanceGroupTable == nullptr)
		return;

	if (DanceGroupTable->DanceTIDs.Num() == 0)
		return;

	CurDanceGroupTable = DanceGroupTable;
	CurIndex = 0;
	NextStepType = EWRNextStepType::None;
	FlowTime = 0.f;
	NextStepStartTime = 0.f;
	NextStepEndTime = 0.f;

	if (bInRetry == false)
	{
		AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(GetOwnerCharacter());
		if (MyCharacter)
		{
			float OffsetX = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetXForDance);
			float OffsetZ = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetZForDance);
			FVector OffsetXZ = (FVector::ForwardVector * OffsetX) + (FVector::UpVector * OffsetZ);
			
			if (MyCharacter->IsShowing3rdCharacter())
			{
				Before3rdCharacterOffset = MyCharacter->Get3rdCharacterCameraOffset();
				MyCharacter->Show3rdCharacter(false, Before3rdCharacterOffset);
			}
			MyCharacter->Show3rdCharacter(true, OffsetXZ);
			
			MyCharacter->SetUsingSyncHMDHead(false);
			MyCharacter->SetHMDRotationToUpdate(FRotator::ZeroRotator);
			MyCharacter->SetEnableSyncHMDTick(false);
		}
	}
			
	SetDanceStep(CurDanceGroupTable->DanceTIDs[CurIndex]);

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::DanceMode) == false)
	{
		ShowDafaultDanceUI();
	}
}

void UWRActorComponentDanceModeForMine::StopDanceMode(const bool IN bInStopFSM /*= true*/)
{
	Super::StopDanceMode(bInStopFSM);

	WRUIManager::Get()->CloseUI(EWRUIPanelKind::DanceModeBoard);

	WRUIManager::Get()->CloseUI(EWRUIPanelKind::DanceMode);
	if (bRetry == false && IsEnterDanceArea())
	{
		ShowStartDanceUI();
	}
	
	if (bRetry == false)
	{
		AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(GetOwnerCharacter());
		if (MyCharacter)
		{
			float OffsetX = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetXForDance);
			float OffsetZ = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetZForDance);
			FVector OffsetXZ = (FVector::ForwardVector * OffsetX) + (FVector::UpVector * OffsetZ);

			MyCharacter->Show3rdCharacter(false, OffsetXZ);
			if (Before3rdCharacterOffset.IsZero() == false)
			{
				MyCharacter->Show3rdCharacter(true, Before3rdCharacterOffset);
				Before3rdCharacterOffset = FVector::ZeroVector;
			}

			MyCharacter->SetUsingSyncHMDHead(true);
			MyCharacter->SetEnableSyncHMDTick(true);
		}
	}
}

bool UWRActorComponentDanceModeForMine::ShowStartDanceUI()
{
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
		return false;

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SelectDialogue))
		return false;

	if (GetOwnerCharacter()->GetBoardingVehicle() != nullptr)
		return false;

	UWRUIPanel_Drink* Panel_Drink = WRUIManager::Get()->FindUI< UWRUIPanel_Drink>(EWRUIPanelKind::Drink);
	if (Panel_Drink && Panel_Drink->GetUIType() == EWRDrinkType::Suggested)
		return false;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			//InCombinedPanelData.WidgetComponentBase->SetDrawAtDesiredSize(false);
			//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
			UWRUIPanel_DanceMode* Panel_DanceMode = Cast<UWRUIPanel_DanceMode>(InCombinedPanelData.UIPanel);
			if (Panel_DanceMode)
				Panel_DanceMode->SetFunctionType(UWRUIPanel_DanceMode::EWRFunctionType::StartDanceMode);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::DanceMode, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
 	//FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::DanceMode);
 	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::DanceMode, FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector), EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	return true;
}

void UWRActorComponentDanceModeForMine::ShowDafaultDanceUI()
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			//InCombinedPanelData.WidgetComponentBase->SetDrawAtDesiredSize(false);
			//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
			UWRUIPanel_DanceMode* Panel_DanceMode = Cast<UWRUIPanel_DanceMode>(InCombinedPanelData.UIPanel);
			if (Panel_DanceMode)
				Panel_DanceMode->SetFunctionType(UWRUIPanel_DanceMode::EWRFunctionType::None);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::DanceMode, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::DanceMode);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::DanceMode, FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector), EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
}

void UWRActorComponentDanceModeForMine::IncreaseDancePoint(const int32 IN InDancePoint)
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	FVector SpawnLocation = MyCharacter->GetActorLocation();
	//[20.04.03 SeungYeop] :기획 요청에 따라 주석 처리
//	SpawnLocation += MyCharacter->GetActorForwardVector() * 300.f;
	AWRVFX* SpawnedFX = WRVFXManager::Get()->Spawn(EFFECT_PATH_4_DANCE_POINT, WRCharacterManager::Get()->GetMyCharacter(), nullptr);
	if (SpawnedFX)
	{
		SpawnedFX->SetActorLocation(SpawnLocation);
	}
}

void UWRActorComponentDanceModeForMine::OnFinishDanceEnded()
{
	if (CurDanceGroupTable == nullptr)
		return;

	if (CurDanceGroupTable->DanceTIDs.Num() == 0)
		return;

	bRetry = true;
	IMMessageSender::RequestChangeDanceStep(CurDanceGroupTable->GetTableID(), CurDanceGroupTable->DanceTIDs[0]);
}

void UWRActorComponentDanceModeForMine::OnAckBeginDanceReceived(const WRTableID IN InDanceGroupTID)
{
	StartDanceMode(InDanceGroupTID);
}

void UWRActorComponentDanceModeForMine::OnAckEndDanceReceived()
{
	StopDanceMode();
}

void UWRActorComponentDanceModeForMine::OnAckChangeDanceStepReceived(const WRTableID IN InDanceGroupTID, const WRTableID IN InDanceTID)
{
	if (bRetry)
	{
		StartDanceMode(InDanceGroupTID, true);
		bRetry = false;
	}
	else
	{
		SetDanceStep(InDanceTID);
	}
}

void UWRActorComponentDanceModeForMine::TickNextStep(const float IN InDeltaTime)
{
	FlowTime += InDeltaTime;
	if (NextStepType == EWRNextStepType::Start)
	{
		if (FlowTime > NextStepStartTime)
		{
			UWRUIPanel_DanceMode* Panel_DanceMode = WRUIManager::Get()->FindUI<UWRUIPanel_DanceMode>(EWRUIPanelKind::DanceMode);
			if (Panel_DanceMode && CurDanceTable)
			{
				/*UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *WRUIUtility::ConvertFNameToFString(CurDanceTable->DanceKeyIcon));
				Panel_DanceMode->SetNextStepInput(CurDanceTable->InputKey, Texture);*/
				Panel_DanceMode->SetNextStepInput(CurDanceTable->InputKey, WRUIUtility::ConvertFNameToFString(CurDanceTable->DanceKeyIcon));
			}

			NextStepType = EWRNextStepType::End;
		}
	}
	else if (NextStepType == EWRNextStepType::End)
	{
		if (FlowTime > NextStepEndTime)
		{
			UWRUIPanel_DanceMode* Panel_DanceMode = WRUIManager::Get()->FindUI<UWRUIPanel_DanceMode>(EWRUIPanelKind::DanceMode);
			if (Panel_DanceMode && CurDanceTable)
			{
				Panel_DanceMode->SetFunctionType(UWRUIPanel_DanceMode::EWRFunctionType::None);
			}


			SetNextStepTimer();
		}
	}
}

void UWRActorComponentDanceModeForMine::SetDanceStep(const WRTableID IN InDanceTID)
{
	FWRDanceTable* DanceTable = WRTableManager::Get()->FindRow<FWRDanceTable>(InDanceTID);
	if (DanceTable == nullptr)
	{
		StopDanceMode();
		return;
	}

	CurDanceTable = DanceTable;

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter == nullptr)
		return;

	WRFSMInfo FSMInfo;
	FSMInfo.FSMType = EWRFSM::Dance;
	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(OwnerCharacter);
	if (MyCharacter && MyCharacter->IsShowing3rdCharacter())
	{
		FSMInfo.MontageTID = DanceTable->BaseSubMontageTID;
		FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(DanceTable->BaseSubMontageName);
	}
	else
	{
		FSMInfo.MontageTID = DanceTable->BaseMontageTID;
		FSMInfo.JumpToSectionName = WRUIUtility::ConvertFNameToFString(DanceTable->BaseMontageName);
	}

	if (OwnerCharacter->GetCurrentFSMType() == EWRFSM::Dance)
	{
		OwnerCharacter->PlayAnimJumpToSection(EWRFSM::Dance, FSMInfo.JumpToSectionName);
	}
	else
	{
		OwnerCharacter->DetermineFSMSignal(FSMInfo);
	}

	// [ 2020-3-12 : kyu ] 댄스 시 임시장비 버림
	WRItemManager::Get()->ThrowTemporaryItemAway(EWRReasonForThrowTemporaryItemAway::Dance);

	SetNextStepTimer();

	if (CurDanceTable->StartSound.IsNone() == false)
		WRSFXManager::Get()->SpawnAttached(WRUIUtility::ConvertFNameToFString(CurDanceTable->StartSound), OwnerCharacter->GetRootComponent());

	if (CurDanceTable->StartFX.IsNone() == false)
	{
		WRVFXManager::Get()->Spawn(WRUIUtility::ConvertFNameToFString(CurDanceTable->StartFX), OwnerCharacter, nullptr);
	}

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::DanceModeBoard))
	{
		UWRUIPanel_DanceModeBoard* Panel_DanceModeBoard = WRUIManager::Get()->FindUI< UWRUIPanel_DanceModeBoard>(EWRUIPanelKind::DanceModeBoard);
		if (Panel_DanceModeBoard)
		{
			/*UTexture2D* StartTexture = LoadObject<UTexture2D>(nullptr, *WRUIUtility::ConvertFNameToFString(CurDanceTable->StartImg));
			UTexture2D* KeepTexture = LoadObject<UTexture2D>(nullptr, *WRUIUtility::ConvertFNameToFString(CurDanceTable->KeepImg));
			Panel_DanceModeBoard->SetData(StartTexture, KeepTexture);*/
			Panel_DanceModeBoard->SetData(WRUIUtility::ConvertFNameToFString(CurDanceTable->StartImg), WRUIUtility::ConvertFNameToFString(CurDanceTable->KeepImg));
		}
	}
	else
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (CurDanceTable == nullptr)
			{
				WRUIManager::Get()->CloseUI(EWRUIPanelKind::DanceModeBoard);
				return;
			}

			if (InCombinedPanelData.Is3DWidget())
			{
				UWRUIPanel_DanceModeBoard* Panel_DanceModeBoard = Cast<UWRUIPanel_DanceModeBoard>(InCombinedPanelData.UIPanel);
				if (Panel_DanceModeBoard)
				{
					/*UTexture2D* StartTexture = LoadObject<UTexture2D>(nullptr, *WRUIUtility::ConvertFNameToFString(CurDanceTable->StartImg));
					UTexture2D* KeepTexture = LoadObject<UTexture2D>(nullptr, *WRUIUtility::ConvertFNameToFString(CurDanceTable->KeepImg));
					Panel_DanceModeBoard->SetData(StartTexture, KeepTexture);*/
					Panel_DanceModeBoard->SetData(WRUIUtility::ConvertFNameToFString(CurDanceTable->StartImg), WRUIUtility::ConvertFNameToFString(CurDanceTable->KeepImg));
				}
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::DanceModeBoard, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::DanceModeBoard);
		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::DanceModeBoard, FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector), EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	}
}


void UWRActorComponentDanceModeForMine::SetNextStepTimer()
{
	if (CurDanceTable == nullptr)
	{
		NextStepType = EWRNextStepType::None;
		FlowTime = 0.f;
		NextStepStartTime = 0.f;
		NextStepEndTime = 0.f;
		return;
	}

	if (CurDanceTable->InputKey == EWRInput::None)
	{
		NextStepType = EWRNextStepType::None;
		FlowTime = 0.f;
		NextStepStartTime = 0.f;
		NextStepEndTime = 0.f;
		return;
	}

	if (NextStepType == EWRNextStepType::End)
		FlowTime = FlowTime - NextStepStartTime;

	int32 Scope = CurDanceTable->InputCallMaxTime - CurDanceTable->InputCallMinTime;
	if(Scope == 0)
		NextStepStartTime = (CurDanceTable->InputCallMaxTime) / 1000.f;
	else
		NextStepStartTime = (CurDanceTable->InputCallMaxTime + FMath::Rand() % Scope) / 1000.f;
	NextStepEndTime = NextStepStartTime + (CurDanceTable->InputLimiteTime) / 1000.f;

	NextStepType = EWRNextStepType::Start;
}

bool UWRActorComponentDanceModeForMine::IsEnterDanceArea()
{
	if (bEnterDanceAreaByVolume == true)
		return true;

	if (CheckDancerAround())
		return true;

	if (CheckDancingPlayerAround())
		return true;

	return false;
}

bool UWRActorComponentDanceModeForMine::CheckDancerAround()
{
	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(GetOwnerCharacter());
	if (MyCharacter == nullptr)
		return false;

	FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(MyCharacter->GetCharacterTableID());
	if (PlayerTable == nullptr)
		return false;

	TDoubleLinkedList<AWRCharacterNPC*> NPCCharacters;
	WRCharacterManager::Get()->FindByNpcJobType(EWRNpcJobType::Dancer, NPCCharacters);

	bool bDancerAround = false;
	for (auto& Element : NPCCharacters)
	{
		AWRCharacterNPC* NPCCharacter = Element;
		if (NPCCharacter == nullptr)
			continue;

		float Dist = WRActorUtility::GetDistance(MyCharacter, NPCCharacter);
		if (Dist > PlayerTable->DanceRange)
			continue;

		return true;
	}

	return false;
}

bool UWRActorComponentDanceModeForMine::CheckDancingPlayerAround()
{
	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(GetOwnerCharacter());
	if (MyCharacter == nullptr)
		return false;

	FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(MyCharacter->GetCharacterTableID());
	if (PlayerTable == nullptr)
		return false;

	TArray<AWRCharacter*> OutPlayerCharacter;
	WRCharacterManager::Get()->GetCharactersByType(EWRCharacterType::Player, OutPlayerCharacter);

	bool bDancerAround = false;
	for (auto& Element : OutPlayerCharacter)
	{
		AWRCharacter* PlayerCharacter = Element;
		if (PlayerCharacter == nullptr)
			continue;

		if(PlayerCharacter->IsMe())
			continue;

		float Dist = WRActorUtility::GetDistance(MyCharacter, PlayerCharacter);
		if (Dist > PlayerTable->DanceRange)
			continue;

		UWRActorComponentDanceMode* DanceModeComponent = PlayerCharacter->GetComponent<UWRActorComponentDanceMode>();
		if(DanceModeComponent == nullptr)
			continue;

		if(DanceModeComponent->IsDanceModeOn() == false)
			continue;

		return true;
	}

	return false;
}

void UWRActorComponentDanceModeForMine::SetEnterDanceAreaByVolume(const bool IN bInEnterDanceAreaForVolume)
{
	bEnterDanceAreaByVolume = bInEnterDanceAreaForVolume;

	if (bEnterDanceAreaByVolume == false && CheckDancerAround() == false)
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::DanceMode);
	}

	// + UI Exception
	if (bEnterDanceAreaByVolume == false && CheckDancerAround() == false)
	{
		// 댄스모드 해제
		if (WRCheersManager::Get()->AttemptToOpenCheersContents())
		{
			// [ 2020-4-20 : kyu ] 건배상태 체크 및 설정
		}
	}
	else
	{
		// 댄스모드 설정
		if (WRCheersManager::Get()->PauseCheersContents())
		{
			// [ 2020-4-20 : kyu ] 건배상태 체크 및 해제
		}
	}	
}
