// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRCompassManager.h"
#include "WRUIManager.h"
#include "UI/Panel/WRUIPanel_Main.h"
#include "WRInputSystemManager.h"
#include "WRConsoleManager.h"
#include "WRMapManager.h"
#include "UI/Panel/WRUIPanel_CompassMark.h"



//========================================================================================================================================================================
WRCompassManager::WRCompassManager()
	: IgnoreDistCheckingTime(0.f)
{
}

WRCompassManager::~WRCompassManager()
{
}
	
void WRCompassManager::OnInitialize()
{

}

void WRCompassManager::OnShutdown()
{
	Clear();
}

void WRCompassManager::OnLoadLevelStart()
{
	for (auto Iter : CompassMarkMap)
	{
		FWRCompassMarkInfoBase* CompassMarkInfoBase = Iter.Value;
		if (CompassMarkInfoBase == nullptr)
			continue;

		CompassMarkInfoBase->bWaitCreateUI = false;
		CompassMarkInfoBase->UIHandle = INVALID_UI_HANDLE;
	}
	SetEnableTick(false);
}

void WRCompassManager::OnLoadLevelComplete()
{
	SetEnableTick(true);
}

void WRCompassManager::OnRenderStart()
{
	
}

bool WRCompassManager::OnTick(float InDeltaTime)
{
	CheckDisplay3DMark();

	if (IgnoreDistCheckingTime > 0)
	{
		IgnoreDistCheckingTime -= InDeltaTime;
	}

	return true;
}

void WRCompassManager::InitCompassUI()
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_Main* Panel_Main = Cast<UWRUIPanel_Main>(InCombinedPanelData.UIPanel);
			if (Panel_Main)
				Panel_Main->InitCompassUI();
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Main, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);	
}

WRCompassMarkHandle WRCompassManager::AddMark(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const int16 IN InZOffset)
{
	return AddMark_Internal(InMapTID, InTargetPosition, InZOffset, FWRCompassMarkInfoBase::EWRCompassMarkUIType::Question);
}

WRCompassMarkHandle WRCompassManager::AddMark(const WRTableID IN InMapTID, AActor* IN InActor, const FVector& IN InOffset)
{
	return AddMark_Internal(InMapTID, InActor, "", InOffset, FWRCompassMarkInfoBase::EWRCompassMarkUIType::Question);
}

WRCompassMarkHandle WRCompassManager::AddMark(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const int16 IN InZOffset, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType)
{
	return AddMark_Internal(InMapTID, InTargetPosition, InZOffset, InCompassMarkUIType);
}

WRCompassMarkHandle WRCompassManager::AddMark(const WRTableID IN InMapTID, AActor* IN InActor, const FVector& IN InOffset, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType)
{
	return AddMark_Internal(InMapTID, InActor, "", InOffset, InCompassMarkUIType);
}

WRCompassMarkHandle WRCompassManager::AddMark(const WRTableID IN InMapTID, AActor* IN InActor, const FName& IN InSocketName, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType)
{
	return AddMark_Internal(InMapTID, InActor, InSocketName, FVector::ZeroVector, InCompassMarkUIType);
}

void WRCompassManager::RemoveMark(const WRCompassMarkHandle IN InCompassMarkHandle)
{
	if (InCompassMarkHandle == INVALID_COMPASS_MARK_HANDLE)
		return;

	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::Main);
	if (CombinedPanelData && CombinedPanelData->UIPanel)
	{
		UWRUIPanel_Main* Panel_Main = Cast<UWRUIPanel_Main>(CombinedPanelData->UIPanel);
		if (Panel_Main)
			Panel_Main->RemoveMark(InCompassMarkHandle); 
	}

	const FWRCompassMarkInfoBase* CompassMarkInfoBase = CompassMarkMap.FindRef(InCompassMarkHandle);
	if (CompassMarkInfoBase)
	{
		WRUIManager::Get()->CloseUI(CompassMarkInfoBase->UIHandle);
		delete CompassMarkInfoBase;
	}

	CompassMarkMap.Remove(InCompassMarkHandle);
}

void WRCompassManager::StartIgnoreDistChecking(const float IN InIgnoreDistCheckingTime)
{
	if (InIgnoreDistCheckingTime <= 0)
		return;

	IgnoreDistCheckingTime = InIgnoreDistCheckingTime;
}

void WRCompassManager::CheckDisplay3DMark()
{
	for (auto Iter : CompassMarkMap)
	{
		WRCompassMarkHandle CompassMarkHandle = Iter.Key;
		FWRCompassMarkInfoBase* CompassMarkInfoBase = Iter.Value;
		if (CompassMarkInfoBase == nullptr)
			continue;

		if(CompassMarkInfoBase->bWaitCreateUI)
			continue;
		
		FWRCompassMarkInfoBase::EWRCompassMarkType CompassMarkType = CompassMarkInfoBase->GetCompassMarkType();
		if (CompassMarkType == FWRCompassMarkInfoBase::EWRCompassMarkType::Position)
		{
			CheckDisplay3DMarkForPosition(CompassMarkHandle, CompassMarkInfoBase);
		}
		else if (CompassMarkType == FWRCompassMarkInfoBase::EWRCompassMarkType::Actor)
		{
			CheckDisplay3DMarkForActor(CompassMarkHandle, CompassMarkInfoBase);
		}
		else
		{
			continue;
		}
	}
}

void WRCompassManager::CheckDisplay3DMarkForPosition(const WRCompassMarkHandle IN InCompassMarkHandle, FWRCompassMarkInfoBase* IN InCompassMarkInfoBase)
{
	if (InCompassMarkInfoBase == nullptr)
		return;

	const FWRCompassMarkInfoForPosition* CompassMarkInfoForPosition = static_cast<const FWRCompassMarkInfoForPosition*>(InCompassMarkInfoBase);
	if (CompassMarkInfoForPosition == nullptr)
		return;

	if (CompassMarkInfoForPosition->MapTID != WRMapManager::Get()->GetCurrentMapTID())
		return;

	FVector HMDLocation = WRInputSystemManager::Get()->GetHMDLocation();
	float Dist = 0;

	// True면 거리체크를 한다
	if (IgnoreDistCheckingTime <= 0)
	{
		Dist = FVector::Dist(HMDLocation, CompassMarkInfoForPosition->TargetPosition);

		if (Dist < WRConsoleManager::Get()->GetInt(EWRConsole::UI_3DMarkDIst))
		{
			if (InCompassMarkInfoBase->UIHandle != INVALID_UI_HANDLE)
				return;

			FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
			{				
				if (CompassMarkMap.Contains(InCompassMarkHandle) == false)
				{
					WRUIManager::Get()->CloseUI(InCombinedPanelData.UIHandle);
				}
				else
				{
					InCompassMarkInfoBase->bWaitCreateUI = false;
					InCompassMarkInfoBase->UIHandle = InCombinedPanelData.UIHandle;

					UWRUIPanel_CompassMark* Panel_CompassMark = Cast<UWRUIPanel_CompassMark>(InCombinedPanelData.UIPanel);
					if (Panel_CompassMark)
						Panel_CompassMark->SetCompassMarkInfo(InCompassMarkHandle, InCompassMarkInfoBase);
					//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::IO_TargetUIFixedSize) * 0.0001f);
				}
			});
			FVector SpawnPosition = CompassMarkInfoForPosition->TargetPosition;
			SpawnPosition.Z += CompassMarkInfoForPosition->ZOffset;

			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CompassMark, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
			WidgetInfo.SetTransform(FTransform(SpawnPosition));
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
			//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::CompassMark, FTransform(SpawnPosition), EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);

			InCompassMarkInfoBase->bWaitCreateUI = true;
		}
		else
		{
			if (InCompassMarkInfoBase->UIHandle == INVALID_UI_HANDLE)
				return;

			WRUIManager::Get()->CloseUI(InCompassMarkInfoBase->UIHandle);
			InCompassMarkInfoBase->UIHandle = INVALID_UI_HANDLE;
		}
	}
	else // 거리 체크 없이 보여진다.
	{
		if (InCompassMarkInfoBase->UIHandle != INVALID_UI_HANDLE)
			return;

		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (CompassMarkMap.Contains(InCompassMarkHandle) == false)
			{
				WRUIManager::Get()->CloseUI(InCombinedPanelData.UIHandle);
			}
			else
			{
				InCompassMarkInfoBase->bWaitCreateUI = false;
				InCompassMarkInfoBase->UIHandle = InCombinedPanelData.UIHandle;

				UWRUIPanel_CompassMark* Panel_CompassMark = Cast<UWRUIPanel_CompassMark>(InCombinedPanelData.UIPanel);
				if (Panel_CompassMark)
					Panel_CompassMark->SetCompassMarkInfo(InCompassMarkHandle, InCompassMarkInfoBase);
				//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::IO_TargetUIFixedSize) * 0.0001f);
			}
		});
		FVector SpawnPosition = CompassMarkInfoForPosition->TargetPosition;
		SpawnPosition.Z += CompassMarkInfoForPosition->ZOffset;

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CompassMark, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WidgetInfo.SetTransform(FTransform(SpawnPosition));
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::CompassMark, FTransform(SpawnPosition), EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		InCompassMarkInfoBase->bWaitCreateUI = true;
	}
}

void WRCompassManager::CheckDisplay3DMarkForActor(const WRCompassMarkHandle IN InCompassMarkHandle, FWRCompassMarkInfoBase* IN InCompassMarkInfoBase)
{
	if (InCompassMarkInfoBase == nullptr)
		return;

	const FWRCompassMarkInfoForActor* CompassMarkInfoForActor = static_cast<const FWRCompassMarkInfoForActor*>(InCompassMarkInfoBase);
	if (CompassMarkInfoForActor == nullptr)
		return;

	if (CompassMarkInfoForActor->TargetActor== nullptr)
		return;

	if (CompassMarkInfoForActor->MapTID != WRMapManager::Get()->GetCurrentMapTID())
		return;

	if (WRUIManager::Get()->IsWidgetOpened(CompassMarkInfoForActor->TargetActor, EWRUIPanelKind::SpeakDialogueIcon))
	{
		if (InCompassMarkInfoBase->UIHandle == INVALID_UI_HANDLE)
			return;

		WRUIManager::Get()->CloseUI(InCompassMarkInfoBase->UIHandle);
		InCompassMarkInfoBase->UIHandle = INVALID_UI_HANDLE;
		return;
	}

	FVector HMDLocation = WRInputSystemManager::Get()->GetHMDLocation();
	float Dist = 0;

	// True면 거리체크를 한다
	if (IgnoreDistCheckingTime <= 0)
	{
		FVector TargetPosition = CompassMarkInfoForActor->TargetActor->GetActorLocation();
		Dist = FVector::Dist(HMDLocation, TargetPosition);

		if (Dist < WRConsoleManager::Get()->GetInt(EWRConsole::UI_3DMarkDIst))
		{
			if (InCompassMarkInfoBase->UIHandle != INVALID_UI_HANDLE)
				return;

			FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
			{
				if (CompassMarkMap.Contains(InCompassMarkHandle) == false)
				{
					WRUIManager::Get()->CloseUI(InCombinedPanelData.UIHandle);
				}
				else
				{
					InCompassMarkInfoBase->bWaitCreateUI = false;
					InCompassMarkInfoBase->UIHandle = InCombinedPanelData.UIHandle;

					UWRUIPanel_CompassMark* Panel_CompassMark = Cast<UWRUIPanel_CompassMark>(InCombinedPanelData.UIPanel);
					if (Panel_CompassMark)
						Panel_CompassMark->SetCompassMarkInfo(InCompassMarkHandle, InCompassMarkInfoBase);
					//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::IO_TargetUIFixedSize) * 0.0001f);
					if(CompassMarkInfoForActor->SocketName.IsNone() == false)
						InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(CompassMarkInfoForActor->SocketName);
				}
			});
			
			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CompassMark, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCharacter, &Delegate, CompassMarkInfoForActor->TargetActor);
			WidgetInfo.SetTransform(FTransform(FRotator::ZeroRotator, CompassMarkInfoForActor->Offset, FVector::OneVector));
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
			//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::CompassMark, CompassMarkInfoForActor->TargetActor, FTransform(FRotator::ZeroRotator, CompassMarkInfoForCharacter->Offset, FVector::OneVector), EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCharacter, &Delegate);

			InCompassMarkInfoBase->bWaitCreateUI = true;
		}
		else
		{
			if (InCompassMarkInfoBase->UIHandle == INVALID_UI_HANDLE)
				return;

			WRUIManager::Get()->CloseUI(InCompassMarkInfoBase->UIHandle);
			InCompassMarkInfoBase->UIHandle = INVALID_UI_HANDLE;
		}
	}
	else // 거리 체크 없이 보여진다.
	{
		if (InCompassMarkInfoBase->UIHandle != INVALID_UI_HANDLE)
			return;

		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (CompassMarkMap.Contains(InCompassMarkHandle) == false)
			{
				WRUIManager::Get()->CloseUI(InCombinedPanelData.UIHandle);
			}
			else
			{
				InCompassMarkInfoBase->bWaitCreateUI = false;
				InCompassMarkInfoBase->UIHandle = InCombinedPanelData.UIHandle;

				UWRUIPanel_CompassMark* Panel_CompassMark = Cast<UWRUIPanel_CompassMark>(InCombinedPanelData.UIPanel);
				if (Panel_CompassMark)
					Panel_CompassMark->SetCompassMarkInfo(InCompassMarkHandle, InCompassMarkInfoBase);
				//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::IO_TargetUIFixedSize) * 0.0001f);
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CompassMark, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCharacter, &Delegate, CompassMarkInfoForActor->TargetActor);
		WidgetInfo.SetTransform(FTransform(FRotator::ZeroRotator, CompassMarkInfoForActor->Offset, FVector::OneVector));
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::CompassMark, CompassMarkInfoForCharacter->TargetCharacter, FTransform(FRotator::ZeroRotator, CompassMarkInfoForCharacter->Offset, FVector::OneVector), EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth | EWRUIWidgetCreationBitFlag::AttachToCharacter, &Delegate);
		InCompassMarkInfoBase->bWaitCreateUI = true;
	}
}

void WRCompassManager::Clear()
{
	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::Main);
	if (CombinedPanelData && CombinedPanelData->UIPanel)
	{
		UWRUIPanel_Main* Panel_Main = Cast<UWRUIPanel_Main>(CombinedPanelData->UIPanel);
		if (Panel_Main)
			Panel_Main->ClearCompassUI();
	}

	for (auto Iter : CompassMarkMap)
	{
		const FWRCompassMarkInfoBase* CompassMarkInfoBase = Iter.Value;
		if(CompassMarkInfoBase == nullptr)
			continue;

		if (CompassMarkInfoBase->UIHandle != INVALID_UI_HANDLE)
		{
			WRUIManager::Get()->CloseUI(CompassMarkInfoBase->UIHandle);
		}

		delete CompassMarkInfoBase;
	}

	CompassMarkMap.Empty();
}

WRCompassMarkHandle WRCompassManager::GenerateCompassMarkHandle()
{
	static WRCompassMarkHandle LastHandle = 0;
	if (++LastHandle < 0)
	{
		ensureMsgf(false, TEXT("Invalid LastHandle"));
		return INVALID_COMPASS_MARK_HANDLE;
	};

	return LastHandle;
}

WRCompassMarkHandle WRCompassManager::AddMark_Internal(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const int16 IN InZOffset, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType)
{
	WRCompassMarkHandle GeneratedHandle = GenerateCompassMarkHandle();
	if (GeneratedHandle == INVALID_COMPASS_MARK_HANDLE)
		return INVALID_COMPASS_MARK_HANDLE;

	FWRCompassMarkInfoBase* CreatedCompassMarkInfoBase = new FWRCompassMarkInfoForPosition(InMapTID, InTargetPosition, InZOffset, InCompassMarkUIType);
	if (CreatedCompassMarkInfoBase == nullptr)
		return INVALID_COMPASS_MARK_HANDLE;

	CompassMarkMap.Add(GeneratedHandle, CreatedCompassMarkInfoBase);

	if (InMapTID == WRMapManager::Get()->GetCurrentMapTID())
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (CompassMarkMap.Contains(GeneratedHandle) == false)
			{
				WRUIManager::Get()->CloseUI(InCombinedPanelData.UIHandle);
				return;
			}

			if (InCombinedPanelData.Is3DWidget())
			{
				UWRUIPanel_Main* Panel_Main = Cast<UWRUIPanel_Main>(InCombinedPanelData.UIPanel);
				if (Panel_Main)
					Panel_Main->AddMark(GeneratedHandle, CreatedCompassMarkInfoBase);
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Main, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}

	return GeneratedHandle;
}

WRCompassMarkHandle WRCompassManager::AddMark_Internal(const WRTableID IN InMapTID, AActor* IN InActor, const FName& IN InSocketName, const FVector& IN InOffset, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType)
{
	WRCompassMarkHandle GeneratedHandle = GenerateCompassMarkHandle();
	if (GeneratedHandle == INVALID_COMPASS_MARK_HANDLE)
		return INVALID_COMPASS_MARK_HANDLE;

	FWRCompassMarkInfoBase* CreatedCompassMarkInfoBase = new FWRCompassMarkInfoForActor(InMapTID, InActor, InOffset, InSocketName, InCompassMarkUIType);
	if (CreatedCompassMarkInfoBase == nullptr)
		return INVALID_COMPASS_MARK_HANDLE;

	CompassMarkMap.Add(GeneratedHandle, CreatedCompassMarkInfoBase);

	if (InMapTID == WRMapManager::Get()->GetCurrentMapTID())
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (CompassMarkMap.Contains(GeneratedHandle) == false)
			{
				WRUIManager::Get()->CloseUI(InCombinedPanelData.UIHandle);
				return;
			}

			if (InCombinedPanelData.Is3DWidget())
			{
				UWRUIPanel_Main* Panel_Main = Cast<UWRUIPanel_Main>(InCombinedPanelData.UIPanel);
				if (Panel_Main)
					Panel_Main->AddMark(GeneratedHandle, CreatedCompassMarkInfoBase);
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Main, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}

	return GeneratedHandle;
}

