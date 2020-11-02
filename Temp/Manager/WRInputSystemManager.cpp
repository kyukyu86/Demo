// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRInputSystemManager.h"
#include "WRCharacterManager.h"
#include "WRConsoleManager.h"
#include "WRObjectInteractionManager.h"
#include "WRDialogueManager.h"
#include "WRQuestManager.h"
#include "WRInventoryManager.h"
#include "WRCombatManager.h"
#include "WRUIManager.h"
#include "WRTutorialManager.h"
#include "WRCompassManager.h"
#include "WRPopupManager.h"
#include "WRUIManager.h"
#include "WRCheersManager.h"
#include "WRChatBubbleManager.h"
#include "WRDoubleClickManager.h"
#include "WRTriggerManager.h"
#include "WRSFXManager.h"
#include "IConsoleManager.h"
#include "WRNoticeManager.h"
#include "WREnvInteractionManager.h"
#include "WRVoiceContentsManager.h"
#include "WRItemManager.h"

#include "Component/Character/WRActorComponentDynamicMovement.h"
#include "Component/Character/WRActorComponentDanceMode.h"
#include "Component/Character/WRActorComponentAttack.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacterIO.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Actor/Animation/WRAnimInstance.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRForceFeedbackTable.h"
#include "Table/WRVoiceRecognitionTable.h"
#include "Table/WRDynamicMovementTable.h"
#include "Table/WRCoordinateTable.h"
#include "Table/WRCustomTextTable.h"

#include "Utility/WRUIUtility.h"
#include "UI/Panel/WRUIPanel_Popup.h"
#include "UI/Panel/WRUIPanel_SocialMenu.h"
#include "UI/Panel/WRUIPanel_DanceMode.h"
#include "UI/Panel/WRUIPanel_Main.h"
#include "UI/Panel/WRUIPanel_SelectDialogue.h"
#include "UI/Panel/WRUIPanel_Menu.h"
#include "UI/Panel/WRUIPanel_CostumeChange.h"

#include "Network/Share/Define/Define_BluePrint.h"
#include "Network/IMMessageSender.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/WRGameModeBase.h"
#include "GameFramework/InputSettings.h"
#include "Sound/SoundSubmix.h"
#include "SingletonClass/WRSingleton.h"
#include "CombatSystem/Combat/WRCombatInfo.h"
#include "Core/WRGameInstance.h"
#include "Define/WRPathDefine.h"

#include <HeadMountedDisplayFunctionLibrary.h>
#include <WidgetInteractionComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Engine/Engine.h>
#include <JsonReader.h>
#include <JsonObject.h>
#include <JsonSerializer.h>
#include <WebSockets/Public/WebSocketsModule.h>
#include <IWebSocket.h>
#include <EngineUtils.h>
#include <GameFramework/Character.h>
#include <Camera/CameraComponent.h>
#if PLATFORM_WINDOWS
#include <time.h>
#pragma warning(disable:4996)
#endif
#include "Component/Character/WRActorComponentCommonNpcTalk.h"
#include "Component/Character/WRActorComponentPlayerSocial.h"
#include "CustomAsset/WRPlay360VideoAsset.h"
#include "Component/Character/WRActorComponentNpcSocial.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "VoiceInterface.h"
#include "Utility/WRActorUtility.h"
#include "Table/WRPlayerTable.h"
#include "Matrix.h"
#include "WRVehicleManager.h"
#include "WRInteriorManager.h"
#include "Component/Character/WRActorComponentVoiceChatAudio.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WRVFXManager.h"
#include "Actor/Effect/WRVFX.h"
#include "Component/Character/WRActorComponentInteraction.h"




// For Cpp
DEFINE_LOG_CATEGORY(WRInputSystem)

#define ECC_TargettingTrace ECC_GameTraceChannel14
// #define ECC_WRCharacterOther ECC_GameTraceChannel5
// #define ECC_WRIOCollision ECC_GameTraceChannel6

#define DEFERRED_MOTION_COUNT 5
#define BYTE_RATE   32000
#define DEF_CUSTOM_TEXT_TID_FOR_BLACK_OUTIN_ON 15
#define DEF_CUSTOM_TEXT_TID_FOR_BLACK_OUTIN_OFF 16

DECLARE_DELEGATE_OneParam(FWRBindingKeyInputDelegate, EInputEvent);



void WRHMDController::Init()
{

}

void WRHMDController::OnShutdown()
{

}

bool WRHMDController::OnTick(float InDeltaTime)
{
	if (TickFlag & EWRHMDTick::HMDInfo)
		GetHMDLocationAndRotation();

	if (TickFlag & EWRHMDTick::Targetting)
		TargettingProcess();

	return true;
}


AActor* WRHMDController::GetLookAtActor()
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();

	if (MyChar != nullptr)
	{
		FHitResult Hit(1.0f);

		// [ 2019-8-14 : hansang88 ] 콘솔 변수로 변경
		float RayLength = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingRayLength);

		UCameraComponent* CamComponet = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
		if (CamComponet == nullptr)
			return nullptr;

		FVector HMDForward = CamComponet->GetForwardVector();
		FVector StartLocation = CamComponet->GetComponentLocation();
		FVector EndLocation = StartLocation + (HMDForward * RayLength);
		//float	SphereRadius = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingSphereRadius);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(MyChar);

		TArray<AWRCharacter*> DeadCharacters = WRCharacterManager::Get()->GetDeadCharacters();
		for(int i = 0; i < DeadCharacters.Num(); ++i)
			CollisionParams.AddIgnoredActor(DeadCharacters[i]);

		
		if (MyChar->GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_TargettingTrace, CollisionParams) == true)
		//if (MyChar->GetWorld()->SweepSingleByObjectType(Hit, StartLocation, EndLocation, FQuat::Identity, ObjectParams, FCollisionShape::MakeSphere(SphereRadius), Params))
		{
// 			FString ActorName = Hit.Actor->GetName();
// 			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, ActorName);
			return Hit.Actor.Get();
		}
	}
	
	return nullptr;
}

FVector WRHMDController::GetHMDForwardVector()
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if(MyChar == nullptr)
		return FVector::ZeroVector;

	UCameraComponent* CamComponet = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
	if (CamComponet == nullptr)
		return FVector::ZeroVector;

	return CamComponet->GetForwardVector(); 
}

FVector WRHMDController::GetHMDRightVector()
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar == nullptr)
		return FVector::ZeroVector;

	UCameraComponent* CamComponet = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
	if (CamComponet == nullptr)
		return FVector::ZeroVector;

	return CamComponet->GetRightVector();
}

void WRHMDController::TargettingProcess()
{
	ACharacter* LookAtChar = Cast<ACharacter>(GetLookAtActor());
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	// [ 2019-8-27 : hansang88 ] 일단 기획이 없으므로 임의로 작성
	if (bMultiTargetMode == true)
	{
		//int MultiTargetMax = WRConsoleManager::Get()->GetInt(EWRConsole::HMD_TargettingCountMax);
		if (MaxMultiCount > TargetActors.Num())
		{
			if (nullptr == LookAtChar)
				return;

			//적대적 대상이 아니면 패스
			if (pMe->GetRelationToTarget(Cast<AWRCharacter>(LookAtChar)) != EWRFactionRelation::Hostile)
				return;

			if (TargetActors.Contains(LookAtChar) == false)
			{
				TargetActors.Add(LookAtChar);
				// [ 2019-8-27 : hansang88 ] UI 표현을 위한 Target Add 노티 필요
				AWRCharacter* CastCharacter = Cast<AWRCharacter>(LookAtChar);
				if (CastCharacter != nullptr)
					CastCharacter->OnTargeted(true);
			}
		}
	}
	else
	{
		if (bLockOnMode == false)
		{
			if (TargetActor != LookAtChar)
			{
				TargetActor = LookAtChar;
				HMDTargetChangedDelegate.Broadcast(LookAtChar);
			}
		}
		else
		{
			if (TargetActor)
			{
				float HoldingTime = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingHoldingTime);
				bool bRenderd = TargetActor->WasRecentlyRendered(HoldingTime);
				if (bRenderd == false)
					SetTargetLock(false);
			}
		}
	}
}

#define CHECK_MOVEMENT_LOCATION_VALUE 5.0f
#define CHECK_MOVEMENT_ROTATION_VALUE 2.0f
void WRHMDController::GetHMDLocationAndRotation()
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar != nullptr)
	{
		FVector VRTrackingLocation;
		FRotator VRTrackingRotation;

		PreHMDLocation = HMDLocation;
		PreHMDRotation = HMDRotation;

		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == true && UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
		{
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(VRTrackingRotation, VRTrackingLocation);
			FTransform TrackingToWorld = UHeadMountedDisplayFunctionLibrary::GetTrackingToWorldTransform(MyChar);
			HMDLocation = TrackingToWorld.TransformPosition(VRTrackingLocation);
			HMDRotation = VRTrackingRotation;
		}
		else
		{
			UCameraComponent* CameraComponent = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
			if (CameraComponent != nullptr)
			{
				HMDLocation = CameraComponent->GetComponentLocation();
				HMDRotation = CameraComponent->GetComponentRotation();
			}
		}

		// [ 2020-3-10 : magedoga ] check movement
		float HMDLocationSize = HMDLocation.Size();
		float PreHMDLocationSize = PreHMDLocation.Size();
		bMovedLocation = (FMath::Abs(HMDLocationSize - PreHMDLocationSize) > CHECK_MOVEMENT_LOCATION_VALUE);
		//bMovedLocation = (FMath::Abs(HMDLocation.Size() - PreHMDLocation.Size()) > CHECK_MOVEMENT_LOCATION_VALUE);

		FRotator SubtractRotator = (PreHMDRotation -= HMDRotation);
		bRotated = (FMath::Abs(SubtractRotator.Yaw) > CHECK_MOVEMENT_ROTATION_VALUE ||
					FMath::Abs(SubtractRotator.Pitch) > CHECK_MOVEMENT_ROTATION_VALUE ||
					FMath::Abs(SubtractRotator.Roll) > CHECK_MOVEMENT_ROTATION_VALUE);

// 		if (bMovedLocation || bRotated)
// 		{
// 			UE_LOG(WRInputSystem, Display, TEXT("WRHMDController::GetHMDLocationAndRotation - [Location : %d] / [Rotation : %d]"), bMovedLocation, bRotated);
// 		}
	}
}

void WRHMDController::StartMultiTargetMode()
{
	TargetActors.Empty();
	bMultiTargetMode = true;
	SetTargetLock(false);

	TargetActor = nullptr;
	HMDTargetChangedDelegate.Broadcast(nullptr);
}

void WRHMDController::EndMultiTargetMode()
{
	bMultiTargetMode = false;
	MaxMultiCount = 0;

	for (auto& Iter : TargetActors)
	{
		AWRCharacter* CastCharacter = Cast<AWRCharacter>(Iter);
		if(CastCharacter == nullptr)
			continue;

		CastCharacter->ReleasedTarget(true);
	}
	//return TargetActors;

	TargetActors.Empty();
}

void WRHMDController::ActionMultiTargetMode()
{
	if(TargetActors.Num() > 0)
		HMDMultiTargetActionDelegate.Broadcast(TargetActors);

	EndMultiTargetMode();
}

bool WRHMDController::IsTargetActor(const AActor* IN InActor)
{
	if (TargetActor->IsValidLowLevel() == false)
		return false;

	if (TargetActor == InActor)
		return true;

	return false;
}

void WRHMDController::SetTargetLock(const bool IN bInLock)
{
	if (bLockOnMode == bInLock)
		return;

	bLockOnMode = bInLock;
	if (TargetActor != nullptr)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter != nullptr)
		{
			MyCharacter->OnTargetLockChanged(bLockOnMode, TargetActor);
		}
	}
}

void WRHMDController::ResetOrientation()
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter->IsValidLowLevel() == false)
	{
		UE_LOG(WRInputSystem, Display, TEXT("WRHMDController::ResetOrientation - Null MyCharacter"));
		return;
	}

	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(MyCharacter->GetActorRotation().Yaw);
}

bool WRHMDController::GetSweepSingleByChannelUsingHMD(FVector& InoutLocation)
{
	UWorld* pWorld = UWRGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return false;

	FHitResult Hit(1.0f);
	FCollisionQueryParams Params(FName(TEXT("HMDPickingRay")), false);

	//무시할 액터들 구해오기
	TArray<AActor*> IgnoreActors;
	//character
	WRCharacterManager::Get()->GetAllActor(IgnoreActors);
	//vehicle
	WRVehicleManager::Get()->GetAllVehicle(IgnoreActors);
	//add to param
	Params.AddIgnoredActors(IgnoreActors);

	static float aa = 200.0f;
	static float TestDistance = 5000.0f;
#ifdef UE_EDITOR
//	UKismetSystemLibrary::DrawDebugLine(pWorld, HMDLocation + GetHMDForwardVector() * 100.0f, HMDLocation + GetHMDForwardVector() * TestDistance, FLinearColor::Red, 0.f, 1.0f);
#endif // DEF_YUL_TEST

	FVector BoxSize = FVector(1.0f, 1.0f, 1.0f);
	if (pWorld->SweepSingleByChannel(Hit, HMDLocation, HMDLocation + GetHMDForwardVector() * TestDistance, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, FCollisionShape::MakeBox(BoxSize), Params))
	{
		FVector LocationOnNav = WRActorUtility::ProjectPointToNavigationNotExtent(Hit.Location);
#ifdef UE_EDITOR
		//UKismetSystemLibrary::DrawDebugSphere(pWorld, LocationOnNav, 50.0f, 12, FLinearColor::Yellow);
#endif // DEF_YUL_TEST

		if (Hit.Location.Z + 50.0f <= LocationOnNav.Z)
		{
			InoutLocation = FVector::ZeroVector;
		}
		else
		{
			InoutLocation = LocationOnNav;
		}
	}
	else
	{
		FVector DestLocation = HMDLocation + GetHMDForwardVector() * TestDistance;
		InoutLocation = WRActorUtility::ProjectPointToNavigationNotExtent(DestLocation);
	}

#ifdef UE_EDITOR
	//UKismetSystemLibrary::DrawDebugSphere(pWorld, InoutLocation, 50.0f, 12, FLinearColor::Blue);
#endif // DEF_YUL_TEST

	return true;
}

//========================================================================================================================================================================

void UWRPadController::Init()
{
	MotionValueZ[EWRPadMotionType::PULL] = WRConsoleManager::Get()->GetFloat(EWRConsole::PAD_PullMotionValueZ);
	MotionValueY[EWRPadMotionType::PULL] = WRConsoleManager::Get()->GetFloat(EWRConsole::PAD_PullMotionValueY);

	MotionValueZ[EWRPadMotionType::PUSH] = WRConsoleManager::Get()->GetFloat(EWRConsole::PAD_PushMotionValueZ);
	MotionValueY[EWRPadMotionType::PUSH] = WRConsoleManager::Get()->GetFloat(EWRConsole::PAD_PushMotionValueY);

	StartTouchPos = FVector2D::ZeroVector;
	PreTouchPos = FVector2D::ZeroVector;
	TouchDragDistance = FVector2D::ZeroVector;

 	for (int i = 0; i < EWRPadMotionType::MAX; ++i)
 	{
 		DeferredCount[i] = 0;
 	}
}

void UWRPadController::OnShutdown()
{

}

bool UWRPadController::OnTick(float InDeltaTime)
{
	if (TouchDragDistance != FVector2D::ZeroVector)
	{
		WRInputSystemManager::Get()->InputEventTouchPadDrag(TouchDragDistance);
		TouchDragDistance = FVector2D::ZeroVector;
	}

	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetWorld()->GetFirstPlayerController();
	if (PlayerController != nullptr)
	{
		FVector Empty;
		FVector Gravity;
		FVector Acc;
		PlayerController->GetInputMotionState(Empty, Empty, Gravity, Acc);
		FVector OutFrontDir;
		FVector OutRightDir;
		PlayerController->GetAudioListenerPosition(ListenerPos, OutFrontDir, OutRightDir);
		
		bool bCurrentMotion[EWRPadMotionType::MAX] = { false, };

		if (FMath::Abs(Acc.Y) > MotionValueY[EWRPadMotionType::PULL] && Acc.Z > MotionValueZ[EWRPadMotionType::PULL])
		{
			bMotion[EWRPadMotionType::PULL] = true;
			bCurrentMotion[EWRPadMotionType::PULL] = true;
		}

		if (FMath::Abs(Acc.Y) > MotionValueY[EWRPadMotionType::PUSH] && Acc.Z < MotionValueZ[EWRPadMotionType::PUSH])
		{
			bMotion[EWRPadMotionType::PUSH] = true;
			bCurrentMotion[EWRPadMotionType::PUSH] = true;
		}

		// [ 2019-11-26 : hansang88 ] DEFERRED_MOTION_COUNT 동안 지연 처리 하며 첫 모션 입력후 지연 프레임동안 들어온 입력 무시
		for (int i = 0; i < EWRPadMotionType::MAX; ++i)
		{
			if (bMotion[i] == true && bCurrentMotion[i] == false)
			{
				if (DeferredCount[i] > DEFERRED_MOTION_COUNT)
				{
					bMotion[i] = false;
					DeferredCount[i] = 0;
					WRInputSystemManager::Get()->InputEventMotion((EWRPadMotionType)i);
				}
				else
				{
					++DeferredCount[i];
					for (int j = 0; j < EWRPadMotionType::MAX; ++j)
					{
						if (j != i)
						{
							if (bMotion[j] == true)
							{
								if (DeferredCount[i] <= DeferredCount[j])
								{
									DeferredCount[i] = 0;
									bMotion[i] = false;
								}
								else
								{
									DeferredCount[j] = 0;
									bMotion[j] = false;
								}
							}
						}
					}
				}
			}
		}
	}

	return true;
}

void UWRPadController::InputKeyEvent(EWRInput InInputType, EInputEvent InEventType)
{
	WRInputSystemManager::Get()->InputEvent(InInputType, InEventType);
}

void UWRPadController::TouchPadX(float InScale)
{
	if (InScale != 0)
	{
		if (StartTouchPos.X == 0)
		{
			StartTouchPos.X = InScale;
		}
		else
		{
			PreTouchPos.X = InScale;
		}
	}
	else
	{
		if (StartTouchPos.X != 0)
		{
			if (PreTouchPos.X != 0)
				TouchDragDistance.X = PreTouchPos.X - StartTouchPos.X;

			StartTouchPos.X = 0;
			PreTouchPos.X = 0;
		}
	}
}

void UWRPadController::TouchPadY(float InScale)
{
	if (InScale != 0)
	{
		if (StartTouchPos.Y == 0)
		{
			StartTouchPos.Y = InScale;
		}
		else
		{
			PreTouchPos.Y = InScale;
		}
	}
	else
	{
		if (StartTouchPos.Y != 0)
		{
			if(PreTouchPos.Y != 0)
				TouchDragDistance.Y = PreTouchPos.Y - StartTouchPos.Y;

			StartTouchPos.Y = 0;
			PreTouchPos.Y = 0;
		}
	}
}

void UWRPadController::InputEventLStickX(float IN InScale)
{
	WRInputSystemManager::Get()->InputEventLStickX(InScale);
}

void UWRPadController::InputEventLStickY(float IN InScale)
{
	WRInputSystemManager::Get()->InputEventLStickY(InScale);
}

#if WITH_EDITOR
void UWRPadController::KyuInputTest()
{
//	WRPopupManager::Get()->EventCalled();
}
void UWRPadController::Kyu_L1_Pressed()
{
	bL1 = true;
	nL2 = 0;
}
void UWRPadController::Kyu_L1_Released()
{
	bL1 = false;
	nL2 = 0;
}
void UWRPadController::Kyu_L2_Pressed()
{
	++nL2;
}
void UWRPadController::Kyu_L2_Released()
{
	return;

	if (bL1 && nL2 == 2)
	{
		AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
		if (MyChar->IsValidLowLevel() == true)
		{
			UWidgetInteractionComponent* WidgetInteractionComponent = Cast<UWidgetInteractionComponent>(MyChar->GetComponentByClass(UWidgetInteractionComponent::StaticClass()));
			if (WidgetInteractionComponent != nullptr)
			{
				WidgetInteractionComponent->bShowDebug = !WidgetInteractionComponent->bShowDebug;
			}
		}
	}
}

void UWRPadController::InputEventLShiftBtn(EInputEvent InEventType)
{
	WRInputSystemManager::Get()->InputEventLShiftBtn(InEventType);
}

void UWRPadController::PressYsyTest()
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	AID_t TargetAID = MyCharacter->GetTargetAID();
	if (TargetAID == INVALID_AID)
		return;

	IMMessageSender::RequestSocialAction(99999, TargetAID);
}

void UWRPadController::KorLanPressed()
{
	IConsoleVariable* ConsoleVariable = WRConsoleManager::Get()->GetConsoleVariable(EWRConsole::LanguageType);

	if (ConsoleVariable == nullptr)
		return;

	ConsoleVariable->Set(int32(EWRLanguageType::Korean), EConsoleVariableFlags::ECVF_SetByConsole);
}

void UWRPadController::JpnLanPressed()
{
	IConsoleVariable* ConsoleVariable = WRConsoleManager::Get()->GetConsoleVariable(EWRConsole::LanguageType);

	if (ConsoleVariable == nullptr)
		return;

	ConsoleVariable->Set(int32(EWRLanguageType::Japanese), EConsoleVariableFlags::ECVF_SetByConsole);
}

void UWRPadController::ChnLanPressed()
{
	IConsoleVariable* ConsoleVariable = WRConsoleManager::Get()->GetConsoleVariable(EWRConsole::LanguageType);

	if (ConsoleVariable == nullptr)
		return;

	ConsoleVariable->Set(int32(EWRLanguageType::Chinese), EConsoleVariableFlags::ECVF_SetByConsole);
}

void UWRPadController::EngLanPressed()
{
	IConsoleVariable* ConsoleVariable = WRConsoleManager::Get()->GetConsoleVariable(EWRConsole::LanguageType);

	if (ConsoleVariable == nullptr)
		return;

	ConsoleVariable->Set(int32(EWRLanguageType::English), EConsoleVariableFlags::ECVF_SetByConsole);
}

void UWRPadController::JinTestPressed()
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	FString MoviePath;

	bool bUse3D = WRConsoleManager::Get()->GetBool(EWRConsole::Test_360Movie_3DUse);
	if (bUse3D)
	{
		MoviePath = "/Game/Asset/Data/Asset/360Movie/Sample3D360MovieAsset.Sample3D360MovieAsset";
	}
	else
	{
		MoviePath = "/Game/Asset/Data/Asset/360Movie/Sample360MovieAsset.Sample360MovieAsset";
	}

	FStringAssetReference Play360AssetSTR = FStringAssetReference(MoviePath);
	MyCharacter->Play360Movie(Cast<UWRPlay360VideoAsset>(Play360AssetSTR.TryLoad()), bUse3D);
}

void UWRPadController::JinTestPressed2()
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	MyCharacter->Stop360Movie();
}

#else // WITH_EDITOR
#endif // WITH_EDITOR

//========================================================================================================================================================================



//========================================================================================================================================================================
WRInputSystemManager::WRInputSystemManager()
{
}

WRInputSystemManager::~WRInputSystemManager()
{
}
	
// [ 2019-8-14 : hansang88 ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRInputSystemManager::OnInitialize()
{
	char* c_path = TCHAR_TO_UTF8(*FPaths::ProjectLogDir());
#ifdef PLATFORM_WINDOWS
	time_t timer = time(NULL);
	struct tm *t = localtime(&timer);

	sprintf(VoiceLogs, "%svoice_log_%2d%02d%02d-%02d%02d%02d.txt", c_path, t->tm_year - 100, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
#else
	strcpy(VoiceLogs, "%svoice_log.txt", c_path);
#endif

	if (PadController == nullptr)
	{
		PadController = NewObject<UWRPadController>();
		PadController->AddToRoot();
		PadController->Init();
	}
	if (VoiceController == nullptr)
	{
		VoiceController = NewObject<UWRVoiceController>();
		VoiceController->AddToRoot();
		VoiceController->Init();
	}

	HMDController.Init();

#if PLATFORM_PS4
#if WR_ADD_VOICE_PORT
	IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(UWRGameInstance::GetGameInstance()->GetWorld());
	if (PS4Subsystem != nullptr)
	{
		IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();
		if (VoiceInt.IsValid() == true)
		{
			VoiceInt->Init3DVoice();

			for (int i = 0; i < IN_PROT_COUNT; ++i)
			{
				FString EnumName = FString::Printf(TEXT("PS4_Voice_Volume_%d"), i + 1);
				EWRConsole ConsoleEnum = (EWRConsole)WREnumUtility::StringToEnum("EWRConsole", EnumName);
				float VoiceVolume = WRConsoleManager::Get()->GetFloat(ConsoleEnum);
				VoiceInt->SetPortVolume(i, VoiceVolume);
			}	
		}
	}
#endif //PLATFORM_PS4
#endif //WR_ADD_VOICE_PORT
}

// [ 2019-8-14 : hansang88 ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRInputSystemManager::OnShutdown()
{
	if (PadController)
	{
		PadController->OnShutdown();
		PadController->RemoveFromRoot();
		PadController = nullptr;
	}

	if (VoiceController)
	{
		VoiceController->OnShutdown();
		VoiceController->RemoveFromRoot();
		VoiceController = nullptr;
	}

	HMDController.OnShutdown();

	RemoveHMDPickingVFX();
}

// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRInputSystemManager::OnLoadLevelStart()
{
	HMDController.SetTargetLock(false);

	bUpdateHMDPicking = false;
	RemoveHMDPickingVFX();
}

// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRInputSystemManager::OnLoadLevelComplete()
{
	
}

// [ 2019-8-14 : hansang88 ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRInputSystemManager::OnRenderStart()
{
	
}

void WRInputSystemManager::EndLoadLevelProcess()
{
	// [ 2019-12-10 : animal14 ] 그리드 연출 시작하면 인풋 잠금 해제
	// [ 2020-03-26 : firefirefox ] 연구실에서는 움직임을 계속 막아둠
	AWRGameModeBase* GameModeBase = Cast<AWRGameModeBase>(UGameplayStatics::GetGameMode(UWRGameInstance::GetGameInstance()));
	if (GameModeBase && GameModeBase->GetGameModeType() == EWRGameMode::Lab)
	{
		WRInputSystemManager::Get()->SetLocationLock(true);
	}
	else
	{
		WRInputSystemManager::Get()->SetLocationLock(false);
	}
}

void WRInputSystemManager::SetUpInputBind(UInputComponent* PlayerInputComponent)
{
	InputMaps.Add(TEXT("Triangle"), EWRInput::Triangle);
	InputMaps.Add(TEXT("Square"), EWRInput::Square);
	InputMaps.Add(TEXT("Circle"), EWRInput::Circle);
	InputMaps.Add(TEXT("Cross"), EWRInput::Cross);
	InputMaps.Add(TEXT("DpadUP"), EWRInput::DpadUP);
	InputMaps.Add(TEXT("DpadDown"), EWRInput::DpadDown);
	InputMaps.Add(TEXT("DpadLeft"), EWRInput::DpadLeft);
	InputMaps.Add(TEXT("DpadRight"), EWRInput::DpadRight);
	InputMaps.Add(TEXT("L1"), EWRInput::L1);
	InputMaps.Add(TEXT("L2"), EWRInput::L2);
	InputMaps.Add(TEXT("L3"), EWRInput::L3);
	InputMaps.Add(TEXT("R1"), EWRInput::R1);
	InputMaps.Add(TEXT("R2"), EWRInput::R2);
	InputMaps.Add(TEXT("R3"), EWRInput::R3);
	InputMaps.Add(TEXT("OptionBtn"), EWRInput::Option);
	InputMaps.Add(TEXT("ShareBtn"), EWRInput::Share);
	InputMaps.Add(TEXT("TouchPadClick"), EWRInput::TouchPadClick);
	InputMaps.Add(TEXT("PullMotion"), EWRInput::JoystickPull);
	InputMaps.Add(TEXT("PushMotion"), EWRInput::JoystickPush);
	InputMaps.Add(TEXT("InteractionSkip"), EWRInput::InteractionSkip);
	InputMaps.Add(TEXT("RStickRight"), EWRInput::RStickRight);
	InputMaps.Add(TEXT("RStickLeft"), EWRInput::RStickLeft);
	InputMaps.Add(TEXT("RStickUp"), EWRInput::RStickUp);
	InputMaps.Add(TEXT("RStickDown"), EWRInput::RStickDown);
	InputMaps.Add(TEXT("RStickX"), EWRInput::RStickX);
	InputMaps.Add(TEXT("RStickY"), EWRInput::RStickY);
	InputMaps.Add(TEXT("LStickRight"), EWRInput::LStickRight);
	InputMaps.Add(TEXT("LStickLeft"), EWRInput::LStickLeft);
	InputMaps.Add(TEXT("LStickUp"), EWRInput::LStickUp);
	InputMaps.Add(TEXT("LStickDown"), EWRInput::LStickDown);
	InputMaps.Add(TEXT("LStickX"), EWRInput::LStickX);
	InputMaps.Add(TEXT("LStickY"), EWRInput::LStickY);
	

	PlayerInputComponent->BindAxis(TEXT("TouchPadX"), PadController, &UWRPadController::TouchPadX);			// [ 2019-10-23 : hansang88 ]  
	PlayerInputComponent->BindAxis(TEXT("TouchPadY"), PadController, &UWRPadController::TouchPadY);			// [ 2019-10-23 : hansang88 ]  

	PlayerInputComponent->BindAxis(TEXT("LStick_X"), PadController, &UWRPadController::InputEventLStickX);
	PlayerInputComponent->BindAxis(TEXT("LStick_Y"), PadController, &UWRPadController::InputEventLStickY);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("Triangle"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::Triangle, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("Triangle"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::Triangle, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("Square"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::Square, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("Square"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::Square, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("Circle"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::Circle, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("Circle"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::Circle, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("Cross"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::Cross, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("Cross"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::Cross, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadUP"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadUP, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadUP"), EInputEvent::IE_Repeat, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadUP, EInputEvent::IE_Repeat);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadUP"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadUP, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadDown"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadDown, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadDown"), EInputEvent::IE_Repeat, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadDown, EInputEvent::IE_Repeat);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadDown"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadDown, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadLeft"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadLeft, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadLeft"), EInputEvent::IE_Repeat, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadLeft, EInputEvent::IE_Repeat);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadLeft"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadLeft, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadRight"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadRight, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadRight"), EInputEvent::IE_Repeat, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadRight, EInputEvent::IE_Repeat);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("DpadRight"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::DpadRight, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("L1"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::L1, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("L1"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::L1, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("L2"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::L2, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("L2"), EInputEvent::IE_Repeat, PadController, &UWRPadController::InputKeyEvent, EWRInput::L2, EInputEvent::IE_Repeat);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("L2"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::L2, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("L3"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::L3, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("L3"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::L3, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("R1"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::R1, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("R1"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::R1, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("R2"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::R2, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("R2"), EInputEvent::IE_Repeat, PadController, &UWRPadController::InputKeyEvent, EWRInput::R2, EInputEvent::IE_Repeat);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("R2"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::R2, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("R3"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::R3, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("R3"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::R3, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("OptionBtn"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::Option, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("OptionBtn"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::Option, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("ShareBtn"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::Share, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("ShareBtn"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::Share, EInputEvent::IE_Released);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("TouchPadClick"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::TouchPadClick, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("TouchPadClick"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::TouchPadClick, EInputEvent::IE_Released);
		
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("PullMotion"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::JoystickPull, EInputEvent::IE_Pressed);
	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("PushMotion"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputKeyEvent, EWRInput::JoystickPush, EInputEvent::IE_Pressed);

	PlayerInputComponent->BindAction<FWRBindingInputDelegate>(TEXT("InteractionSkip"), EInputEvent::IE_Released, PadController, &UWRPadController::InputKeyEvent, EWRInput::InteractionSkip, EInputEvent::IE_Released);

#if WITH_EDITOR
	// [ 2019-8-20 : kyu ] Input Test
	PlayerInputComponent->BindAction(TEXT("KyuInputTest"), EInputEvent::IE_Pressed, PadController, &UWRPadController::KyuInputTest);	// [ 2019-8-28 : kyu ] Shift + 1
	PlayerInputComponent->BindAction(TEXT("YsyTest"), EInputEvent::IE_Pressed, PadController, &UWRPadController::PressYsyTest);		// [ 2019-8-28 : ysy ] Shift + 2
	PlayerInputComponent->BindAction(TEXT("JinTest"), EInputEvent::IE_Pressed, PadController, &UWRPadController::JinTestPressed);		// [ 2020-1-9 : magedoga ] Alt + [
	PlayerInputComponent->BindAction(TEXT("JinTest2"), EInputEvent::IE_Pressed, PadController, &UWRPadController::JinTestPressed2);		// [ 2020-1-9 : magedoga ] Alt + ]
	PlayerInputComponent->BindAction<FWRBindingKeyInputDelegate>(TEXT("LShift"), EInputEvent::IE_Pressed, PadController, &UWRPadController::InputEventLShiftBtn, EInputEvent::IE_Pressed);		// [ 2020-1-21 : kyu ] L Shift
	PlayerInputComponent->BindAction<FWRBindingKeyInputDelegate>(TEXT("LShift"), EInputEvent::IE_Released, PadController, &UWRPadController::InputEventLShiftBtn, EInputEvent::IE_Released);	// [ 2020-1-21 : kyu ] L Shift
	PlayerInputComponent->BindAction(TEXT("KorLan"), EInputEvent::IE_Pressed, PadController, &UWRPadController::KorLanPressed);	// 국가 타입 변경 CTRL + 1
	PlayerInputComponent->BindAction(TEXT("JpnLan"), EInputEvent::IE_Pressed, PadController, &UWRPadController::JpnLanPressed);	// 국가 타입 변경 CTRL + 2
	PlayerInputComponent->BindAction(TEXT("ChnLan"), EInputEvent::IE_Pressed, PadController, &UWRPadController::ChnLanPressed);	// 국가 타입 변경 CTRL + 3
	PlayerInputComponent->BindAction(TEXT("EngLan"), EInputEvent::IE_Pressed, PadController, &UWRPadController::EngLanPressed);	// 국가 타입 변경 CTRL + 4
#else // WITH_EDITOR
#endif // WITH_EDITOR
}

bool WRInputSystemManager::OnTick(float InDeltaTime)
{
	HMDController.OnTick(InDeltaTime);
	PadController->OnTick(InDeltaTime);
	OnTickProcessAvoiding();
	OnTickHMDPicking();

	if (VoiceEventType != EWRVoiceWordType::NONE)
	{
		WRVoiceContentsManager::Get()->OnDefaultVoiceRecordResulted(VoiceEventType, VoiceCommonDataTID);
		VoiceEventType = EWRVoiceWordType::NONE;
	}	

//	UpdateKeepPressedInputEventTime(InDeltaTime);
	if (VoiceController == nullptr)
	{
		return true;
	}

	VoiceController->OnTick(InDeltaTime);

	return true;
}

void WRInputSystemManager::OnTickProcessAvoiding()
{
	if (FVector::ZeroVector == HMDStartPosition || 
		FRotator::ZeroRotator == HMDStartRotation)
		return;

	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == pMe)
		return;

	float RollGap = GetHMDRotation().Roll - HMDStartRotation.Roll;
	if (0 < UKismetMathLibrary::Abs(RollGap))
	{
		AWRCharacter* Target = WRCharacterManager::Get()->FindbyAID(pMe->GetTargetAID());
		if (nullptr == Target)
			return;

		UWRAnimInstance* TargetAnimInstance = Target->GetAnimInstance();
		if (nullptr == TargetAnimInstance)
			return;

		UWRActorComponentDynamicMovement* DyMoveComponent = pMe->GetComponent<UWRActorComponentDynamicMovement>();
		if (nullptr == DyMoveComponent)
			return;

		//회피가능상태가 아니면 리턴
		if (TargetAnimInstance->IsIncludedCombatGimmickState(EWRCombatGimmickState::AvoidingLeft) == false &&
			TargetAnimInstance->IsIncludedCombatGimmickState(EWRCombatGimmickState::AvoidingRight) == false)
			return;

		//방향 상관없이 회피 가능 상태일때 입력값이 15 이상이면 회피 실행
		if (UKismetMathLibrary::Abs(RollGap) <= 15.0f)
			return;

		//회피 실행
		pMe->DetermineFSMSignal(EWRFSM::Avoiding);
		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(Target->GetActorID());
		if (CombatInfo)
		{
			CombatInfo->RequestToServer_Avoid(pMe, 0);
		}

		//액션무브 실행
		FVector MoveDirection = FVector::ZeroVector;
		if (RollGap < -15.0f)	//왼쪽 입력일때
		{
			MoveDirection = pMe->GetActorRightVector() * -1.0f;
		}
		else if (15.0f < RollGap)	//오른쪽 입력일떄
		{
			MoveDirection = pMe->GetActorRightVector();
		}

		if (FVector::ZeroVector != MoveDirection)
		{
			FWRDynamicMovementTable* DynamicMoveTable = WRTableManager::Get()->FindRow<FWRDynamicMovementTable>(AVOID_TABLE_TID);
			if (DynamicMoveTable)
			{
				FVector DestLocation = (MoveDirection * DynamicMoveTable->Distance) + pMe->GetActorLocation();
				DyMoveComponent->InvokeDynamicMovement(AVOID_TABLE_TID, DestLocation);
			}
		}
	}
}

void WRInputSystemManager::OnTickHMDPicking()
{
	if (bUpdateHMDPicking)
	{
		if (HMDController.GetSweepSingleByChannelUsingHMD(HMDPickingLocation) == false ||
			GetAttackHeightType() == EWRAttackHeightType::Upper)
		{
			HMDPickingLocation = FVector::ZeroVector;
		}

		UpdateHMDPickingVFX();
	}
}

// void WRInputSystemManager::StartMultiTargetMode()
// {
// 
// }
// 
// TArray<ACharacter*> WRInputSystemManager::EndMultiTargetMode()
// {
// 	return HMDController.EndMultiTargetMode();
// }

AActor* WRInputSystemManager::GetLookAtActor()
{
	return HMDController.GetLookAtActor();
}

void WRInputSystemManager::StartRecording(const EWRVoiceContentType InVoiceContentType, TArray<FWRSubCommandData>& subCommand, FWRVoiceEventDelegate callback)
{
	if (VoiceController->IsImpossibleVoiceRecording())
		return;

	if (IsVoiceRecording())
		return;

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter)
	{
		if (WREnvInteractionManager::Get()->IsEnvVehiclePassenger(MyCharacter->GetActorID(), EWREnvType::Tardis))
			return;
	}

	if (WRInputSystemManager::Get()->IsChatRecording())
		WRInputSystemManager::Get()->CancelRecording();
	
	VoiceController->StartRecording(InVoiceContentType, subCommand, callback);
}

void WRInputSystemManager::StartChatRecording()
{
	VoiceController->StartChatRecording();
}

void WRInputSystemManager::StopRecording()
{
	if( VoiceController->IsCapturing() )
		VoiceController->StopRecording();
}

void WRInputSystemManager::CancelRecording()
{
	VoiceController->CancelRecording();
}

void WRInputSystemManager::SetImpossibleVoiceRecording(const bool IN bInImpossibleVoiceRecording)
{
	if (VoiceController)
		VoiceController->SetImpossibleVoiceRecording(bInImpossibleVoiceRecording);
}

void WRInputSystemManager::SetVoiceRecordingState(EWRVoiceRecordingStateType state)
{
	if (VoiceController)
	{
		VoiceController->SetVoiceRecording(state);
	}
}

void WRInputSystemManager::FinishedRecognition(FVoiceRecognition* InFinishedTask)
{
	if (VoiceController->IsValidLowLevel() == true)
	{
		VoiceController->FinishedRecognition(InFinishedTask);
	}
}

void WRInputSystemManager::OnResponseGoogleSTT(FString Str)
{
	if (Str.IsEmpty())
		return;
	
	IMMessageSender::RequestSpeechBubble(EWRLanguageType::en(WRConsoleManager::Get()->GetInt(EWRConsole::LanguageType)), Str);
	
	if (WRConsoleManager::Get()->GetBool(EWRConsole::Debug_ShowChatRecordUI_Speaker))
	{
		WRNoticeManager::Get()->AddStyleNotice(Str);
	}
}

int32 WRInputSystemManager::Translation(EWRLanguageType::en Src, EWRLanguageType::en Tar, FString Str)
{
	return VoiceController->Translation(Src, Tar, Str);
}

void WRInputSystemManager::OnResponseTranslation(int32 Identify, EWRLanguageType::en Src, EWRLanguageType::en Tar, FString OrgStr, FString TarStr)
{
	WRChatBubbleManager::Get()->OnResponseTranslation(Identify, TarStr);
}

void WRInputSystemManager::OnInf_VoiceMessage(const AID_t IN playerAID, const FString IN voiceMsg)
{
	if (VoiceController->IsValidLowLevel() == true)
	{
		VoiceController->OnInf_VoiceMessage(playerAID, voiceMsg);
	}
}

//void WRInputSystemManager::InputVoiceEvent(WRTableID InVoiceCommandDataTID)
//{
//	int32 CheatTID = WRConsoleManager::Get()->GetInt(EWRConsole::Cheat_VoiceTID);
//	if (CheatTID != 0)
//		InVoiceCommandDataTID = CheatTID;
//
//	EWRVoiceWordType EventType = EWRVoiceWordType::UNKNOWN;
//
//	FWRVoiceCommandData* VoiceCommandData = nullptr;
//	if (InVoiceCommandDataTID != INVALID_TABLE_TID)
//	{
//		VoiceCommandData = WRTableManager::Get()->FindRow<FWRVoiceCommandData>(InVoiceCommandDataTID);
//		if (VoiceCommandData)
//			EventType = VoiceCommandData->WordType;
//	}
//
//	
//	FString Text = FString::Printf(TEXT("[WRInputSystemManager::InputVoiceEvent] %d"), EventType);
//	this->DisplayAndSaveLog(Text);
//
//	FWRVoiceEventDialogue* VoiceEventDialogueTable = FWRVoiceEventDialogue::GetVoiceEventDialogueData(EventType);
//
//	if (VoiceCommandData && VoiceCommandData->VolumeCheck && AvailableVoiceCommandTIDs.Contains(InVoiceCommandDataTID) == false)
//	{
//		WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandVolumeImposibbleDialogueTID(VoiceEventDialogueTable);
//		if(DialogueTID != INVALID_TABLE_TID)
//			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//
//		IMMessageSender::RequestEndVoiceCommand();
//		return;
//	}
//
//	switch (EventType)
//	{
//	case EWRVoiceWordType::UNKNOWN:
//	{
//		WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//		if (DialogueTID != INVALID_TABLE_TID)
//			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//	}
//	break;
//	case EWRVoiceWordType::WAKEUP:
//	{
//		WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//		if (DialogueTID != INVALID_TABLE_TID)
//			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//	}
//	break;
//	case EWRVoiceWordType::HEAL:
//	{
//		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
//		if (MyCharacter)
//		{
//			UWRActorComponentAttack* ActorComponentAttack = MyCharacter->GetComponent<UWRActorComponentAttack>();
//			if (ActorComponentAttack)
//			{
//				if (ActorComponentAttack->IsActiveCoolTime(DEF_AIDA_HEAL_SKILL_TABLE_ID))
//				{
//					WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandImpossibleDialogueTID(VoiceEventDialogueTable);
//					if (DialogueTID != INVALID_TABLE_TID)
//						WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//				}
//				else
//				{
//					WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//					if (DialogueTID != INVALID_TABLE_TID)
//						WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//
//					MyCharacter->UseAidaHeal();
//				}
//			}
//		}
//	}
//	break;
//	case EWRVoiceWordType::QUEST_INFO:
//	{
//		const FWRQuestTable* QuestTable = WRQuestManager::Get()->FindQuestTable(WRQuestManager::Get()->GetTrackedQuestTID());
//		if (QuestTable && QuestTable->SummaryDialogueTID != INVALID_TABLE_TID)
//		{
//			WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//			if (DialogueTID != INVALID_TABLE_TID)
//				WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//
//			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, QuestTable->SummaryDialogueTID);
//			float IgnoreDistCheckingTime = 5.f;
//			WRCompassManager::Get()->StartIgnoreDistChecking(IgnoreDistCheckingTime);
//		}
//		else
//		{
//			WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandImpossibleDialogueTID(VoiceEventDialogueTable);
//			if (DialogueTID != INVALID_TABLE_TID)
//				WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//		}
//	}
//	break;
//	case  EWRVoiceWordType::TUTORIAL:
//	{
//		WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//		if (DialogueTID != INVALID_TABLE_TID)
//			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//
//		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
//		{
//			if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
//			{
//				InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
//				UWRUIPanel_Menu* Panel_Menu = Cast<UWRUIPanel_Menu>(InCombinedPanelData.UIPanel);
//				if (Panel_Menu)
//					Panel_Menu->SetCategory(EWRMenuCategory::Tutorial);
//			}
//		});
//
//		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Menu, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
//		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
//	}
//	break;
//	case  EWRVoiceWordType::TELEPORT:
//	{
//		if (VoiceCommandData && VoiceEventDialogueTable)
//		{
//			FWRCoordinateTable* CoordinateTable = WRTableManager::Get()->FindRow<FWRCoordinateTable>(VoiceCommandData->TargetTID);
//			if (CoordinateTable)
//			{
//				WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//				if (DialogueTID != INVALID_TABLE_TID)
//					WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//
//				FTransform Transform(FRotator(0.f, 180.f, 0.f), FVector(250, 0, 0), FVector::OneVector);
//				WRPopupManager::Get()->OpenPopupOkCancel(Transform
//					, WRUIUtility::ConvertFNameToFText(CoordinateTable->NameLocalkey)
//					, FOnPopupButtonDelegate::CreateUObject(WRCharacterManager::Get()->GetMyCharacter(), &AWRCharacterMine::TeleportUsingVoiceRecognition, VoiceCommandData->TargetTID)
//					, FOnPopupButtonDelegate::CreateStatic(&IMMessageSender::RequestEndVoiceCommand)
//				);
//			}
//		}	
//	}
//	break;
//	case  EWRVoiceWordType::SUMMON:
//	{
//		WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//		if (DialogueTID != INVALID_TABLE_TID)
//			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//
//		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
//		if (MyCharacter)
//			MyCharacter->CallAida();
//	}
//	break;
//	case EWRVoiceWordType::OPTION:
//	{
//		WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//		if (DialogueTID != INVALID_TABLE_TID)
//			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//
//		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::Menu) == false)
//		{
//			if (WRUIManager::Get()->IsWidgetCreating(EWRUIPanelKind::Menu))
//				return;
//
//			FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
//			{
//				if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
//				{
//					InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
//				}
//			});
//
//			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Menu, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
//			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
//		}
//	}
//	break;
//	case EWRVoiceWordType::TRIGGER:
//	{
//		WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//		if (DialogueTID != INVALID_TABLE_TID)
//			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//
//		WRTriggerManager::Get()->RunActiveTrigger(VoiceCommandData->TargetTID);
//	}
//	break;
//	case EWRVoiceWordType::DIALOGUE:
//	{
//		WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::NpcTalk, VoiceCommandData->TargetTID, FSimpleDelegate::CreateStatic(&IMMessageSender::RequestEndVoiceCommand));
//	}
//	break;
//	case EWRVoiceWordType::ChatRecord:
//	{
//		StartChatRecording();
//	}
//	break;
//	case EWRVoiceWordType::StopVoiceRecord:
//	{
//		//StopRecording();
//	}
//	break;
//	default:
//	{
//		WRTableID DialogueTID = FWRVoiceEventDialogue::GetRandUnderstandDialogueTID(VoiceEventDialogueTable);
//		if (DialogueTID != INVALID_TABLE_TID)
//			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
//	}
//	break;
//	}
//
//	// 팝업이 뜨는경우가 아니면 바로 end
//	if(EventType != EWRVoiceWordType::TELEPORT && EventType != EWRVoiceWordType::DIALOGUE)
//		IMMessageSender::RequestEndVoiceCommand();
//}

void WRInputSystemManager::SetInputVoiceEvent(EWRVoiceWordType InEventType, WRTableID InVoiceCommonDataTID)
{
	FString Text = FString::Printf(TEXT("[WRInputSystemManager::SetInputVoiceEvent] %d"), InEventType);
	this->DisplayAndSaveLog(Text);

	if (VoiceController->IsValidLowLevel() == false)
	{
		return;
	}

	VoiceController->OnMatchVoiceEvent(InEventType);
	VoiceEventType = InEventType;
	VoiceCommonDataTID = InVoiceCommonDataTID;
	SetVoiceRecordingState(EWRVoiceRecordingStateType::NONE);
}

void WRInputSystemManager::InputEvent(EWRInput InInputType, EInputEvent InEventType)
{
	bool bEnabled = IsInputEventEnabled(InInputType, InEventType);
	if (bEnabled == false)
		return;
	// [ 2020-2-4 : kyu ] 전투입력체크가 우선이다. 다른 컨텐츠들의 입력체크(return bool)를 한 후 전투입력 체크하도록 하자.
	
	if (InEventType == EInputEvent::IE_Pressed)
		WRDoubleClickManager::Get()->OnInputPressed(InInputType);
	else if (InEventType == EInputEvent::IE_Released)
		WRDoubleClickManager::Get()->OnInputReleased(InInputType);

	WRVoiceContentsManager::Get()->OnInputKeyForAgreeSecretDialog(InInputType);
	

	bool bSuccess = WRVehicleManager::Get()->InputEvent(InInputType, InEventType);
	if (bSuccess)
	{
		return;
	}

	bSuccess = WRInteriorManager::Get()->InputEvent(InInputType, InEventType);
	if (bSuccess)
	{
		return;
	}

	bool bInputPass = false;
	switch (InInputType)
	{
	case EWRInput::Triangle:
	{
		bInputPass = InputEventTriangle(InEventType);
	}
	break;
	case EWRInput::Square:
	{
		bInputPass = InputEventSquare(InEventType);
	}
	break;
	case EWRInput::Circle:
	{
		bInputPass = InputEventCircle(InEventType);
	}
	break;
	case EWRInput::Cross:
	{
		bInputPass = InputEventCross(InEventType);
	}
	break;
	case EWRInput::DpadUP:
	{
		bInputPass = InputEventDpadUp(InEventType);
	}
	break;
	case EWRInput::DpadDown:
	{
		bInputPass = InputEventDpadDown(InEventType);
	}
	break;
	case EWRInput::DpadLeft:
	{
		bInputPass = InputEventDpadLeft(InEventType);
	}
	break;
	case EWRInput::DpadRight:
	{
		bInputPass = InputEventDpadRight(InEventType);
	}
	break;
	case EWRInput::L1:
	{
		bInputPass = InputEventL1(InEventType);
	}
	break;
	case EWRInput::L2:
	{
		bInputPass = InputEventL2(InEventType);
	}
	break;
	case EWRInput::L3:
	{
		bInputPass = InputEventL3(InEventType);
	}
	break;
	case EWRInput::R1:
	{
		bInputPass = InputEventR1(InEventType);
	}
	break;
	case EWRInput::R2:
	{
		bInputPass = InputEventR2(InEventType);
	}
	break;
	case EWRInput::R3:
	{
		bInputPass = InputEventR3(InEventType);
	}
	break;
	case EWRInput::Option:
	{
		bInputPass = InputEventOptionBtn(InEventType);
	}
	break;
	case EWRInput::Share:
	{
		bInputPass = InputEventShareBtn(InEventType);
	}
	break;
	case EWRInput::TouchPadClick:
	{
		bInputPass = InputEventTouchBtn(InEventType);
	}
	break;
	case EWRInput::JoystickPull:
	{
		bInputPass = InputEventMotion(EWRPadMotionType::PULL);
	}
	break;
	case EWRInput::JoystickPush:
	{
		bInputPass = InputEventMotion(EWRPadMotionType::PUSH);
	}
	break;
	case EWRInput::InteractionSkip:
	{
		if (WRObjectInteractionManager::IsValid())
		{
			WRObjectInteractionManager::Get()->CurInteractionSkip();
		}
		bInputPass = true;
	}
	break;
	default:
	{

	}
	break;
	}	

	if (bInputPass == false)
	{
		bInputPass = WRObjectInteractionManager::Get()->InputEventToInteraction(InInputType, InEventType);
	}

	if (bInputPass == false)
	{
		bInputPass = WRInventoryManager::Get()->OnInputEvent(InInputType, InEventType);
	}

	if (bInputPass == false)
	{
		InputEventCombat(InInputType, InEventType);
	}


	// [ 2020-4-7 : kyu ] 컨텐츠들의 입력이벤트에 반응하여 취소 및 기타 동작들 정의
	InputEvnetToContents(InInputType, InEventType);
}

bool WRInputSystemManager::IsInputEventEnabled(EWRInput InInputType, EInputEvent InEventType)
{
	if (IsOnlyMoveInput())
	{
		// [ 2019-12-12 : kyu ] 이동불가일 뿐이지 패드의 움직임까지 막을 필요는 없음
		bool bException = InInputType == EWRInput::JoystickPull
			|| InInputType == EWRInput::JoystickPush
			|| InInputType == EWRInput::Option
			|| InInputType == EWRInput::Share
			|| InInputType == EWRInput::InteractionSkip;
// 			|| InInputType == EWRInput::TouchPadClick
// 			|| InInputType == EWRInput::TouchPadDragDownY
// 			|| InInputType == EWRInput::TouchPadDragUpY;

		if (bException == false)
		{
			return false;
		}
	}

	if (HMDController.bMultiTargetMode)
	{
		if (InEventType == EInputEvent::IE_Pressed)
		{
			HMDController.EndMultiTargetMode();			
		}
	}
	return true;
}

FDynamicForceFeedbackHandle WRInputSystemManager::PlayDynamicForceFeedback(float Intensity, float Duration, bool bAffectsLeftLarge /*= true*/, bool bAffectsLeftSmall /*= true*/, bool bAffectsRightLarge /*= true*/, bool bAffectsRightSmall /*= true*/)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == true)
	{
		APlayerController* Controller = MyChar->GetWorld()->GetFirstPlayerController();
		if (Controller)
		{
			// [ 2019-9-16 : hansang88 ] @param Intensity 피드백의 강도. 유효한 값은 0.0과 1.0 사이
			// [ 2019-9-16 : hansang88 ] @param Duration 피드백이 재생되는 시간입니다. 값이 음수이면 중지 될 때까지 재생
			// [ 2019-9-16 : hansang88 ] @param bAffectsLeftLarge 큰 왼쪽 서보에 강도를 적용할지 여부
			// [ 2019-9-16 : hansang88 ] @param bAffectsLeftSmall 작은 왼쪽 서보에 강도를 적용할지 여부
			// [ 2019-9-16 : hansang88 ] @param bAffectsRightLarge 큰 오른쪽 서보에 강도를 적용할지 여부
			// [ 2019-9-16 : hansang88 ] @param bAffectsRightSmall 작은 오른쪽 서보에 강도를 적용할지 여부
			return Controller->PlayDynamicForceFeedback(Intensity, Duration, bAffectsLeftLarge, bAffectsLeftSmall, bAffectsRightLarge, bAffectsRightSmall);
		}
	}

	return 0;
}

void WRInputSystemManager::StopDynamicForceFeedback(FDynamicForceFeedbackHandle ActionHandle)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == true)
	{
		APlayerController* Controller = MyChar->GetWorld()->GetFirstPlayerController();
		if (Controller)
		{
			Controller->PlayDynamicForceFeedback(0.f, 0.f, false, false, false, false, EDynamicForceFeedbackAction::Stop, ActionHandle);
		}
	}
}

void WRInputSystemManager::ClientPlayForceFeedback(WRTableID InForceFeedbackID)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == true)
	{
		APlayerController* Controller = MyChar->GetWorld()->GetFirstPlayerController();
		if (Controller)
		{
			FWRForceFeedbackTable* ForceFeedbackTable = WRTableManager::Get()->FindRow<FWRForceFeedbackTable>(InForceFeedbackID);
			if (ForceFeedbackTable)
			{
				UForceFeedbackEffect* FeedbackEffect = LoadObject<UForceFeedbackEffect>(nullptr, *ForceFeedbackTable->Forcefeedbackfile);
				if (FeedbackEffect)
				{
					FForceFeedbackParameters FeedbackParams;
					FeedbackParams.bLooping = ForceFeedbackTable->Looping;
					FeedbackParams.bPlayWhilePaused = ForceFeedbackTable->Playwhilepaused;
					FeedbackParams.Tag = ForceFeedbackTable->Tag;
					
					Controller->ClientPlayForceFeedback(FeedbackEffect, FeedbackParams);
				}
			}
		}
	}
}

void WRInputSystemManager::ClientStopForceFeedback(WRTableID InForceFeedbackID)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == true)
	{
		APlayerController* Controller = MyChar->GetWorld()->GetFirstPlayerController();
		if (Controller)
		{
			FWRForceFeedbackTable* ForceFeedbackTable = WRTableManager::Get()->FindRow<FWRForceFeedbackTable>(InForceFeedbackID);
			if (ForceFeedbackTable)
			{
				UForceFeedbackEffect* FeedbackEffect = LoadObject<UForceFeedbackEffect>(nullptr, *ForceFeedbackTable->Forcefeedbackfile);
				if (FeedbackEffect)
				{
					Controller->ClientStopForceFeedback(FeedbackEffect, ForceFeedbackTable->Tag);
				}
			}
		}
	}
}

void WRInputSystemManager::ReleaseTargetLock()
{
	if (HMDController.bLockOnMode == true)
	{
		HMDController.SetTargetLock(false);
	}
}

void WRInputSystemManager::OnMyCharacterDameged()
{
	// [SeungYeop 19/12/17] : 광 파트장님의 요청으로 기능 삭제
	/*if(HMDController.bMultiTargetMode)
		HMDController.EndMultiTargetMode();*/
}

float WRInputSystemManager::GetTargetDirAngle(const float InScale)
{
	if (HMDController.bLockOnMode)
	{
		AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
		if (MyChar->IsValidLowLevel() == true)
		{
			// [ 2019-9-17 : hansang88 ] 타겟과 각도 구하고 같은 방향인지 확인 해당 각도가 콘솔변수로 셋팅된 범위를 넘어선다면 각도 리턴 아니면 0리턴
			FVector MyLocation = MyChar->GetActorLocation();
			MyLocation.Z = 0.f;

			FVector TargetLocation = HMDController.TargetActor->GetActorLocation();
			TargetLocation.Z = 0.f;

			FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);
			FRotator MyRotator = MyChar->GetActorRotation();

			float YawGap = LookAtRotator.Yaw - MyRotator.Yaw;

			if (FMath::Abs(YawGap) > 180)
			{
				if (YawGap > 0)
					YawGap -= 360;
				else
					YawGap += 360;
			}

			if ((YawGap > 0 && InScale < 0) || (YawGap < 0 && InScale >0))
				return 0.f;

			YawGap = FMath::Abs(YawGap);

			float IgnoreAngle = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingRotIgnoreAngle);
			if (IgnoreAngle > YawGap)
				return 0.f;
			
			return YawGap;
		}
	}

	return 0.f;
}

void WRInputSystemManager::SetHMDStartPositionForAvoid(bool bSet)
{
	if (bSet)
	{
		HMDStartPosition = GetHMDLocation();
		HMDStartRotation = GetHMDRotation();
	}
	else
	{
		HMDStartPosition = FVector::ZeroVector;
		HMDStartRotation = FRotator::ZeroRotator;
	}
}


bool WRInputSystemManager::IsTargetActor(const AActor* IN InActor)
{
	return HMDController.IsTargetActor(InActor);
}

// ACharacter* WRInputSystemManager::GetTargetActor()
// {
// 	return HMDController.TargetActor;
// }

void WRInputSystemManager::SetMultiTargetMode(const bool IN InSet, int32 MaxCount)
{
	if (InSet)
	{
		// [SeungYeop 20-02-06] : 연구소에서 멀티타겟팅 지원 X
		AWRGameModeBase* GameModeBase = Cast<AWRGameModeBase>(UGameplayStatics::GetGameMode(UWRGameInstance::GetGameInstance()));
		if (GameModeBase && GameModeBase->GetGameModeType() == EWRGameMode::Lab)
			return;

		if (HMDController.bMultiTargetMode == false)
		{
			// [ 2019-9-27 : magedoga ] TODO : 추후 테이블 생기면 테이블 값으로 대체예정.
			//int MultiTargetMax = WRConsoleManager::Get()->GetInt(EWRConsole::HMD_TargettingCountMax);
			HMDController.MaxMultiCount = MaxCount;
			if (HMDController.MaxMultiCount != 0)
			{
				// [ 2019-9-17 : hansang88 ] 멀티타겟팅 스킬 발동
				HMDController.StartMultiTargetMode();
			}
			else
			{
				// [ 2019-9-17 : hansang88 ] 일반 스킬 발동
			}
		}

		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([](const FWRCombinedPanelData& InCombinedPanelData)
		{
// 			if (InCombinedPanelData.Is3DWidget())
// 			{
// 				InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
// 			}

			if (WRInputSystemManager::Get()->IsActiveMultiTargetMode() == false)
			{
				WRUIManager::Get()->CloseUI(EWRUIPanelKind::MultiTargetUI);
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::MultiTargetUI, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
   		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::MultiTargetUI, FTransform(FRotator(0.f, 180.f, 0.f), FVector(250, 0, 0), FVector::OneVector), EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	}	
	else
	{
		if (HMDController.bMultiTargetMode == true)
		{
			// [ 2019-9-17 : hansang88 ] 노티 먼저 해줘야함.

			HMDController.ActionMultiTargetMode();
		}

		WRUIManager::Get()->CloseUI(EWRUIPanelKind::MultiTargetUI);
	}
}

void WRInputSystemManager::SetMotionValueZ(EWRPadMotionType InType, float InValue)
{
	if (PadController->IsValidLowLevel())
	{
		PadController->SetMotionValueZ(InType, InValue);
	}
}

void WRInputSystemManager::SetMotionValueY(EWRPadMotionType InType, float InValue)
{
	if (PadController->IsValidLowLevel())
	{
		PadController->SetMotionValueY(InType, InValue);
	}
}

void WRInputSystemManager::ToggleHMDPicking()
{
	AWRCharacterMine* pMe = WRCharacterManager::Get()->GetMyCharacter();
	if (pMe->IsValidLowLevel() == false)
		return;

	if (pMe->IsAIMoveTo())
	{
		IMMessageSender::RequestPlayerMoveStop(pMe);
		CancelHMDPicking();
	}
	else
	{
		bUpdateHMDPicking = !bUpdateHMDPicking;
		if (bUpdateHMDPicking)
		{
			//이펙트 생성은 UpdateHMDPickingVFX 여기에서 함
			//여기서는 ui만 켜준다
			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::MoveSimulationGuide, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCameraAll, nullptr, pMe);
			strAIMoveUIAsyncKey = WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		}
		else
		{
			CancelHMDPicking();
		}
	}
}

bool WRInputSystemManager::ConfirmHMDPicking()
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == MyChar)
		return false;

	if (false == bUpdateHMDPicking)
		return false;

/*
	switch (MyChar->GetCurrentFSMType())
	{
	case EWRFSM::Idle:
	case EWRFSM::Move:
	{
	}
	break;
	default:
	{
		WRSFXManager::Get()->Spawn2D(WRUIUtility::ConvertFNameToFString("UI_SELECT.UI_SELECT"));
		IMMessageSender::ReqeustPlayerAIMoveTo(MyChar, HMDPickingLocation);
	}
	break;
	}
*/

	WRSFXManager::Get()->Spawn2D(WRUIUtility::ConvertFNameToFString("UI_SELECT.UI_SELECT"));
	IMMessageSender::ReqeustPlayerAIMoveTo(MyChar, HMDPickingLocation);
	CancelHMDPicking();

	return true;
}

void WRInputSystemManager::CancelHMDPicking()
{
	RemoveHMDPickingVFX();
	HMDPickingLocation = FVector::ZeroVector;
	bUpdateHMDPicking = false;

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::MoveSimulationGuide))
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::MoveSimulationGuide);
	}
	else
	{
		WRUIManager::Get()->CancelUIAsync(strAIMoveUIAsyncKey);
	}
}

void WRInputSystemManager::UpdateHMDPickingVFX()
{
	if (LocationVFX)
	{
		if (HMDPickingLocation != FVector::ZeroVector)	//좌표가 유효할때 이펙트 좌표 업데이트
		{
			if (LocationVFX->IsHidden())
			{
				LocationVFX->SetActorHiddenInGame(false);
				//BeamVFX->SetActorHiddenInGame(false);
			}

			LocationVFX->SetActorLocation(HMDPickingLocation);
		}
		else
		{
			//좌표가 유효하지 않으면 이펙트 끄기
			LocationVFX->SetActorHiddenInGame(true);
			//BeamVFX->SetActorHiddenInGame(true);
		}
	}
	else
	{
		//이펙트가 생성되지 않은상태면 생성
		CreateHMDPickingVFX(HMDPickingLocation);
	}
}

void WRInputSystemManager::CreateHMDPickingVFX(FVector InLocation, bool bCreaetBeamVfx /* = true*/)
{
	if (LocationVFX)
	{
		//기존에 생성된게 있으면 삭제하고 재생성
		RemoveHMDPickingVFX();
	}

	LocationVFX = WRVFXManager::Get()->Spawn("/Game/Asset/Effect/00_Cascade/XR_LocationMesh.XR_LocationMesh", WRCharacterManager::Get()->GetMyCharacter(), nullptr);
	if (LocationVFX)
	{
		LocationVFX->SetActorLocation(InLocation);
		if (bCreaetBeamVfx)
		{
			//BeamVFX = WRVFXManager::Get()->Spawn("/Game/Asset/BluePrint/Effect/Cascade/XR_LocationBeam.XR_LocationBeam", WRCharacterManager::Get()->GetMyCharacter(), nullptr, LocationVFX);
		}
	}
}

void WRInputSystemManager::RemoveHMDPickingVFX()
{
	if (LocationVFX)
	{
		WRVFXManager::Get()->Remove(LocationVFX);
		//WRVFXManager::Get()->Remove(BeamVFX);

		LocationVFX = nullptr;
	}
}

void WRInputSystemManager::ResetHMDOrientation()
{
	HMDController.ResetOrientation();
}

void WRInputSystemManager::InputEventCombat(EWRInput InInputType, EInputEvent InEventType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (nullptr == MyChar)
		return;


	UWRActorComponentAttack* AttackComponent = MyChar->GetComponent<UWRActorComponentAttack>();
	if (nullptr == AttackComponent)
		return;

	switch (InEventType)
	{
	case IE_Pressed:
	{
		AttackComponent->PressBtn(InInputType);
	}
	break;
	case IE_Released:
	{
		AttackComponent->ReleaseBtn(InInputType);
	}
	break;
	}
}

bool WRInputSystemManager::InputEventTriangle(EInputEvent InEventType)
{
	switch (InEventType)
	{
	case IE_Pressed:
	{		
		AWRCharacter* TargetCharacter = const_cast<AWRCharacter*>(WRCharacterManager::Get()->GetMyTarget());
		if (TargetCharacter)
		{
			if (TargetCharacter->GetCharacterType() == EWRCharacterType::NPC)
			{
				UWRActorComponentCommonNpcTalk* CommonNPCTalkComponent = TargetCharacter->GetComponent<UWRActorComponentCommonNpcTalk>();
				if (CommonNPCTalkComponent && CommonNPCTalkComponent->IsAvailableCommonNpcTalk())
				{
					CommonNPCTalkComponent->ProgressCommonNpcTalk();
					return true;
				}

				UWRActorComponentNpcSocial* NpcSocialComponent = TargetCharacter->GetComponent<UWRActorComponentNpcSocial>();
				if (NpcSocialComponent && NpcSocialComponent->IsAvailableNpcSocial())
				{
					NpcSocialComponent->ProgressNpcSocial();
					return true;
				}
			}
			else if (TargetCharacter->GetCharacterType() == EWRCharacterType::Player)
			{
				UWRActorComponentPlayerSocial* PlayerSocialComponent = TargetCharacter->GetComponent<UWRActorComponentPlayerSocial>();
				if (PlayerSocialComponent && PlayerSocialComponent->IsAvailablePlayerSocial())
				{
					PlayerSocialComponent->ProgressPlayerSocial();
					return true;
				}
			}
		}		

		/*AWRGameModeBase* GameModeBase = Cast<AWRGameModeBase>(UGameplayStatics::GetGameMode(UWRGameInstance::GetGameInstance()));
		if (GameModeBase && GameModeBase->GetGameModeType() != EWRGameMode::Lab && WRMapManager::Get()->GetCurrentMapType() != EWRMapType::BattleField)
		{
			AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
			if (MyCharacter && MyCharacter->IsInputPad() == false)
			{
				if (WRUIManager::Get()->IsWidgetCreating(EWRUIPanelKind::SocialMenu) == false && WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu) == false)
				{
					if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SelectDialogue) == false 
						&& WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange) == false
						&& MyCharacter->IsDanceModeOn())
					{
						FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
						{
							if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
							{
								UWRUIPanel_SocialMenu* Panel_SocialMenu = Cast<UWRUIPanel_SocialMenu>(InCombinedPanelData.UIPanel);
								if (Panel_SocialMenu)
									Panel_SocialMenu->SetOwnerCharacter(MyCharacter);

								InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
							}
						});

						WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::SocialMenu, MyCharacter, FTransform(FRotator(0.f, 0.f, 0.f), FVector(350, 0, 0), FVector::OneVector), EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::WidgetInteractionable | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, nullptr, true);
					}
				}
				else
				{
					WRUIManager::Get()->CloseUI(EWRUIPanelKind::SocialMenu);
				}

				return true;
			}
		}*/
	}	
	break;
	case IE_Released:
	{
		
	}
	break;
	}

	return false;
}

bool WRInputSystemManager::InputEventSquare(EInputEvent InEventType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;

	switch (InEventType)
	{
	case IE_Pressed:
	{
		// [19/12/20 SeungYeop] : 기획 요청(규일 팀장님)으로 막음
		/*AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter)
		{
			MyCharacter->UseAidaHeal();
		}*/
		/*IConsoleVariable* ConsoleVariable = WRConsoleManager::Get()->GetConsoleVariable(EWRConsole::BlackOutIn_Enable);
		if (ConsoleVariable)
		{
			bool BeforeBlackOutIn_Enable = WRConsoleManager::Get()->GetBool(EWRConsole::BlackOutIn_Enable);
			if (BeforeBlackOutIn_Enable)
			{
				FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(DEF_CUSTOM_TEXT_TID_FOR_BLACK_OUTIN_OFF);
				if (CustomTextTable)
				{
					WRNoticeManager::Get()->AddNotice(WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey));
				}
			}
			else
			{
				FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(DEF_CUSTOM_TEXT_TID_FOR_BLACK_OUTIN_ON);
				if (CustomTextTable)
				{
					WRNoticeManager::Get()->AddNotice(WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey));
				}
			}

			ConsoleVariable->Set(!WRConsoleManager::Get()->GetBool(EWRConsole::BlackOutIn_Enable));
		}	*/

		//WRSFXManager::Get()->Spawn2D(SOUND_PATH_4_SQUARE_BTN);
	}
	break;
	case IE_Released:
	{

	}
	break;
	}

	return false;
}

bool WRInputSystemManager::InputEventCircle(EInputEvent InEventType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;

	switch (InEventType)
	{
	case IE_Pressed:
	{
#if PLATFORM_PS4
		// [ 2020-2-11 : hansang88 ] PS4 진행 해보기 위해 임시 코드
		InputEventMotion(EWRPadMotionType::PULL);
#endif //PLATFORM_PS4
		if (ConfirmHMDPicking())
		{
			return true;
		}

		// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜져있으면 OK 호출을 한다.
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
		{
			UWRUIPanel_CostumeChange* CostumeChangePanel = WRUIManager::Get()->FindUI<UWRUIPanel_CostumeChange>(EWRUIPanelKind::CostumeChange);
			if (CostumeChangePanel != nullptr)
			{
				CostumeChangePanel->OnOKKeyPressed();
			}
			return true;
		}

		UWRUIPanel_DanceMode* Panel_DanceMode = WRUIManager::Get()->FindUI<UWRUIPanel_DanceMode>(EWRUIPanelKind::DanceMode);
		if (WRCheersManager::Get()->CheckSocialInput(EWRInput::Circle))
		{
			SetInputReleaseIgnore(EWRInput::Circle);
		}
		else if (Panel_DanceMode && Panel_DanceMode->GetCurFunctionType() == UWRUIPanel_DanceMode::EWRFunctionType::StartDanceMode)
		{
			Panel_DanceMode->OnStartDanceModeClicked();
			SetInputReleaseIgnore(EWRInput::Circle);
			return true;
		}

		if (WRVoiceContentsManager::Get()->GetVideoChatController().OnInputEvent(EWRInput::Circle))
		{
			return true;
		}

		UWRUIPanel_SocialMenu* Panel_SocialMenu = WRUIManager::Get()->Get()->FindUI<UWRUIPanel_SocialMenu>(EWRUIPanelKind::SocialMenu);
		if (Panel_SocialMenu && Panel_SocialMenu->OnInputEventReceived(EWRInput::Circle))
		{
			return true;
		}


		// [ 2019-8-20 : kyu ] Widget Interaction
		UWidgetInteractionComponent* WidgetInteractionComponent = Cast<UWidgetInteractionComponent>(MyChar->GetComponentByClass(UWidgetInteractionComponent::StaticClass()));
		if (WidgetInteractionComponent != nullptr && WidgetInteractionComponent->GetHoveredWidgetComponent() != nullptr)
		{
			WidgetInteractionComponent->PressPointerKey(EKeys::LeftMouseButton);
			return true;
		}
	}
	break;
	case IE_Released:
	{
		// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜져있으면 OK 호출을 한다.
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
		{
			UWRUIPanel_CostumeChange* CostumeChangePanel = WRUIManager::Get()->FindUI<UWRUIPanel_CostumeChange>(EWRUIPanelKind::CostumeChange);
			if (CostumeChangePanel != nullptr)
			{
				CostumeChangePanel->OnOKKeyClicked();
			}
			return true;
		}

		// [ 2020-3-12 : kyu ] 입력 Press에서 체크 후 SetInputReleaseIgnore() 사용 시 Release에서 체크 무시하도록 셋팅
		// [ 2020-3-12 : kyu ] 전투입력도 무시
		if (CheckInputReleaseIgnore(EWRInput::Circle))
			return true; 

		// [ 2019-8-20 : kyu ] Widget Interaction
		UWidgetInteractionComponent* WidgetInteractionComponent = Cast<UWidgetInteractionComponent>(MyChar->GetComponentByClass(UWidgetInteractionComponent::StaticClass()));
		if (WidgetInteractionComponent != nullptr && WidgetInteractionComponent->GetHoveredWidgetComponent() != nullptr)
		{
			WidgetInteractionComponent->ReleasePointerKey(EKeys::LeftMouseButton);
			return true;
		}
	}
	break;
	}

	
	return false;
}

bool WRInputSystemManager::InputEventCross(EInputEvent InEventType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;
	
	switch (InEventType)
	{
	case IE_Pressed:
	{
#if PLATFORM_PS4
		// [ 2020-2-11 : hansang88 ] PS4 진행 해보기 위해 임시 코드
		InputEventMotion(EWRPadMotionType::PULL);
#endif //PLATFORM_PS4
		
		
		// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜져있으면 Cancel 호출을 한다.
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
		{
			UWRUIPanel_CostumeChange* CostumeChangePanel = WRUIManager::Get()->FindUI<UWRUIPanel_CostumeChange>(EWRUIPanelKind::CostumeChange);
			if (CostumeChangePanel != nullptr)
			{
				CostumeChangePanel->OnCancelKeyPressed();
			}
			return true;
		}

		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SelectDialogue))
		{
			SetInputReleaseIgnore(EWRInput::Cross);
			UWRUIPanel_SelectDialogue* Panel_SelectDialogue = WRUIManager::Get()->FindUI<UWRUIPanel_SelectDialogue>(EWRUIPanelKind::SelectDialogue);
			if (Panel_SelectDialogue)			
				Panel_SelectDialogue->ForcedStopSelectDialogue();
			return true;
		}
		else if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::SocialMenu);
			SetInputReleaseIgnore(EWRInput::Cross);
			return true;
		}
		else if (WRCheersManager::Get()->CheckSocialInput(EWRInput::Cross))
		{
			SetInputReleaseIgnore(EWRInput::Cross);
			return true; 
		}
		else if (WRItemManager::Get()->ThrowTemporaryItemAway(EWRReasonForThrowTemporaryItemAway::ThrowOutManual))
		{
			SetInputReleaseIgnore(EWRInput::Cross);
			return true;
		}
		else if (WRVoiceContentsManager::Get()->GetVideoChatController().OnInputEvent(EWRInput::Cross))
		{
			return true;
		}
		
		// [ 2020-3-26 : magedoga ] Stop 360 Movie. if, Playing 360 movie
		MyChar->Stop360Movie();
	}
	break;
	case IE_Released:
	{
		// [ 2020-3-12 : kyu ] 입력 Press에서 체크 후 SetInputReleaseIgnore() 사용 시 Release에서 체크 무시하도록 셋팅
		// [ 2020-3-12 : kyu ] 전투입력도 무시
		if (CheckInputReleaseIgnore(EWRInput::Cross))
			return true; 
		
		// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜져있으면 Cancel 호출을 한다.
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
		{
			UWRUIPanel_CostumeChange* CostumeChangePanel = WRUIManager::Get()->FindUI<UWRUIPanel_CostumeChange>(EWRUIPanelKind::CostumeChange);
			if (CostumeChangePanel != nullptr)
			{
				CostumeChangePanel->OnCancelKeyClicked();
			}
			return true;
		}
			
		// [ 2019-12-5 : kyu ] 입력해제 시 인터렉션중인 대상에게로 전달. 인터렉션중이 아니면 케릭터의 타겟에게 전달
		if (WRObjectInteractionManager::Get()->IsInteractingWithTarget())
		{
			AWRCharacter* pTarget = WRObjectInteractionManager::Get()->GetSingleInteractionTarget();
			if (pTarget->IsValidLowLevel() && pTarget->IsInteracting())
			{
				pTarget->OnReleasedEvent(EWRInput::Cross);
				return true;
			}
		}
		else
		{
			if (MyChar->GetTargetAID() != INVALID_AID)
			{
				AWRCharacter* pFoundCharacter = WRCharacterManager::Get()->FindbyAID(MyChar->GetTargetAID());
				if (pFoundCharacter && pFoundCharacter->IsValidLowLevel() && pFoundCharacter->IsIO())
				{
					pFoundCharacter->OnReleasedEvent(EWRInput::Cross);
					return true;
				}
			}
		}	

		// [ 2020-07-13 : EZ - SecretDialogPortal Open ]
		//WREnvInteractionManager::Get()->StartSecretDialogProcess();
		
	}
	break;
	}

	return false;
}

bool WRInputSystemManager::InputEventL1(EInputEvent InEventType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;
	
	switch (InEventType)
	{
	case IE_Pressed:
	{
		// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜져있으면 L1 호출을 한다.
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
		{
			UWRUIPanel_CostumeChange* CostumeChangePanel = WRUIManager::Get()->FindUI<UWRUIPanel_CostumeChange>(EWRUIPanelKind::CostumeChange);
			if (CostumeChangePanel != nullptr)
			{
				CostumeChangePanel->OnL1KeyPressed();
			}
			return true;
		}

		UWRActorComponentDanceMode* DanceModeComponent = MyChar->GetComponent<UWRActorComponentDanceMode>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn())
		{
			UWRUIPanel_DanceMode* Panel_DanceMode = WRUIManager::Get()->FindUI<UWRUIPanel_DanceMode>(EWRUIPanelKind::DanceMode);
			if (Panel_DanceMode && Panel_DanceMode->GetCurFunctionType() == UWRUIPanel_DanceMode::EWRFunctionType::NextStep && Panel_DanceMode->GetNextStepInput() == EWRInput::L1)
				Panel_DanceMode->OnNextStepClicked();

			return true;
		}
	}
	break;
	case IE_Released:
	{
		// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜져있으면 L1 호출을 한다.
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
		{
			UWRUIPanel_CostumeChange* CostumeChangePanel = WRUIManager::Get()->FindUI<UWRUIPanel_CostumeChange>(EWRUIPanelKind::CostumeChange);
			if (CostumeChangePanel != nullptr)
			{
				CostumeChangePanel->OnL1KeyClicked();
			}
			return true;
		}

		UWRActorComponentDanceMode* DanceModeComponent = MyChar->GetComponent<UWRActorComponentDanceMode>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn())
		{
			return true;
		}
	}
	break;
	}

	return false;
}

bool WRInputSystemManager::InputEventL2(EInputEvent InEventType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;

	switch (InEventType)
	{
	case IE_Pressed:
	{
		bPressedL2 = true;

		UWRActorComponentDanceMode* DanceModeComponent = MyChar->GetComponent<UWRActorComponentDanceMode>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn())
		{
			UWRUIPanel_DanceMode* Panel_DanceMode = WRUIManager::Get()->FindUI<UWRUIPanel_DanceMode>(EWRUIPanelKind::DanceMode);
			if (Panel_DanceMode && Panel_DanceMode->GetCurFunctionType() == UWRUIPanel_DanceMode::EWRFunctionType::NextStep && Panel_DanceMode->GetNextStepInput() == EWRInput::L2)
				Panel_DanceMode->OnNextStepClicked();

			return true;
		}
	}
	break;
	case IE_Released:
	{
		bPressedL2 = false;

		UWRActorComponentDanceMode* DanceModeComponent = MyChar->GetComponent<UWRActorComponentDanceMode>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn())
		{
			return true;
		}
	}
	break;
	}

	// [ 2019-11-28 : magedoga ] 임시 내부 조합
	if (bPressedL2 == true && bPressedR2 == true)
	{
		this->OnEventPressedL2R2();
	}

	return false;
}

bool WRInputSystemManager::InputEventL3(EInputEvent InEventType)
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	switch (InEventType)
	{
	case IE_Pressed:
	{
		UE_LOG(LogTemp, Log, TEXT("InputL3 - Pressed"));
// 		if (MyCharacter->IsValidLowLevel() == true)
// 		{
// 			MyCharacter->SetSyncRotationByHMD(true);
// 		}

		// [20-02-06 SeungYeop ]: TargetLock 기능 삭제
		/*if (HMDController.TargetActor)
		{
			if (HMDController.bLockOnMode == true)
			{
				HMDController.SetTargetLock(false);
			}
			else
			{
				HMDController.SetTargetLock(true);
			}
		}*/
	}
	break;
	case IE_Released:
	{
		UE_LOG(LogTemp, Log, TEXT("InputL3 - Released"));
// 		if (MyCharacter->IsValidLowLevel() == true)
// 		{
// 			MyCharacter->SetSyncRotationByHMD(false);
// 		}
	}
	break;
	}

	return false;
}

bool WRInputSystemManager::InputEventR1(EInputEvent InEventType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;

	switch (InEventType)
	{
	case IE_Pressed:
	{
		// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜져있으면 R1 호출을 한다.
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
		{
			UWRUIPanel_CostumeChange* CostumeChangePanel = WRUIManager::Get()->FindUI<UWRUIPanel_CostumeChange>(EWRUIPanelKind::CostumeChange);
			if (CostumeChangePanel != nullptr)
			{
				CostumeChangePanel->OnR1KeyPressed();
			}
			return true;
		}

		UWRActorComponentDanceMode* DanceModeComponent = MyChar->GetComponent<UWRActorComponentDanceMode>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn())
		{
			UWRUIPanel_DanceMode* Panel_DanceMode = WRUIManager::Get()->FindUI<UWRUIPanel_DanceMode>(EWRUIPanelKind::DanceMode);
			if (Panel_DanceMode && Panel_DanceMode->GetCurFunctionType() == UWRUIPanel_DanceMode::EWRFunctionType::NextStep && Panel_DanceMode->GetNextStepInput() == EWRInput::R1)
				Panel_DanceMode->OnNextStepClicked();

			return true;
		}
	}
	break;
	case IE_Released:
	{
		// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜져있으면 R1 호출을 한다.
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
		{
			UWRUIPanel_CostumeChange* CostumeChangePanel = WRUIManager::Get()->FindUI<UWRUIPanel_CostumeChange>(EWRUIPanelKind::CostumeChange);
			if (CostumeChangePanel != nullptr)
			{
				CostumeChangePanel->OnR1KeyClicked();
			}
			return true;
		}

		UWRActorComponentDanceMode* DanceModeComponent = MyChar->GetComponent<UWRActorComponentDanceMode>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn())
		{
			return true;
		}
	}
	break;
	}

	return false;
}

bool WRInputSystemManager::InputEventR2(EInputEvent InEventType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;

	switch (InEventType)
	{
	case IE_Pressed:
	{
		bPressedR2 = true;

		if (WRVoiceContentsManager::Get()->RotateHMDInDirectionOfFocusTalkTarget())
			return true;

		UWRActorComponentDanceMode* DanceModeComponent = MyChar->GetComponent<UWRActorComponentDanceMode>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn())
		{
			UWRUIPanel_DanceMode* Panel_DanceMode = WRUIManager::Get()->FindUI<UWRUIPanel_DanceMode>(EWRUIPanelKind::DanceMode);
			if (Panel_DanceMode && Panel_DanceMode->GetCurFunctionType() == UWRUIPanel_DanceMode::EWRFunctionType::NextStep && Panel_DanceMode->GetNextStepInput() == EWRInput::R2)
				Panel_DanceMode->OnNextStepClicked();

			return true;
		}
		else if (WRCheersManager::Get()->CheckSocialInput(EWRInput::R2))
		{
			SetInputReleaseIgnore(EWRInput::R2);
			return true;
		}
	}
	break;
	case IE_Released:
	{
		bPressedR2 = false;

		UWRActorComponentDanceMode* DanceModeComponent = MyChar->GetComponent<UWRActorComponentDanceMode>();
		if (DanceModeComponent && DanceModeComponent->IsDanceModeOn())
		{
			return true;
		}
		else if (CheckInputReleaseIgnore(EWRInput::R2))
		{
			// [ 2020-3-12 : kyu ] 입력 Press에서 체크 후 SetInputReleaseIgnore() 사용 시 Release에서 체크 무시하도록 셋팅
			return true;
		}
	}
	break;
	}

	// [ 2019-11-28 : magedoga ] 임시 내부 조합
	if (bPressedL2 == true && bPressedR2 == true)
	{
		this->OnEventPressedL2R2();
	}

	return false;
}

bool WRInputSystemManager::InputEventR3(EInputEvent InEventType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;

	switch (InEventType)
	{
	case IE_Pressed:
	{
		// [ 2020-9-2 : kyu ] 점유중 HMD Picking 불가처리
		UWRActorComponentInteraction* Component = MyChar->GetComponent<UWRActorComponentInteraction>();
		if (Component && Component->GetOccupiedIO() != nullptr)
		{
			return true;
		}

		ToggleHMDPicking();
	}
	break;
	case IE_Released:
	{
		
	}
	break;
	}

	return true; // 무조건
}

bool WRInputSystemManager::OnEventPressedL2R2()
{
	this->ResetHMDOrientation();

	return true; // 무조건
}

void WRInputSystemManager::SetInputReleaseIgnore(EWRInput IN InInput)
{
	bool& bIgnore = InputReleaseIgnore.FindOrAdd(InInput, true);
	bIgnore = true;
}

bool WRInputSystemManager::CheckInputReleaseIgnore(EWRInput IN InInput)
{
	bool* pIgnore = InputReleaseIgnore.Find(InInput);
	if (pIgnore == nullptr)
		return false;
	bool bIgnore = *pIgnore;
	*pIgnore = false;
	return bIgnore;
}

void WRInputSystemManager::CalculateHMDRelativePosition(OUT FVector& ResultPos, FVector InPos)
{
	FMatrix HMDMatrix = FMatrix::Identity;
	FQuat HMDQuat = GetHMDRotation().Quaternion();
	float qx, qy, qz, qw, qx2, qy2, qz2, qxqx2, qyqy2, qzqz2, qxqy2, qyqz2, qzqw2, qxqz2, qyqw2, qxqw2;
	qx = HMDQuat.X;
	qy = HMDQuat.Y;
	qz = HMDQuat.Z;
	qw = HMDQuat.W;
	qx2 = (qx + qx);
	qy2 = (qy + qy);
	qz2 = (qz + qz);
	qxqx2 = (qx * qx2);
	qxqy2 = (qx * qy2);
	qxqz2 = (qx * qz2);
	qxqw2 = (qw * qx2);
	qyqy2 = (qy * qy2);
	qyqz2 = (qy * qz2);
	qyqw2 = (qw * qy2);
	qzqz2 = (qz * qz2);
	qzqw2 = (qw * qz2);
	FVector Col1 = FVector(((1.0f - qyqy2) - qzqz2), (qxqy2 + qzqw2), (qxqz2 - qyqw2));
	FVector Col2 = FVector((qxqy2 - qzqw2), ((1.0f - qxqx2) - qzqz2), (qyqz2 + qxqw2));
	FVector Col3 = FVector((qxqz2 + qyqw2), (qyqz2 - qxqw2), ((1.0f - qxqx2) - qyqy2));

	FVector HMDLocation = GetHMDLocation();
	HMDMatrix.SetAxes(&Col1, &Col2, &Col3, &HMDLocation);

	ResultPos = HMDMatrix.InverseTransformPosition(InPos) / 1000.f;
}

void WRInputSystemManager::CalculateVoiceChatInfo(OUT FVector& OutPos, OUT float& OutGain, AID_t InAID)
{
	AWRCharacter* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	AWRCharacter* Talker = WRCharacterManager::Get()->FindbyAID(InAID);

	if (MyChar->IsValidLowLevel() == true && Talker->IsValidLowLevel() == true)
	{
		float Distance = WRActorUtility::GetDistance(MyChar, Talker);
		float AttenuationIgnoreDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::PS4_Voice_Attenuation_Ignore_Distance);
		float Voice3DIgnoreDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::PS4_Voice_3D_Ignore_Distance);

		OutPos = FVector::ZeroVector;
		OutGain = 1.f;
		// 3d hmd 상대 좌표셋팅
		if (Distance > Voice3DIgnoreDistance)
		{
			CalculateHMDRelativePosition(OutPos, Talker->GetActorLocation());
		}

		// 감쇠율 셋팅
		if (Distance > AttenuationIgnoreDistance)
		{
			FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(MyChar->GetCharacterTableID());
			if (PlayerTable != nullptr)
			{
				float MinimumVolume = WRConsoleManager::Get()->GetFloat(EWRConsole::PS4_Voice_Minimum_Volume);
				// 음성채팅 영역 벗어낫을때
				if (Distance >= PlayerTable->VoiceChatRange)
				{
					// 최소 볼륨 비율로 셋팅
					OutGain = MinimumVolume;
				}
				else
				{
					// 음성 영역 내에 위치할때 감쇠 비율 계산
					if (PlayerTable->VoiceChatRange > AttenuationIgnoreDistance)
					{
						OutGain = 1 - ((Distance - AttenuationIgnoreDistance) / (PlayerTable->VoiceChatRange - AttenuationIgnoreDistance) * (1 - MinimumVolume));
					}
				}
			}
		}
	}
}

EWRInput WRInputSystemManager::FindInput(const FName& IN InKeyNameInSetting)
{
	const EWRInput* FoundInput = InputMaps.Find(InKeyNameInSetting);
	if (FoundInput == nullptr)
		return EWRInput::None;

	return *FoundInput;
}

EWRAttackHeightType WRInputSystemManager::GetAttackHeightType()
{
	float Pitch = GetHMDRotation().Pitch;
	if (WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TopAttack_Degree) <= Pitch)
	{
		return EWRAttackHeightType::Upper;
	}
	else if (WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_BottomAttack_Degree) < Pitch)
	{
		return EWRAttackHeightType::Middle;
	}
	else
	{
		return EWRAttackHeightType::Lower;
	}
}

#if PLATFORM_PS4
	#if WR_ADD_VOICE_PORT

void WRInputSystemManager::SetVoiceVolume(uint32 PortIndex, float Volume)
{
	IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(UWRGameInstance::GetGameInstance()->GetWorld());
	if (PS4Subsystem != nullptr)
	{
		IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();
		if (VoiceInt.IsValid() == true)
		{
			VoiceInt->SetPortVolume(PortIndex, Volume);
		}
	}
}

int WRInputSystemManager::GetPortIndexByDistance(AID_t InAID, AID_t InFocusedPlayerAID)
{
	AWRCharacter* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	AWRCharacter* Talker = WRCharacterManager::Get()->FindbyAID(InAID);

	if (MyChar->IsValidLowLevel() == true && Talker->IsValidLowLevel() == true)
	{
		if (MyChar->GetActorID() == InFocusedPlayerAID)
		{
			return IN_PROT_COUNT - 1;
		}

		float Distance = WRActorUtility::GetDistance(MyChar, Talker);
		
		float PreDistance = 0;
		for (int i = 0; i < IN_PROT_COUNT; ++i)
		{
			FString EnumName = FString::Printf(TEXT("PS4_Voice_Distance_%d"), i + 1);
			EWRConsole ConsoleEnum = (EWRConsole)WREnumUtility::StringToEnum("EWRConsole", EnumName);
			float VoiceDistance = WRConsoleManager::Get()->GetFloat(ConsoleEnum);
			if (PreDistance >= Distance && Distance <= VoiceDistance)
			{
				return i;
			}

			PreDistance = VoiceDistance;
		}
	}
	return IN_PROT_COUNT - 1;
}
	#endif //WR_ADD_VOICE_PORT
void WRInputSystemManager::OnInformVoiceChat(AID_t InAID, FString MsgBuffer, uint16 Length, AID_t InFocusedPlayerAID)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == true)
	{
		IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(MyChar->GetWorld());
		if (PS4Subsystem != nullptr)
		{
			IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();

			if (VoiceInt.IsValid() == true)
			{
	#if WR_ADD_VOICE_PORT
				uint8_t WriteBuffer[2048];
		#if WR_ADD_3D_VOICE		
				float Gain = 1.f;
				FVector TalkerRelativeLocation = FVector::ZeroVector;

				CalculateVoiceChatInfo(TalkerRelativeLocation, Gain, InAID);

				StringToBytes(MsgBuffer, WriteBuffer, Length);
				VoiceInt->WriteVoice(0, WriteBuffer, Length);

				VoiceInt->Output2Audio3D(TalkerRelativeLocation, Gain);
		#else //WR_ADD_3D_VOICE
// 				int PortIndex = GetPortIndexByDistance(InAID, InFocusedPlayerAID);
// 				StringToBytes(MsgBuffer, WriteBuffer, Length);
// 				VoiceInt->WriteVoice(PortIndex, WriteBuffer, Length);
				AWRCharacterPlayer* Talker = Cast<AWRCharacterPlayer>(WRCharacterManager::Get()->FindbyAID(InAID));
				if (Talker->IsValidLowLevel() == true)
				{
					UWRActorComponentVoiceChatAudio* ChatAudioComponent = Talker->GetComponent<UWRActorComponentVoiceChatAudio>();
					if (ChatAudioComponent)
					{
						if (Length > 0)
						{
							TArray<uint8> voiceData;
							voiceData.AddZeroed(Length);
							StringToBytes(MsgBuffer, voiceData.GetData(), Length);

							float volumeMultiplier = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_VoiceChatVolumeMultiplier);

							ChatAudioComponent->QueueChatAudio(voiceData);
							ChatAudioComponent->PlayVoiceAudio();
						}
					}
				}
		#endif //WR_ADD_3D_VOICE

	#else //WR_ADD_VOICE_PORT
		#if WR_ADD_VOICE
				VoiceInt->WriteRemoteVoiceBuffer(MsgBuffer, Length, 0, InAID);
		#endif //WR_ADD_VOICE
	#endif //WR_ADD_VOICE_PORT
			}
		}
	}
}
#endif //PLATFORM_PS4

void WRInputSystemManager::DisplayAndSaveLog(const FString& InString)
{
	GEngine->AddOnScreenDebugMessage(IndexLogs++, 3.0f, FColor::Green, InString);

	FILE* fp = fopen(VoiceLogs, "a+");
	if (fp == nullptr)
	{
		return;
	}

	fprintf(fp, "%s\n", TCHAR_TO_UTF8(*InString));

	fclose(fp);
}

void WRInputSystemManager::InputEventLStickX(float IN InScale)
{
	LStickScale.X = InScale;
}

void WRInputSystemManager::InputEventLStickY(float IN InScale)
{
	LStickScale.Y = InScale;
}

bool WRInputSystemManager::InputEventDpadUp(EInputEvent InEventType)
{
	switch (InEventType)
	{
	case IE_Pressed:
	{
		// [19/12/20 SeungYeop] : 기획 요청(규일 팀장님)으로 막음
		/*FWRVoiceEventDialogue* VoiceEventDialogueTable = WRTableManager::Get()->FindRow<FWRVoiceEventDialogue>(1);
		if (VoiceEventDialogueTable && VoiceEventDialogueTable->Understand != INVALID_TABLE_TID)
		{
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, VoiceEventDialogueTable->Understand);
		}

		const FWRQuestTable* QuestTable = WRQuestManager::Get()->FindQuestTable(WRQuestManager::Get()->GetTrackedQuestTID());
		if (QuestTable && QuestTable->SummaryDialogueTID != INVALID_TABLE_TID)
			WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, QuestTable->SummaryDialogueTID);*/

		UWRUIPanel_SocialMenu* Panel_SocialMenu = WRUIManager::Get()->Get()->FindUI<UWRUIPanel_SocialMenu>(EWRUIPanelKind::SocialMenu);
		if (Panel_SocialMenu && Panel_SocialMenu->OnInputEventReceived(EWRInput::DpadUP))
		{
			return true;
		}
	}
	break;
	case IE_Released:
	{
	}
	break;
	}

	return true;
}

bool WRInputSystemManager::InputEventDpadDown(EInputEvent InEventType)
{
	switch (InEventType)
	{
	case IE_Pressed:
	{
		UWRUIPanel_SocialMenu* Panel_SocialMenu = WRUIManager::Get()->Get()->FindUI<UWRUIPanel_SocialMenu>(EWRUIPanelKind::SocialMenu);
		if (Panel_SocialMenu && Panel_SocialMenu->OnInputEventReceived(EWRInput::DpadDown))
		{
			return true;
		}
	}
	break;
	case IE_Released:
	{

	}
	break;
	}

	return true; // 무조건
}

bool WRInputSystemManager::InputEventDpadLeft(EInputEvent InEventType)
{
	switch (InEventType)
	{
	case IE_Pressed:
	{
		UWRUIPanel_DanceMode* Panel_DanceMode = WRUIManager::Get()->FindUI<UWRUIPanel_DanceMode>(EWRUIPanelKind::DanceMode);
		if (Panel_DanceMode && Panel_DanceMode->GetCurFunctionType() == UWRUIPanel_DanceMode::EWRFunctionType::StartDanceMode)
		{
			Panel_DanceMode->PrevDanceKind();
			return true;
		}		

		UWRUIPanel_SocialMenu* Panel_SocialMenu = WRUIManager::Get()->Get()->FindUI<UWRUIPanel_SocialMenu>(EWRUIPanelKind::SocialMenu);
		if (Panel_SocialMenu && Panel_SocialMenu->OnInputEventReceived(EWRInput::DpadLeft))
		{
			return true;
		}

		WRInventoryManager::Get()->EquipNextSubWeapon();
	}
	break;
	case IE_Released:
	{

	}
	break;
	}

	return true; // 무조건
}

bool WRInputSystemManager::InputEventDpadRight(EInputEvent InEventType)
{
	switch (InEventType)
	{
	case IE_Pressed:
	{
		UWRUIPanel_DanceMode* Panel_DanceMode = WRUIManager::Get()->FindUI<UWRUIPanel_DanceMode>(EWRUIPanelKind::DanceMode);
		if (Panel_DanceMode && Panel_DanceMode->GetCurFunctionType() == UWRUIPanel_DanceMode::EWRFunctionType::StartDanceMode)
		{
			Panel_DanceMode->NextDanceKind();
			return true;
		}

		UWRUIPanel_SocialMenu* Panel_SocialMenu = WRUIManager::Get()->Get()->FindUI<UWRUIPanel_SocialMenu>(EWRUIPanelKind::SocialMenu);
		if (Panel_SocialMenu && Panel_SocialMenu->OnInputEventReceived(EWRInput::DpadRight))
		{
			return true;
		}

		WRInventoryManager::Get()->EquipNextMainWeapon();
		
	}
	break;
	case IE_Released:
	{

	}
	break;
	}

	return true; // 무조건
}

bool WRInputSystemManager::InputEventShareBtn(EInputEvent InEventType)
{
	switch (InEventType)
	{
	case IE_Pressed:
	{

	}
	break;
	case IE_Released:
	{

	}
	break;
	}

	return false;
}

bool WRInputSystemManager::InputEventTouchBtn(EInputEvent InEventType)
{	
	switch (InEventType)
	{
	case IE_Pressed:
	{
	}
	break;
	case IE_Released:
	{
	}
	break;
	}

	return false;
}

bool WRInputSystemManager::InputEventOptionBtn(EInputEvent InEventType)
{
	switch (InEventType)
	{
	case IE_Pressed:
	{

	}
	break;
	case IE_Released:
	{
		// [ 2019-11-15 : kyu ] 튜토리얼 스킵용
		if (WRTutorialManager::Get()->IsDuringTutorial())
		{
			WRTutorialManager::Get()->OnSkip();
		}
		else
		{
			//WRQuestManager::Get()->OpenQuestBoard();

			// [ 2020-9-3 : kyu ] 옵션버튼은 밑에서 인벤토리 오픈에 사용된다
// 			if (WRUIManager::Get()->IsWidgetCreating(EWRUIPanelKind::Menu) == false)
// 			{
// 				FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
// 				{
// 					if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
// 					{
// 						InCombinedPanelData.WidgetComponentBase->SetTickWhenOffscreen(true);
// 					}
// 				});
// 
// 				FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Menu, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
// 				WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 			}
		}
	}
	break;
	}

	return false;
}

bool WRInputSystemManager::InputEventRThumbStickBtn(EInputEvent InEventType)
{
	
	return false;
}

bool WRInputSystemManager::InputEventTouchPadDrag(FVector2D MoveDistance)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;

	EWRInput InputEventType = EWRInput::None;
	if (FMath::Abs(MoveDistance.X) > FMath::Abs(MoveDistance.Y))
	{
		// [ 2020-7-7 : kyu ] Compare X
		float DistanceXValue = WRConsoleManager::Get()->GetFloat(EWRConsole::PAD_DragXValue);
		if (MoveDistance.X >= DistanceXValue)
		{
			UE_LOG(WRInputSystem, Display, TEXT("WRInputSystemManager::InputEventTouchPadDrag - TouchPadDragLeftX"));
			InputEventType = EWRInput::TouchPadDragRightX;
		}
		else if (MoveDistance.X <= -DistanceXValue)
		{
			UE_LOG(WRInputSystem, Display, TEXT("WRInputSystemManager::InputEventTouchPadDrag - TouchPadDragRightX"));
			InputEventType = EWRInput::TouchPadDragLeftX;
		}
	}
	else
	{
		// [ 2020-7-7 : kyu ] Compare Y
		float DistanceYValue = WRConsoleManager::Get()->GetFloat(EWRConsole::PAD_DragYValue);
		if (MoveDistance.Y >= DistanceYValue)
		{
			UE_LOG(WRInputSystem, Display, TEXT("WRInputSystemManager::InputEventTouchPadDrag - TouchPadDragUpY"));
			InputEventType = EWRInput::TouchPadDragUpY;
		}
		else if (MoveDistance.Y <= -DistanceYValue)
		{
			UE_LOG(WRInputSystem, Display, TEXT("WRInputSystemManager::InputEventTouchPadDrag - TouchPadDragDownY"));
			InputEventType = EWRInput::TouchPadDragDownY;
		}
	}
	
	switch (InputEventType)
	{
	case EWRInput::TouchPadDragUpY:
	case EWRInput::TouchPadDragDownY:
	{
		UWRActorComponentAttack* AttackComponent = MyChar->GetComponent<UWRActorComponentAttack>();
		if (AttackComponent)
		{
			AttackComponent->PressBtn(InputEventType);
		}
	}
	break;
	case EWRInput::TouchPadDragLeftX:
	case EWRInput::TouchPadDragRightX:
	{
		if (WRInventoryManager::Get()->OnInputEvent(InputEventType))
		{
			return true;
		}
		if (WRVoiceContentsManager::Get()->OnInputKeyForAgreeSecretDialog(InputEventType))
		{
			return true;
		}
	}
	break;
	default:
	{
		return false;
	}
	break;
	}

	return true; // 무조건
}

bool WRInputSystemManager::InputEventMotion(EWRPadMotionType InMotionType)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return false;
	
	if (InMotionType == EWRPadMotionType::PULL)
	{
		UWRActorComponentAttack* AttackComponent = MyChar->GetComponent<UWRActorComponentAttack>();
		if (AttackComponent)
		{
			AttackComponent->PressBtn(EWRInput::JoystickPull);
		}

		if (MyChar->GetTargetAID() != INVALID_AID)
		{
			AWRCharacter* pFoundCharacter = WRCharacterManager::Get()->FindbyAID(MyChar->GetTargetAID());
			if (pFoundCharacter && pFoundCharacter->IsValidLowLevel() && pFoundCharacter->IsIO())
			{
				pFoundCharacter->OnPadEvent(EWRInput::JoystickPull);
			}
		}

		// [ 2019-11-14 : kyu ] 튜토리얼 이벤트 (키입력)
		WRTutorialManager::Get()->OnEvent(EWRInput::JoystickPull);
	}
	else if (InMotionType == EWRPadMotionType::PUSH)
	{
		if (MyChar->GetTargetAID() != INVALID_AID)
		{
			AWRCharacter* pFoundCharacter = WRCharacterManager::Get()->FindbyAID(MyChar->GetTargetAID());
			if (pFoundCharacter && pFoundCharacter->IsValidLowLevel() && pFoundCharacter->IsIO())
			{
				pFoundCharacter->OnPadEvent(EWRInput::JoystickPush);
			}
		}

		// [ 2019-11-14 : kyu ] 튜토리얼 이벤트 (키입력)
		WRTutorialManager::Get()->OnEvent(EWRInput::JoystickPush);
	}

	return true; // 무조건
}

void WRInputSystemManager::InputEvnetToContents(EWRInput InInputType, EInputEvent InEventType)
{
	// [ 2019-11-14 : kyu ] 튜토리얼 이벤트 (키입력)
	if (InEventType == IE_Released)
	{
		WRTutorialManager::Get()->OnEvent(InInputType);
	}

	// [ 2019-10-7 : kyu ] + 디폴트 동작
	switch (InInputType)
	{
	case EWRInput::L1: // 타겟팅
	case EWRInput::L2: // 보조무기 사용
	case EWRInput::L3: // 점프
	case EWRInput::R3: // 180도 회전
	case EWRInput::Option: // 메뉴/화면정렬
	case EWRInput::Share: // 
	case EWRInput::DpadUP: // 예비용
	{
	}
	break;
	default:
	{
		WRCharacterManager::Get()->CutOffMyHookChain(false, true);
	}
	break;
	}

	// [ 2019-10-7 : kyu ] + 디폴트 동작
	switch (InInputType)
	{
	case EWRInput::L1:
	case EWRInput::R1:
	case EWRInput::Option: // 메뉴/화면정렬
	{
	}
	break;
	default:
	{
		WRObjectInteractionManager::Get()->CancelLadder();
	}
	break;
	}
}

void WRInputSystemManager::InputEventLShiftBtn(EInputEvent InEventType)
{
	if (InEventType == IE_Pressed)
	{
		bPressedLShift = true;
	}
	else if (InEventType == IE_Released)
	{
		bPressedLShift = false;
	}
}


