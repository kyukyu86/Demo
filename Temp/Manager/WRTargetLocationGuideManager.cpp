// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRTargetLocationGuideManager.h"
#include "WRCharacterManager.h"
#include "Actor/Character/WRCharacter.h"
#include "Utility/WRActorUtility.h"
#include "Table/WRPlayerTable.h"
#include "Table/Base/WRTableManager.h"
#include "WRInputSystemManager.h"
#include "WRConsoleManager.h"
#include "WRUIManager.h"
#include "UI/Panel/WRUIPanel_TargetLocationGuide.h"
#include "Kismet/KismetSystemLibrary.h"



//========================================================================================================================================================================
WRTargetLocationGuideManager::WRTargetLocationGuideManager()
{
}

WRTargetLocationGuideManager::~WRTargetLocationGuideManager()
{
}
	
void WRTargetLocationGuideManager::OnInitialize()
{
	
}

void WRTargetLocationGuideManager::OnShutdown()
{
	Clear();
}

void WRTargetLocationGuideManager::OnLoadLevelStart()
{
}

void WRTargetLocationGuideManager::OnLoadLevelComplete()
{
		
}

void WRTargetLocationGuideManager::OnRenderStart()
{
	
}

bool WRTargetLocationGuideManager::OnTick(float InDeltaTime)
{
	/*for (auto& Iter : TargetLocationInfoMap)
	{
		WRTargetLocationGuideHandle TargetLocationGuideHandle = Iter.Key;
		FWRTargetLocationInfoBase* TargetLocationInfoBase = Iter.Value;
		if (TargetLocationInfoBase == nullptr)
			continue;

		UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
		UKismetSystemLibrary::DrawDebugSphere(World, TargetLocationInfoBase->GetTargetLocation(), 100, 100, FLinearColor::Red);
	}*/

	// 삭제 예약된 TargetLocation 들 삭제
	TDoubleLinkedList<WRTargetLocationGuideHandle>::TDoubleLinkedListNode* Node = ReservedRemoveHandleList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRTargetLocationGuideHandle Handle = Node->GetValue();
		if(TargetLocationInfoMap.Contains(Handle) == false)
			continue;

		TargetLocationInfoMap[Handle]->OnAutoRemovedDelegate.ExecuteIfBound();
		delete TargetLocationInfoMap[Handle];
		TargetLocationInfoMap.Remove(Handle);
	}
	ReservedRemoveHandleList.Empty();
	/////////////////////////////////////////////////

	for (auto Iter : TargetLocationInfoMap)
	{
		WRTargetLocationGuideHandle TargetLocationGuideHandle = Iter.Key;
		FWRTargetLocationInfoBase* TargetLocationInfoBase = Iter.Value;
		if (TargetLocationInfoBase == nullptr)
			continue;

		if(TargetLocationInfoBase->SearchedEffectCoolTime > 0.f)
			TargetLocationInfoBase->SearchedEffectCoolTime -= InDeltaTime;
	}
	
	FlowCheckingTime += InDeltaTime;
	//if (FlowCheckingTime > 0.5f)
	{
		CheckEnteredSearchingArea();
		FlowCheckingTime = 0.f;
	}	
	
	return true;
}

void WRTargetLocationGuideManager::EndLoadLevelProcess()
{
	
}

WRTargetLocationGuideHandle WRTargetLocationGuideManager::AddTargetLocationInfo(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const EWRUISlotKind IN InUISlotKind, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist /*= 0.f*/)
{
	return AddTargetLocationInfo_Internal(InMapTID, InTargetPosition, InUISlotKind, InOnAutoRemovedDelegate, bInDeleteWhenEnterSearchingArea, InShowDist);
}

WRTargetLocationGuideHandle WRTargetLocationGuideManager::AddTargetLocationInfo(const WRTableID IN InMapTID, AActor* IN InTargetActor, const EWRUISlotKind IN InUISlotKind, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist /*= 0.f*/, const FVector& IN InOffset /*= FVector::ZeroVector*/)
{
	return AddTargetLocationInfo_Internal(InMapTID, InTargetActor, InUISlotKind, InOnAutoRemovedDelegate, bInDeleteWhenEnterSearchingArea, InShowDist, InOffset);
}

void WRTargetLocationGuideManager::RemoveTargetLocationInfo(const WRTargetLocationGuideHandle IN InHandle)
{
	if (TargetLocationInfoMap.Contains(InHandle) == false)
		return;

	FWRTargetLocationInfoBase* FoundTargetLocationInfo = TargetLocationInfoMap.FindRef(InHandle);
	if (FoundTargetLocationInfo == nullptr)
	{
		TargetLocationInfoMap.Remove(InHandle);
		ensureMsgf(false, TEXT("FoundTargetLocationInfo  nullptr!!"));
		return;
	}	

	// FoundTargetLocationInfo->bShowUI && 
	if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::TargetLocationGuide))
	{
		UWRUIPanel_TargetLocationGuide* Panel_TargetLocationGuide = WRUIManager::Get()->FindUI<UWRUIPanel_TargetLocationGuide>(EWRUIPanelKind::TargetLocationGuide);
		if (Panel_TargetLocationGuide)
		{
			Panel_TargetLocationGuide->RemoveTargetLocationInfo(InHandle);
		}
	}

	delete FoundTargetLocationInfo;
	TargetLocationInfoMap.Remove(InHandle);
}

void WRTargetLocationGuideManager::Clear()
{
	for (auto Iter : TargetLocationInfoMap)
	{
		FWRTargetLocationInfoBase* TargetLocationBase = Iter.Value;
		if (TargetLocationBase == nullptr)
			continue;

		delete TargetLocationBase;
	}

	TargetLocationInfoMap.Empty();

	if (WRUIManager::IsValid())
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::TargetLocationGuide);
}

WRTargetLocationGuideHandle WRTargetLocationGuideManager::GenerateTargetLocationGuideHandle()
{
	static WRTargetLocationGuideHandle LastHandle = 0;
	if (++LastHandle < 0)
	{
		ensureMsgf(false, TEXT("Invalid LastHandle"));
		return INVALID_TARGET_LOCATION_GUIDE_HANDLE;
	};

	return LastHandle;
}

WRTargetLocationGuideHandle WRTargetLocationGuideManager::AddTargetLocationInfo_Internal(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const EWRUISlotKind IN InUISlotKind, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist)
{
	if (InMapTID == INVALID_TABLE_TID)
		return INVALID_TARGET_LOCATION_GUIDE_HANDLE;

	WRTargetLocationGuideHandle GeneratedHandle = GenerateTargetLocationGuideHandle();
	if (GeneratedHandle == INVALID_TARGET_LOCATION_GUIDE_HANDLE)
		return INVALID_TARGET_LOCATION_GUIDE_HANDLE;

	FWRTargetLocationInfoForPosition* CreatedTargetLocationInfoForPosition = new FWRTargetLocationInfoForPosition(InTargetPosition, InUISlotKind, InMapTID, InOnAutoRemovedDelegate, bInDeleteWhenEnterSearchingArea, InShowDist);
	if (CreatedTargetLocationInfoForPosition == nullptr)
		return INVALID_TARGET_LOCATION_GUIDE_HANDLE;

	TargetLocationInfoMap.Add(GeneratedHandle, CreatedTargetLocationInfoForPosition);

	if (CheckEnteredSearchingAreaForPosition(CreatedTargetLocationInfoForPosition))
	{
		OnSearched(GeneratedHandle, CreatedTargetLocationInfoForPosition);
	}

	return GeneratedHandle;
}

WRTargetLocationGuideHandle WRTargetLocationGuideManager::AddTargetLocationInfo_Internal(const WRTableID IN InMapTID, AActor* IN InTargetActor, const EWRUISlotKind IN InUISlotKind, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist, const FVector& IN InOffset)
{
	if (InMapTID == INVALID_TABLE_TID)
		return INVALID_TARGET_LOCATION_GUIDE_HANDLE;

	WRTargetLocationGuideHandle GeneratedHandle = GenerateTargetLocationGuideHandle();
	if (GeneratedHandle == INVALID_TARGET_LOCATION_GUIDE_HANDLE)
		return INVALID_TARGET_LOCATION_GUIDE_HANDLE;

	FWRTargetLocationInfoForActor* CreatedTargetLocationInfoForActor = new FWRTargetLocationInfoForActor(InTargetActor, InUISlotKind, InMapTID, InOnAutoRemovedDelegate, bInDeleteWhenEnterSearchingArea, InShowDist, InOffset);
	if (CreatedTargetLocationInfoForActor == nullptr)
		return INVALID_TARGET_LOCATION_GUIDE_HANDLE;

	TargetLocationInfoMap.Add(GeneratedHandle, CreatedTargetLocationInfoForActor);

	if (CheckEnteredSearchingAreaForActor(CreatedTargetLocationInfoForActor))
	{
		OnSearched(GeneratedHandle, CreatedTargetLocationInfoForActor);
	}

	return GeneratedHandle;
}

bool WRTargetLocationGuideManager::CheckEnteredSearchingArea()
{
	bool bShowPanel = false;
	for (auto Iter : TargetLocationInfoMap)
	{
		WRTargetLocationGuideHandle TargetLocationGuideHandle = Iter.Key;
		FWRTargetLocationInfoBase* TargetLocationInfoBase = Iter.Value;
		if (TargetLocationInfoBase == nullptr)
			continue;

		bool bShowUI = false;
		if (CheckVisibilityDist(TargetLocationInfoBase))
		{
			bool bEntered = false;
			if (TargetLocationInfoBase->Type == FWRTargetLocationInfoBase::EWRTargetLocationInfoType::Actor)
			{
				bEntered = CheckEnteredSearchingAreaForActor(static_cast<const FWRTargetLocationInfoForActor*>(TargetLocationInfoBase));
			}
			else if (TargetLocationInfoBase->Type == FWRTargetLocationInfoBase::EWRTargetLocationInfoType::Position)
			{
				bEntered = CheckEnteredSearchingAreaForPosition(static_cast<const FWRTargetLocationInfoForPosition*>(TargetLocationInfoBase));
			}

			bShowUI = !bEntered;
		}
		else
		{
			bShowUI = false;
		}

		if (bShowUI)
		{
			bShowPanel = true;
			if(WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::TargetLocationGuide) == false)
				break;
		}

		if (TargetLocationInfoBase->bShowUI != bShowUI)
		{
			if (bShowUI)
			{
				UWRUIPanel_TargetLocationGuide* Panel_TargetLocationGuide = WRUIManager::Get()->FindUI<UWRUIPanel_TargetLocationGuide>(EWRUIPanelKind::TargetLocationGuide);
				if (Panel_TargetLocationGuide == nullptr)
					continue;

				Panel_TargetLocationGuide->AddTargetLocationInfo(TargetLocationGuideHandle, TargetLocationInfoBase);
				TargetLocationInfoBase->bShowUI = true;
			}
			else
			{
				OnSearched(TargetLocationGuideHandle, TargetLocationInfoBase);
				UWRUIPanel_TargetLocationGuide* Panel_TargetLocationGuide = WRUIManager::Get()->FindUI<UWRUIPanel_TargetLocationGuide>(EWRUIPanelKind::TargetLocationGuide);
				if (Panel_TargetLocationGuide == nullptr)
					continue;

				Panel_TargetLocationGuide->RemoveTargetLocationInfo(TargetLocationGuideHandle);
				TargetLocationInfoBase->bShowUI = false;
			}
		}
	}

	if (bShowPanel && WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::TargetLocationGuide) == false)
	{
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::TargetLocationGuide, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
	if (bShowPanel == false && WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::TargetLocationGuide))
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::TargetLocationGuide);
	}

	return false;
}

bool WRTargetLocationGuideManager::CheckEnteredSearchingAreaForActor(const FWRTargetLocationInfoForActor* IN InTargetLocationInfoForActor)
{
	if (InTargetLocationInfoForActor == nullptr)
		return false;

	if (InTargetLocationInfoForActor->TargetActor == nullptr)
		return false;

	AWRCharacter* MyCharacter = Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter());
	if (MyCharacter == nullptr)
		return false;

	FVector DirVec = InTargetLocationInfoForActor->TargetActor->GetActorLocation() - MyCharacter->GetActorLocation();
	FVector HMDForwardVector = WRInputSystemManager::Get()->GetHMDForwardVector();
	float Degree = WRActorUtility::GetDegreesBetween2Vectors(HMDForwardVector, DirVec);
	if (FMath::Abs(Degree) > WRConsoleManager::Get()->GetFloat(EWRConsole::TargetLocationGuide_Degree))
		return false;
	
	return true;
}

bool WRTargetLocationGuideManager::CheckEnteredSearchingAreaForPosition(const FWRTargetLocationInfoForPosition* IN InTargetLocationInfoForPosition)
{
	if (InTargetLocationInfoForPosition == nullptr)
		return false;

	AWRCharacter* MyCharacter = Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter());
	if (MyCharacter == nullptr)
		return false;

	FVector DirVec = InTargetLocationInfoForPosition->TargetPosition - MyCharacter->GetActorLocation();
	FVector HMDForwardVector = WRInputSystemManager::Get()->GetHMDForwardVector();
	float Degree = WRActorUtility::GetDegreesBetween2Vectors(HMDForwardVector, DirVec);
	if (FMath::Abs(Degree) > WRConsoleManager::Get()->GetFloat(EWRConsole::TargetLocationGuide_Degree))
		return false;

	return true;
}

bool WRTargetLocationGuideManager::CheckVisibilityDist(const FWRTargetLocationInfoBase* IN InTargetLocationInfoBase)
{
	if (InTargetLocationInfoBase == nullptr)
		return false;

	if (InTargetLocationInfoBase->ShowDist == 0.f)
		return true;

	if (InTargetLocationInfoBase->Type == FWRTargetLocationInfoBase::EWRTargetLocationInfoType::Actor)
	{
		return CheckVisibilityDistForActor(static_cast<const FWRTargetLocationInfoForActor*>(InTargetLocationInfoBase));
	}
	else if (InTargetLocationInfoBase->Type == FWRTargetLocationInfoBase::EWRTargetLocationInfoType::Position)
	{
		return CheckVisibilityDistForPosition(static_cast<const FWRTargetLocationInfoForPosition*>(InTargetLocationInfoBase));
	}

	return false;
}

bool WRTargetLocationGuideManager::CheckVisibilityDistForActor(const FWRTargetLocationInfoForActor* IN InTargetLocationInfoForActor)
{
	if (InTargetLocationInfoForActor == nullptr)
		return false;

	if (InTargetLocationInfoForActor->TargetActor == nullptr)
		return false;

	AWRCharacter* MyCharacter = Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter());
	if (MyCharacter == nullptr)
		return false;

	float Dist = WRActorUtility::GetDistance(MyCharacter, InTargetLocationInfoForActor->TargetActor);
	if (Dist > InTargetLocationInfoForActor->ShowDist)
		return false;

	return true;
}

bool WRTargetLocationGuideManager::CheckVisibilityDistForPosition(const FWRTargetLocationInfoForPosition* IN InTargetLocationInfoForPosition)
{
	if (InTargetLocationInfoForPosition == nullptr)
		return false;

	AWRCharacter* MyCharacter = Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter());
	if (MyCharacter == nullptr)
		return false;

	float Dist = WRActorUtility::GetDistance(MyCharacter, InTargetLocationInfoForPosition->TargetPosition);
	if (Dist > InTargetLocationInfoForPosition->ShowDist)
		return false;

	return true;
}

void WRTargetLocationGuideManager::OnSearched(const WRTargetLocationGuideHandle IN InHandle, FWRTargetLocationInfoBase* IN InTargetLocationInfoBase)
{
	if (InHandle == INVALID_TARGET_LOCATION_GUIDE_HANDLE)
		return;

	if (InTargetLocationInfoBase == nullptr)
		return;

	if (InTargetLocationInfoBase->SearchedEffectCoolTime > 0.f)
		return;

	FVector SpawnPosition = InTargetLocationInfoBase->GetTargetLocation();
	
	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::TargetLocationSearched, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth, nullptr);
	WidgetInfo.SetTransform(FTransform(SpawnPosition));
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

	if (InTargetLocationInfoBase->bDeleteWhenEnterSearchingArea)
	{
		ReservedRemoveHandleList.AddTail(InHandle);
	}

	InTargetLocationInfoBase->SearchedEffectCoolTime = WRConsoleManager::Get()->GetFloat(EWRConsole::TLG_SearchedEffectCoolTime);
}

FVector FWRTargetLocationInfoForActor::GetTargetLocation() const
{
	if (TargetActor == nullptr)
		return FVector::ZeroVector;

	return TargetActor->GetActorLocation() + Offset;
}

FVector FWRTargetLocationInfoForPosition::GetTargetLocation() const
{
	return TargetPosition;
}
