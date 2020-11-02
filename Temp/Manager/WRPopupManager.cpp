
#include "WRPopupManager.h"
#include "WRUImanager.h"
#include "WRCharacterManager.h"
#include "WRCheersManager.h"

#include "Core/WRGameInstance.h"
#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterIO.h"
#include "WidgetComponent/WRWidgetComponentBase.h"

#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

#include "UI/Panel/WRUIPanel_Popup.h"
#include "UI/Panel/WRUIPanel_IOPopup.h"
#include "UI/Panel/WRUIPanel_Cheat.h"
#include "Enum/EWRUIEnum.h"


//====================================================================================
// [ 2019-8-16 : kyu ] Manager
//====================================================================================

WRPopupManager::WRPopupManager()
{
}

WRPopupManager::~WRPopupManager()
{
}

// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRPopupManager::OnInitialize()
{

}

// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRPopupManager::OnShutdown()
{
 	ClosePopupAll();
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRPopupManager::OnLoadLevelStart()
{

}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRPopupManager::OnLoadLevelComplete()
{
	
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRPopupManager::OnRenderStart()
{

}

//====================================================================================

void WRPopupManager::OnCompletedOpenPopup(const FWRCombinedPanelData& IN InPanelData, const FWRPopupInfo IN InInfo)
{
	if (InPanelData.UIPanel == nullptr)
		return;

	if (InPanelData.WidgetComponentBase == nullptr)
		return;

	UWRUIPanel_Popup* CastedPopup = Cast<UWRUIPanel_Popup>(InPanelData.UIPanel);
	if (CastedPopup != nullptr)
	{
		CastedPopup->SetPopupInfo(InInfo);
	}
}

//====================================================================================

void WRPopupManager::OpenPopup(FWRPopupInfo& IN InInfo, FTransform& IN InTransform)
{
	// + UI Exception
	if (WRCheersManager::Get()->PauseCheersContents())
	{
		// [ 2020-4-20 : kyu ] 건배상태 체크 및 해제
	}

	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::CostumeChange))
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::CostumeChange);
	}
 
	FWRUILoadCompletedDelegate Delegate;
	Delegate.BindRaw(this, &WRPopupManager::OnCompletedOpenPopup, InInfo);
	
	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Popup, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WidgetInfo.SetTransform(InTransform);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}
 
void WRPopupManager::OpenPopupOk(FTransform& IN InTransform, FString IN InContents, FOnPopupButtonDelegate IN InFirstDelegate, EPopupOptionFlag IN InOptionFlag /*= EPopupOptionFlag::None*/)
{
	FWRPopupInfo PopupInfo;
	PopupInfo.eButtonFlag = EPopupButtonFlag::OK;
	PopupInfo.Contents = InContents;
	PopupInfo.Delegate_First = InFirstDelegate;
	OpenPopup(PopupInfo, InTransform);
}
 
void WRPopupManager::OpenPopupOkCancel(FTransform& IN InTransform, FString IN InContents, FOnPopupButtonDelegate IN InFirstDelegate, FOnPopupButtonDelegate IN InSecondDelegate, EPopupOptionFlag IN InOptionFlag /*= EPopupOptionFlag::None*/)
{
	FWRPopupInfo PopupInfo;
	PopupInfo.eButtonFlag = EPopupButtonFlag::OK_CANCEL;
	PopupInfo.eOptionFlag = (BitFlag8)InOptionFlag;
	PopupInfo.Contents = InContents;
	PopupInfo.Delegate_First = InFirstDelegate;
	PopupInfo.Delegate_Second = InSecondDelegate;
	OpenPopup(PopupInfo, InTransform);
}

void WRPopupManager::OpenPopupScreen(FWRPopupInfo& IN InInfo)
{
	FWRUILoadCompletedDelegate Delegate;
	Delegate.BindRaw(this, &WRPopupManager::OnCompletedOpenPopup, InInfo);

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Popup, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}

//====================================================================================

void WRPopupManager::ClosePopup(const WRUIHandle IN InUIHandle, bool OUT Out2D /* = false */)
{
	FWRCombinedPanelData* FoundData = WRUIManager::Get()->FindUIData(InUIHandle);
	if (FoundData != nullptr)
	{
		Out2D = !FoundData->Is3DWidget();
	}
	WRUIManager::Get()->CloseUI(InUIHandle);

	// + UI Exception
	if (WRCheersManager::Get()->AttemptToOpenCheersContents())
	{
		// [ 2020-4-20 : kyu ] 건배상태 체크 및 설정
	}
}

void WRPopupManager::ClosePopup()
{
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::Popup) == false)
		return;

	WRUIManager::Get()->CloseUI(EWRUIPanelKind::Popup);

	// + UI Exception
	if (WRCheersManager::Get()->AttemptToOpenCheersContents())
	{
		// [ 2020-4-20 : kyu ] 건배상태 체크 및 설정
	}
}

void WRPopupManager::ClosePopupAll()
{
	// + UI Exception
	if (WRCheersManager::Get()->AttemptToOpenCheersContents())
	{
		// [ 2020-4-20 : kyu ] 건배상태 체크 및 설정
	}
}
 
UWRUIPanel_Popup* WRPopupManager::FindPopupWidget(const WRUIHandle IN InUIHandle)
{
	FWRCombinedPanelData* FoundPanelData = WRUIManager::Get()->FindUIData(InUIHandle);
	if (FoundPanelData != nullptr)
	{
		UWRUIPanel_Popup* CastedWidget = Cast<UWRUIPanel_Popup>(FoundPanelData->UIPanel);
		return CastedWidget;
	}
	return nullptr;
}
 
UWRWidgetComponentBase* WRPopupManager::FindPopupWidgetComponent(const WRUIHandle IN InUIHandle)
{
	FWRCombinedPanelData* FoundPanelData = WRUIManager::Get()->FindUIData(InUIHandle);
	if (FoundPanelData != nullptr)
	{
		UWRWidgetComponentBase* CastedComponent = Cast<UWRWidgetComponentBase>(FoundPanelData->WidgetComponentBase);
		return CastedComponent;
	}
	return nullptr;
}


//====================================================================================
//	Event Test
//====================================================================================

void WRPopupManager::EventCalled()
{
	//EventCall_Test_3DPopup();
}
 
void WRPopupManager::EventCall_Test_3DPopup()
{
	//FWRPopupInfo PopupInfo(
	//	EPopupButtonFlag::OK_CANCEL
	//	, EPopupOptionFlag::Type3D
	//	, FText::FromString(TEXT("Kyu Title"))
	//	, FText::FromString(TEXT("Kyu Contents\n1234\n123"))
	//	, FOnPopupButtonDelegate::CreateRaw(this, &WRPopupManager::OnClickedFirst)
	//	, FOnPopupButtonDelegate::CreateRaw(this, &WRPopupManager::OnClickedSecond)
	//);

	//FTransform SpawnTransform = GetMyFrontTransform();;
	//OpenPopup(PopupInfo, SpawnTransform);
}
 
void WRPopupManager::FocusOnUI(UWidget* IN InWidget)
{
	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
	{
 		return;
	}
	PlayerController->bShowMouseCursor = true;
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController);
}
 
void WRPopupManager::FocusOnGame()
{
	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
	{
		return;
	}
	PlayerController->bShowMouseCursor = false;
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
}
 
FTransform WRPopupManager::GetMyFrontTransform()
{
	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
 
// 	FVector ViewportLocation;
// 	FRotator ViewportRoration;
// 	PlayerController->GetActorEyesViewPoint(ViewportLocation, ViewportRoration);
 
	AActor* ViewTarget = PlayerController->GetViewTarget();
	FVector ViewLocation = ViewTarget->GetActorLocation();
	FVector ViewForward = ViewTarget->GetActorForwardVector();
	FRotator ViewRotation = ViewTarget->GetActorRotation();
 
	float ViewHeight = ViewTarget->GetSimpleCollisionHalfHeight();
	FVector SpawnLocation = ViewLocation + ViewForward * 500 + FVector(0, 0, ViewHeight);
	FVector InverseVector = ViewLocation - SpawnLocation;
 
	FRotator SpawnRotation = InverseVector.Rotation();
 
	FTransform WidgetTransform = UKismetMathLibrary::MakeTransform(SpawnLocation, SpawnRotation, FVector(1, 1, 1));
//	FRotator RevertRoration = UKismetMathLibrary::InverseTransformRotation(WidgetTransform, ViewportRoration);
 
	FTransform SpawnTransform;
// 	SpawnTransform.SetLocation(ViewportLocation);
// 	FQuat ViewportQuat = FQuat(ViewportRoration);
// 	FQuat MulQuat = FQuat(0, 0, 0, 0);
// 	SpawnTransform.SetRotation(ViewportQuat * MulQuat);
 
	SpawnTransform = WidgetTransform;
 
	return SpawnTransform;
}

//====================================================================================

