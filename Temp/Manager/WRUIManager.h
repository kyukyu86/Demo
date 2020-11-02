// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include <Engine/StreamableManager.h>
#include "SingletonClass/WRSingleton.h"
#include "../WidgetComponent/WRWidgetComponentBase.h"
#include "Enum/EWRUIEnum.h"
#include "Define/WRTypeDefine.h"
#include "Define/WRUIDefine.h"
#include "Struct/WRUIStruct.h"
#include <Engine/AssetManager.h>
#include "WRUIPathManager.h"
#include "WRAsyncLoadUObjectManager.h"
#include "UserWidget.h"
#include "UI/Base/WRUIPanel.h"
#include "WRMapManager.h"
#include "Component/Custom/WRUIStereoLayerComponent.h"

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
	#include "Utility/WRActorUtility.h"
	#include "Table/Custom/WRWidgetTable.h"

	#include "Actor/Character/WRCharacter.h"
class UWRWidgetComponentBase;
class UWRUIPanel;
#endif // DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
#include "Core/WRGameInstance.h"
#include "WRConsoleManager.h"

class UWRUISlot;
struct FWRWidgetTable;

class WR_API WRUIManager : public WRSingleton<WRUIManager>
{
	CDECLARE_MANAGER(WRUIManager);

private:
	WRUIManager() {}
	virtual ~WRUIManager() {}

private:
	TMap<EWRUIPanelKind, FWRCombinedPanelData>		UIMap;						// [ 2019-8-27 : vuvle ] for unique UI
	TMap<WRUIHandle, FWRCombinedPanelData>			MultipleUIMap;				// [ 2019-8-27 : kyu ] for multiple UI (Popup, Nametag, IOui...)
	TDoubleLinkedList<TPair<EWRUIPanelKind, const AActor*>>	SavedCreatingStateList;
	TAssetPtr<UDataTable> WidgetTable = nullptr;
	FString LoadWidgetTableHash;

	bool bWidgetComponentVisibility = true;
	BitFlag16 DebugUIFlags = EWRDebugUIBitFlag::None;

#ifdef USE_STEREOLAYER_IN_UIMANAGER
	UPROPERTY()
		class UTextureRenderTarget2D*	WidgetRenderTarget = nullptr;
	UPROPERTY()
		class UMaterialInstanceDynamic* DynamicMaterial;
	UPROPERTY()
		class UMaterialInstanceDynamic* DynamicHiddenWidgetMaterial;
	UPROPERTY()
		class UMaterialInterface*		HiddenWidgetMaterial;
	UPROPERTY()
		class FWidgetRenderer*			WidgetRenderer = nullptr;
#endif // USE_STEREOLAYER_IN_UIMANAGER

#ifdef DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
#else // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
	TQueue<TSharedPtr<FStreamableHandle>>			StreamableHandleQueue;
#endif // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604

private:
	void Clear();
	WRUIHandle GenerateUIHandle();
#ifdef DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
	template <class TWidgetComponent>
	FString CreateUIAsync(const FWRWidgetInfo& IN InWidgetInfo);
#else // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
	template <class TWidgetComponent>
	void CreateUIAsync(const FWRWidgetInfo& IN InWidgetInfo);
#endif // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604 
	
	template <class TWidgetComponent>
	bool CreateWidgetComponent(UWRWidgetComponentBase*& OUT OutWRWidgetComponent, UWRUIPanel* IN InUIPanelToLink, const EWRUIPanelKind IN InUIKind);
	bool CreateStereoLayerComponent(class UWRUIStereoLayerComponent*& OUT OutStereoLayerComponent, UWRUIPanel* IN InUIPanelToLink, const FWRWidgetInfo& IN InWidgetInfo);
	bool CreateUI(const EWRUIPanelKind IN InUIKindToCreate, UWRUIPanel*& OUT OutCreatedPanel);
	const FWRCombinedPanelData& AddUIToContainer(const EWRUIPanelKind IN InUIKind, UWRUIPanel* IN InUIPanel, UWRWidgetComponentBase* IN InWidgetComponentBase = nullptr, UWRUIStereoLayerComponent* IN InStereoLayerComponent = nullptr);
	void SetComponentOptions(UWRUIPanel* IN InUIPanel, UWRWidgetComponentBase* IN InWidgetComponentBase);
	void RemoveCreatingState(const EWRUIPanelKind IN InUIPanelKind, const AActor* IN InOwnerCharacter);
	void LoadWidgetTable();

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnLoadLevelComplete() override {};
	virtual void OnLoadLevelStart() override;
	virtual void OnRenderStart() override {};
	virtual bool OnTick(float InDeltaTime);

	void OnDepartureEffectToMapCalled();	// 출발 이펙트가 호출됨

	UWRUISlot* CreateUISlot(const EWRUISlotKind IN InSlotKind);
	FString CreateUISlotAsync(const EWRUISlotKind IN InSlotKind, const FWRSlotUILoadCompletedDelegate  IN InDelegate);
	template <class TWidgetComponent>
#ifdef DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
	FString OpenUI(const FWRWidgetInfo& IN InWidgetInfo);
#else // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
	void OpenUI(const FWRWidgetInfo& IN InWidgetInfo);
#endif // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
	
	template <class TWidget>
	TWidget* CreateUISync_Casted(FString& IN InPath);
	UUserWidget* CreateUISync(FString& IN InPath);
	template <class TWidget>
	TWidget* CreateUIFullPathSync_Casted(FString& IN InFullPath);
	UUserWidget* CreateUIFullPathSync(FString& IN InFullPath);
	FString CreateUIFullPathASync(FString& IN InFullPath, const FWRSlotUILoadCompletedDelegate  IN InDelegate);

	void CancelUIAsync(const FString& IN InHash);

	void CloseUI(const EWRUIPanelKind IN InUIKind, const bool IN bCheckFadeOut = true);
	void CloseUI(const AActor* IN InCharacter, const EWRUIPanelKind IN InUIKind, const bool IN bCheckFadeOut = true);
	void CloseUI(const WRUIHandle IN InUIHandle, const bool IN bCheckFadeOut = true);
	void CloseAllUIFromCharacter(const AActor* IN InCharacter);
	void CloseAllMultipleUI(const EWRUIPanelKind IN InUIKind);
	void HideAllWidgetComponents();
	void ShowAllWidgetComponents();

	bool		IsWidgetOpened(const EWRUIPanelKind IN InUIKind);
	WRUIHandle	IsWidgetOpened(const AActor* IN InCharacter, const EWRUIPanelKind IN InUIKind);
	bool		IsWidgetCreating(const EWRUIPanelKind IN InUIPanelKind, const AActor* IN InOwnerCharacter = nullptr);

	template <class TWidget>
	TWidget* FindUI(const EWRUIPanelKind IN InUIKind);
	template <class TWidget>
	TWidget* FindUI(const AActor* IN InCharacter, const EWRUIPanelKind IN InUIKind);
	FWRCombinedPanelData* FindUIData(const WRUIHandle IN InUIHandle);
	FWRCombinedPanelData* FindUIData(const AActor* IN InCharacter, const EWRUIPanelKind IN InUIKind);
	FWRCombinedPanelData* FindUIData(const EWRUIPanelKind IN InUIKind);
	const FWRWidgetTable* GetWidgetCreationTable(const EWRUIPanelKind IN InUIKind);

	//====================================================================================
	void OpenSkillCoolTimeUI(const float IN InRemainTime);
	void EditDebugUIFlag(uint16 InFlag);
	void UpdateDebugUI(const FWRDebugUIData IN InData);
	//====================================================================================

#if WITH_EDITOR || PLATFORM_WINDOWS
private:
	class UWidget* SavedFocusWidget = nullptr;
public:
	void SaveFocusedUI(UWidget* IN InFocusWidget);
	void ReleaseFocusedUI();
	void FocusSavedUI();
#endif // UE_EDITOR

	//====================== Stereo Layer Test Code =======================================
	// [ 2019-9-6 : kyu ] Stereo Layer
private:
	UPROPERTY()
		class UStereoLayerComponent*	CreatedSLComponent = nullptr;
	UPROPERTY()
		class UTextureRenderTarget2D*	pRenderTarget = nullptr;
	UPROPERTY()
		class UTextureRenderTarget2D*	pRenderTarget2 = nullptr;
	UPROPERTY()
		class FWidgetRenderer*			pWidgetRenderer = nullptr;
	UPROPERTY()
		class UUserWidget*				pTestWidget = nullptr;
	UMaterialInstanceDynamic* Mat;
	TSharedPtr<SWidget>					StereoSlateWidget;
public:
	void OnStereoLayerInitialized();
	void InitializeStereoLayer();
	//====================================================================================

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
private:
	WRUIHandle GenerateUIHandle(const EWRUIPanelKind IN InPanelKind);
	template <class TWidgetComponent>
	void CreateUIAsync(const EWRUIPanelKind IN InUIKindToLoad, const EWRUISpaceType IN InUISpaceType, const AActor* IN InOwnerCharacter, const FTransform& IN InTransform, const BitFlag16 IN InWidgetComponentFlag, const FWRUILoadCompletedDelegate& IN InFirstDelegateToNotify, const FWRUILoadCompletedDelegate& IN InSecondDelegateToNotify, const bool IN bInSaveCreatingState);
	void CreateUIAsync(const EWRUIPanelKind IN InUIKindToLoad, const EWRUISpaceType IN InUISpaceType, const FWRUILoadCompletedDelegate& IN InDelegateToNotify, const bool IN bInSaveCreatingState);
#endif

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
	public:
	template <class TWidgetComponent>
	void OpenUI(const EWRUIPanelKind IN InUIKind, const FTransform& IN InTransform, const BitFlag16 IN InWidgetComponentFlag, const FWRUILoadCompletedDelegate* IN InFirstDelegate = nullptr, const FWRUILoadCompletedDelegate* IN InSecondDelegate = nullptr, const bool IN bInSaveCreatingState = false);
	template <class TWidgetComponent>
	void OpenUI(const EWRUIPanelKind IN InUIKind, const AActor* IN InOwnerCharacter, const FTransform& IN InOffsetTransform, const BitFlag16 IN InWidgetComponentFlag, const FWRUILoadCompletedDelegate* IN InFirstDelegate = nullptr, const FWRUILoadCompletedDelegate* IN InSecondDelegate = nullptr, const bool IN bInSaveCreatingState = false);
	void Open2DWidget(const EWRUIPanelKind IN InUIKind, const FWRUILoadCompletedDelegate* IN InDelegate = nullptr);
	// [ 2019-9-9 : kyu ] Test
	
#endif
};






template <class TWidgetComponent>
bool WRUIManager::CreateWidgetComponent(UWRWidgetComponentBase*& OUT OutWidgetComponent, UWRUIPanel* IN InUIPanelToLink, const EWRUIPanelKind IN InUIKind)	
{
	if (InUIPanelToLink == nullptr)
	{
		ensureMsgf(false, TEXT("Panel must be exist"));
		return false;
	}

	OutWidgetComponent = nullptr;

	UWRWidgetComponentBase* CreatedWidgetComponentBase = Cast<UWRWidgetComponentBase>(NewObject<TWidgetComponent>());

	if (CreatedWidgetComponentBase == nullptr)
	{
		ensureMsgf(false, TEXT("Widget Component create failed."));
		return false;
	}
	   
	if (CreatedWidgetComponentBase->IsRegistered() == false)
	{
		UWRGameInstance* GameInstance = UWRGameInstance::GetGameInstance();
		if (GameInstance == nullptr)
			return false;

		UWorld* World = GameInstance->GetWorld();
		if (World == nullptr)
			return false;

		CreatedWidgetComponentBase->RegisterComponentWithWorld(World);
	}
	
	CreatedWidgetComponentBase->AddToRoot();
	CreatedWidgetComponentBase->SetWidget(InUIPanelToLink);
	CreatedWidgetComponentBase->SetActive(true);
	if (bWidgetComponentVisibility == false)
		CreatedWidgetComponentBase->SetVisibility(false);


	OutWidgetComponent = CreatedWidgetComponentBase;

	return true;
}

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
template <class TWidgetComponent>
void WRUIManager::OpenUI(const EWRUIPanelKind IN InUIKind, const FTransform& IN InTransform, const BitFlag16 IN InWidgetComponentFlag	, const FWRUILoadCompletedDelegate* IN InFirstDelegate /*= nullptr*/, const FWRUILoadCompletedDelegate* IN InSecondDelegate /*= nullptr*/, const bool IN bInSaveCreatingState /*= false*/)
{
	if (WRMapManager::Get()->GetLoadLevelState() == EWRLoadLevelState::Transition)
		return;

	if ((IsMultiCreatableUI(InUIKind) == false) && UIMap.Contains(InUIKind))
	{
		if (InFirstDelegate != nullptr)
		{
			if(UIMap.Contains(InUIKind)) InFirstDelegate->ExecuteIfBound(UIMap[InUIKind]);
		}
		if (InSecondDelegate != nullptr)
		{
			if(UIMap.Contains(InUIKind)) InSecondDelegate->ExecuteIfBound(UIMap[InUIKind]);
		}
		return;
	}
	 
	if (InFirstDelegate == nullptr)
		CreateUIAsync<TWidgetComponent>(InUIKind, EWRUISpaceType::Space3D, nullptr, InTransform, InWidgetComponentFlag, FWRUILoadCompletedDelegate(), FWRUILoadCompletedDelegate(), bInSaveCreatingState);
	else if (InSecondDelegate == nullptr)
		CreateUIAsync<TWidgetComponent>(InUIKind, EWRUISpaceType::Space3D, nullptr, InTransform, InWidgetComponentFlag, *InFirstDelegate, FWRUILoadCompletedDelegate(), bInSaveCreatingState);
	else
		CreateUIAsync<TWidgetComponent>(InUIKind, EWRUISpaceType::Space3D, nullptr, InTransform, InWidgetComponentFlag, *InFirstDelegate, *InSecondDelegate, bInSaveCreatingState);
	
}

template <class TWidgetComponent>
void WRUIManager::OpenUI(const EWRUIPanelKind IN InUIKind, const AActor* IN InOwnerCharacter, const FTransform& IN InOffsetTransform, const BitFlag16 IN InWidgetComponentFlag, const FWRUILoadCompletedDelegate* IN InFirstDelegate /*= nullptr*/, const FWRUILoadCompletedDelegate* IN InSecondDelegate /*= nullptr*/, const bool IN bInSaveCreatingState /*= false*/)
{
	if (WRMapManager::Get()->GetLoadLevelState() == EWRLoadLevelState::Transition)
		return;

	if ((IsMultiCreatableUI(InUIKind) == false) && UIMap.Contains(InUIKind))
	{
		UIMap[InUIKind].UIPanel->CancelFadeOutForClosePanel();
		if (InFirstDelegate != nullptr)
		{
			if(UIMap.Contains(InUIKind)) InFirstDelegate->ExecuteIfBound(UIMap[InUIKind]);
		}
		if (InSecondDelegate != nullptr)
		{
			if (UIMap.Contains(InUIKind)) InSecondDelegate->ExecuteIfBound(UIMap[InUIKind]);
		}
		return;
	}

	if (InFirstDelegate == nullptr)
		CreateUIAsync<TWidgetComponent>(InUIKind, EWRUISpaceType::Space3D, InOwnerCharacter, InOffsetTransform, InWidgetComponentFlag, FWRUILoadCompletedDelegate(), FWRUILoadCompletedDelegate(), bInSaveCreatingState);
	else if(InSecondDelegate == nullptr)
		CreateUIAsync<TWidgetComponent>(InUIKind, EWRUISpaceType::Space3D, InOwnerCharacter, InOffsetTransform, InWidgetComponentFlag, *InFirstDelegate, FWRUILoadCompletedDelegate(), bInSaveCreatingState);
	else
		CreateUIAsync<TWidgetComponent>(InUIKind, EWRUISpaceType::Space3D, InOwnerCharacter, InOffsetTransform, InWidgetComponentFlag, *InFirstDelegate, *InSecondDelegate, bInSaveCreatingState);
}

template <class TWidgetComponent>
void WRUIManager::CreateUIAsync(const EWRUIPanelKind IN InUIKindToLoad, const EWRUISpaceType IN InUISpaceType, const AActor* IN InOwnerCharacter, const FTransform& IN InOffsetTransform, const BitFlag16 IN InWidgetComponentFlag, const FWRUILoadCompletedDelegate& IN InFirstDelegateToNotify, const FWRUILoadCompletedDelegate& IN InSecondDelegateToNotify, const bool IN bInSaveCreatingState)
{
#if WITH_EDITOR
	// [ 2019-11-11 : kyu ] 구구절절한 사연
	// Shutdown시 UIManager::Shutdown 호출 -> 이후 CharacterManager::Shutdown 호출
	// IO::PreDestroy하기도 전에 Player::Destroy에서 ClearComponentOverlaps 호출하며 IO::OnEndOverlap_InteractionableArea가 호출이 됨.
	// (EndOverlap에서는 Main창에 GyroUI를 끄라는 호출을 함)
	// 결국 EndOverlap에서 아직 DestroyInstance되지 않은 UIManager에 일단 Main창 비동기로드가 호출이 된 상태(FStreamableHandle 유효함)에서 UIManager가 Shutdown되고, 
	// 이후 FStreamableManager::OnPreGarbageCollect()가 호출되며 FStreamableHandle 삭제됨.
	// 그리고 비동기로드에 연결된 람다함수가 호출되며 유효하지 않은 Tail Handle에 접근하며 뻑나는 현상
	if (bShutdown)
		return;
#endif // WITH_EDITOR

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	FSoftObjectPath ObjectPath(WRUIPathManager::Get()->GetUIPath(InUIKindToLoad));

	auto On3DWidgetAsyncLoadCompleted = FStreamableDelegate::CreateLambda([=]()
	{
		TSharedPtr<FStreamableHandle> HandleTemp;
		StreamableHandleQueue.Dequeue(HandleTemp);

		if (bInSaveCreatingState)
		{
			RemoveCreatingState(InUIKindToLoad, InOwnerCharacter);
		}

		if ((IsMultiCreatableUI(InUIKindToLoad) == false) && UIMap.Contains(InUIKindToLoad))
		{
			UIMap[InUIKindToLoad].UIPanel->CancelFadeOutForClosePanel();

			// [ 2020-05-15 : vuvle ] 첫번째 Delegate 에서 해당 Widget 을 Close 하여 사용 유무에 따라 Map 에 존재 하지 않게 될 가능성이 있어 Delegate 를 호출하기 전에 Contains 검사를 한번 더 함.
			// UIMap[A] 의 경우 FindCheck 로 인해 Client 가 shutdown 된다.
			if (UIMap.Contains(InUIKindToLoad)) InFirstDelegateToNotify.ExecuteIfBound(UIMap[InUIKindToLoad]);
			if (UIMap.Contains(InUIKindToLoad)) InSecondDelegateToNotify.ExecuteIfBound(UIMap[InUIKindToLoad]);
			return;
		}

		// SeungYeop : 한캐릭터에 같은종류의 패널은 하나만 존재한다.
		WRUIHandle OpendWidgetHandle = IsWidgetOpened(InOwnerCharacter, InUIKindToLoad);
		if (OpendWidgetHandle != INVALID_UI_HANDLE)
		{
			MultipleUIMap[OpendWidgetHandle].UIPanel->CancelFadeOutForClosePanel();
			if (MultipleUIMap.Contains(OpendWidgetHandle)) InFirstDelegateToNotify.ExecuteIfBound(MultipleUIMap[OpendWidgetHandle]);
			if (MultipleUIMap.Contains(OpendWidgetHandle)) InSecondDelegateToNotify.ExecuteIfBound(MultipleUIMap[OpendWidgetHandle]);
			return;
		}
		

		UWRUIPanel* CreatedPanel = nullptr;
		if (CreateUI(InUIKindToLoad, CreatedPanel) == false)
		{
			ensureMsgf(false, TEXT("Widget Create Failed"));
			return;
		}

		UWRWidgetComponentBase* WidgetComponentBase = nullptr;
		if (CreateWidgetComponent<TWidgetComponent>(WidgetComponentBase, CreatedPanel, InUIKindToLoad) == false)
		{
			ensureMsgf(false, TEXT("WidgetComponent Creation Failed"));
			return;
		}

		if (InOwnerCharacter)
		{
			if (InOwnerCharacter->IsValidLowLevel())
			{
				WidgetComponentBase->SetOwnerActor(InOwnerCharacter);
				WidgetComponentBase->SetOffsetTransform(InOffsetTransform);
			}
			else
			{
				// OwnerCharacter가 이미 삭제되어있다면 생성 하지 않는다
				return;
			}
		}
		else
		{
			WidgetComponentBase->SetFixedTransform(InOffsetTransform);
		}
		
		WidgetComponentBase->SetBitFlag(InWidgetComponentFlag);
		
		const FWRCombinedPanelData& AddedPanelData = AddUIToContainer(InUIKindToLoad, CreatedPanel, WidgetComponentBase);
		InFirstDelegateToNotify.ExecuteIfBound(AddedPanelData);
		InSecondDelegateToNotify.ExecuteIfBound(AddedPanelData);
	});
	
	if (InUISpaceType == EWRUISpaceType::Space2D)
	{
		TSharedPtr<FStreamableHandle> StreamHandle = StreamableManager.RequestAsyncLoad(ObjectPath, On3DWidgetAsyncLoadCompleted, 0);
		if (StreamHandle.IsValid())
		{
			StreamableHandleQueue.Enqueue(StreamHandle);
			if (bInSaveCreatingState)
			{
				SavedCreatingStateList.AddTail(TPair<EWRUIPanelKind, const AActor*>(InUIKindToLoad, InOwnerCharacter));
			}
		}	
	}
	else if (InUISpaceType == EWRUISpaceType::Space3D)
	{
		TSharedPtr<FStreamableHandle> StreamHandle = StreamableManager.RequestAsyncLoad(ObjectPath, On3DWidgetAsyncLoadCompleted, 0);
		if (StreamHandle.IsValid())
		{
			StreamableHandleQueue.Enqueue(StreamHandle);
			if (bInSaveCreatingState)
			{
				SavedCreatingStateList.AddTail(TPair<EWRUIPanelKind, const AActor*>(InUIKindToLoad, InOwnerCharacter));
			}
		}
	}
	else
	{
		ensureMsgf(0, TEXT("Not defined SpaceType Entered"));
	}
}
#endif

template <class TWidgetComponent>
#ifdef DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
FString WRUIManager::OpenUI(const FWRWidgetInfo& IN InWidgetInfo)
{
#if !REFACTORING_LOAD_LEVEL
	if (WRMapManager::Get()->GetLoadLevelState() == EWRLoadLevelState::Transition)
		return NULL_STRING;
#else // REFACTORING_LOAD_LEVEL
	if (WRMapManager::Get()->IsPassedTransition() == false)
		return NULL_STRING;
#endif // REFACTORING_LOAD_LEVEL

	// Already Open State
	EWRUIPanelKind PanelKind = InWidgetInfo.GetPanelKind();
	if ((IsMultiCreatableUI(PanelKind) == false) && UIMap.Contains(PanelKind))
	{
		UIMap[PanelKind].UIPanel->CancelFadeOutForClosePanel();
		InWidgetInfo.GetLoadCompleteDelegate().ExecuteIfBound(UIMap[PanelKind]);
		return NULL_STRING;
	}

	return CreateUIAsync<TWidgetComponent>(InWidgetInfo);;
}
#else // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
void WRUIManager::OpenUI(const FWRWidgetInfo& IN InWidgetInfo)
{
#if !REFACTORING_LOAD_LEVEL
	if (WRMapManager::Get()->GetLoadLevelState() == EWRLoadLevelState::Transition)
		return;
#else // REFACTORING_LOAD_LEVEL
	if (WRMapManager::Get()->IsPassedTransition() == false)
		return;
#endif // REFACTORING_LOAD_LEVEL

// 	if (InWidgetInfo.GetPanelKind() == EWRUIPanelKind::CompassMark)
// 		return;
// 
// 	if (InWidgetInfo.GetPanelKind() == EWRUIPanelKind::QuestBoard)
// 		return;

	// Already Open State
	EWRUIPanelKind PanelKind = InWidgetInfo.GetPanelKind();
	if ((IsMultiCreatableUI(PanelKind) == false) && UIMap.Contains(PanelKind))
	{
		UIMap[PanelKind].UIPanel->CancelFadeOutForClosePanel();
		InWidgetInfo.GetLoadCompleteDelegate().ExecuteIfBound(UIMap[PanelKind]);
		return;
	}

	CreateUIAsync<TWidgetComponent>(InWidgetInfo);
}
#endif // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604

#ifdef DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
template <class TWidgetComponent>
FString WRUIManager::CreateUIAsync(const FWRWidgetInfo& IN InWidgetInfo)
{
	#if WITH_EDITOR
		// [ 2019-11-11 : kyu ] 구구절절한 사연
		// Shutdown시 UIManager::Shutdown 호출 -> 이후 CharacterManager::Shutdown 호출
		// IO::PreDestroy하기도 전에 Player::Destroy에서 ClearComponentOverlaps 호출하며 IO::OnEndOverlap_InteractionableArea가 호출이 됨.
		// (EndOverlap에서는 Main창에 GyroUI를 끄라는 호출을 함)
		// 결국 EndOverlap에서 아직 DestroyInstance되지 않은 UIManager에 일단 Main창 비동기로드가 호출이 된 상태(FStreamableHandle 유효함)에서 UIManager가 Shutdown되고, 
		// 이후 FStreamableManager::OnPreGarbageCollect()가 호출되며 FStreamableHandle 삭제됨.
		// 그리고 비동기로드에 연결된 람다함수가 호출되며 유효하지 않은 Tail Handle에 접근하며 뻑나는 현상
	if (bShutdown)
		return WRASYNCLOADMANAGER_INVALID;
	#endif // WITH_EDITOR

	EWRUIPanelKind PanelKind = InWidgetInfo.GetPanelKind();

	auto On3DWidgetAsyncLoadCompleted = FWRCompleteAsyncLoad::CreateLambda([=](UObject* InObject, FString InKey)
	{
#ifdef DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
#else // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
		TSharedPtr<FStreamableHandle> HandleTemp;
		StreamableHandleQueue.Dequeue(HandleTemp);
#endif // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604

		// Async Load Complete
		if (InWidgetInfo.SaveCreatingState)
		{
			RemoveCreatingState(PanelKind, InWidgetInfo.AttachActor);
		}

		// Already Open State
		if ((IsMultiCreatableUI(PanelKind) == false) && UIMap.Contains(PanelKind))
		{
			UIMap[PanelKind].UIPanel->CancelFadeOutForClosePanel();

			// [ 2020-05-15 : vuvle ] 첫번째 Delegate 에서 해당 Widget 을 Close 하여 사용 유무에 따라 Map 에 존재 하지 않게 될 가능성이 있어 Delegate 를 호출하기 전에 Contains 검사를 한번 더 함.
			// UIMap[A] 의 경우 FindCheck 로 인해 Client 가 shutdown 된다.
			if (UIMap.Contains(PanelKind))
			{
				InWidgetInfo.GetLoadCompleteDelegate().ExecuteIfBound(UIMap[PanelKind]);
			}
			return;
		}

		// SeungYeop : 한캐릭터에 같은종류의 패널은 하나만 존재한다.
		WRUIHandle OpendWidgetHandle = IsWidgetOpened(InWidgetInfo.AttachActor, PanelKind);
		if (OpendWidgetHandle != INVALID_UI_HANDLE)
		{
			MultipleUIMap[OpendWidgetHandle].UIPanel->CancelFadeOutForClosePanel();
			if (MultipleUIMap.Contains(OpendWidgetHandle))
			{
				InWidgetInfo.GetLoadCompleteDelegate().ExecuteIfBound(MultipleUIMap[OpendWidgetHandle]);
			}
			return;
		}

		UWRUIPanel* CreatedPanel = nullptr;
		if (CreateUI(PanelKind, CreatedPanel) == false)
		{
			ensureMsgf(false, TEXT("Widget Create Failed"));
			return;
		}

		UWRWidgetComponentBase* WidgetComponentBase = nullptr;
		if (InWidgetInfo.GetFlags() & EWRUIWidgetCreationBitFlag::CreateWidgetComponent)
		{
			if (CreateWidgetComponent<TWidgetComponent>(WidgetComponentBase, CreatedPanel, PanelKind) == false)
			{
				ensureMsgf(false, TEXT("WidgetComponent Creation Failed"));
				return;
			}
		}

		// 임시
		UWRUIStereoLayerComponent* StereoLayerComponent = nullptr;

#if 0
		if (InWidgetInfo.GetFlags() & EWRUIWidgetCreationBitFlag::CreateStereoLayer)
		{
			if (CreateStereoLayerComponent(StereoLayerComponent, CreatedPanel, InWidgetInfo) == false)
			{
				ensureMsgf(false, TEXT("StereoLayer Creation Failed"));
				return;
			}
		}

		if (WidgetComponentBase == nullptr && StereoLayerComponent == nullptr)
		{
			ensureMsgf(false, TEXT("One of component either Widget & Stereo must be exist."));
			return;
		}

#endif

		if (WidgetComponentBase)
		{
			if (InWidgetInfo.AttachActor)
			{
				if (InWidgetInfo.AttachActor->IsValidLowLevel())
				{
					WidgetComponentBase->SetOwnerActor(InWidgetInfo.AttachActor);
					WidgetComponentBase->SetOffsetTransform(InWidgetInfo.GetTransform());
				}
				else
				{
					// OwnerCharacter가 이미 삭제되어있다면 생성 하지 않는다
					return;
				}
			}
			else
			{
				WidgetComponentBase->SetFixedTransform(InWidgetInfo.GetTransform());
			}

			if (CreatedPanel->IsUsedCylinderGeometry())
			{
				WidgetComponentBase->SetGeometryMode(EWidgetGeometryMode::Cylinder);
				WidgetComponentBase->SetCylinderArcAngle(CreatedPanel->GetCylinderArcAngle());
			}		

			if (WRConsoleManager::Get()->GetBool(EWRConsole::TestYSY))
				WidgetComponentBase->UseStereoLayer();

			// vuvle TODO : 임시로 CreateStereoLayer의 Flag 가 존재할 경우 WidgetComponent 에서 Stereo Component 의 생성으로 변경한다.
			if (WidgetComponentBase->GetStereoLayer() == nullptr)
			{
				if (InWidgetInfo.GetFlags() & EWRUIWidgetCreationBitFlag::CreateStereoLayer)
				{
					if (CreatedPanel)
					{
						if (CreatedPanel->IsUsedCylinderGeometry())
							WidgetComponentBase->UseStereoLayer(EStereoLayerShape::SLSH_CylinderLayer);
						else
							WidgetComponentBase->UseStereoLayer();
					}

					StereoLayerComponent = WidgetComponentBase->GetStereoLayer();
					StereoLayerComponent->SetPriority(CreatedPanel->GetTranslucentSortPriority());
/* Test code
					//StereoLayerComponent->SetCylinderOverlayArc(CreatedPanel->GetCylinderArcAngle());
// 					StereoLayerComponent->SetCylinderOverlayArc(CreatedPanel->GetCylinderArcAngle());
// 					StereoLayerComponent->SetCylinderRadius(70.0f);
// 					StereoLayerComponent->SetCylinderHeight(400.0f);
// 					float a = 350.0f, b = 60.0f;
// 					int32 c = 300;
//					uint32 a = 0;
// 					StereoLayerComponent->SetCylinderOverlayArc(a);
// 					StereoLayerComponent->SetCylinderRadius(b);
// 					StereoLayerComponent->SetCylinderHeight(c);
//					StereoLayerComponent->SetSupportsDepth(a);
*/
				}
			}

			// Add Information
			WidgetComponentBase->SetBitFlag(InWidgetInfo.GetFlags());
			WidgetComponentBase->SetPivot(InWidgetInfo.WidgetComponentPivot);
			WidgetComponentBase->SetOffsetScaleForFixedSize(InWidgetInfo.OffsetScaleForFixedSize);
				/*FWRWidgetInfo::DEFAULT_LOCATION(FVector::ZeroVector);
				FWRWidgetInfo::DEFAULT_ROTATION(FRotator(0.f, 180.f, 0.f));
				FWRWidgetInfo::DEFAULT_SCALE(FVector::OneVector);*/

				//FTransform aa = FVector(10000, 10000, 10000); // , Translation(0.f), Scale3D(FVector::OneVector));
				//FTransform aa = FTransform((FVector::ZeroVector), (FRotator(0.f, 180.f, 0.f)), (FVector::OneVector)); // , Translation(0.f), Scale3D(FVector::OneVector));
				//	FTransform(Rotation(10000, 10000, 10000), Translation(0.f), Scale3D(FVector::OneVector));
				//FTransform aa;// = FTransform(FWRWidgetInfo::DEFAULT_LOCATION, FWRWidgetInfo::DEFAULT_ROTATION, FWRWidgetInfo::DEFAULT_SCALE);
				//aa.set
				//WidgetComponentBase->SetOffsetTransform(aa);

		}


		// Delegate
		const FWRCombinedPanelData& AddedPanelData = AddUIToContainer(PanelKind, CreatedPanel, WidgetComponentBase, StereoLayerComponent);
		InWidgetInfo.GetLoadCompleteDelegate().ExecuteIfBound(AddedPanelData);

	});

	FString CreatingWidgetHash = WRAsyncLoadUObjectManager::Get()->AsyncCreateWidget(WRUIPathManager::Get()->GetUIPath(PanelKind), On3DWidgetAsyncLoadCompleted);

	// On Creating...
	if (InWidgetInfo.SaveCreatingState)
	{
		SavedCreatingStateList.AddTail(TPair<EWRUIPanelKind, const AActor*>(PanelKind, InWidgetInfo.AttachActor));
	}

	return CreatingWidgetHash;

	//TSharedPtr<FStreamableHandle> StreamHandle = StreamableManager.RequestAsyncLoad(ObjectPath, On3DWidgetAsyncLoadCompleted, 0);
	//if (StreamHandle.IsValid())
	//{
	//	StreamableHandleQueue.Enqueue(StreamHandle);

	//	// On Creating...
	//	if (InWidgetInfo.SaveCreatingState)
	//	{
	//		SavedCreatingStateList.AddTail(TPair<EWRUIPanelKind, const AActor*>(PanelKind, InWidgetInfo.AttachActor));
	//	}
	//}
}
#else // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
template <class TWidgetComponent>
void WRUIManager::CreateUIAsync(const FWRWidgetInfo& IN InWidgetInfo)
{
#if WITH_EDITOR
	// [ 2019-11-11 : kyu ] 구구절절한 사연
	// Shutdown시 UIManager::Shutdown 호출 -> 이후 CharacterManager::Shutdown 호출
	// IO::PreDestroy하기도 전에 Player::Destroy에서 ClearComponentOverlaps 호출하며 IO::OnEndOverlap_InteractionableArea가 호출이 됨.
	// (EndOverlap에서는 Main창에 GyroUI를 끄라는 호출을 함)
	// 결국 EndOverlap에서 아직 DestroyInstance되지 않은 UIManager에 일단 Main창 비동기로드가 호출이 된 상태(FStreamableHandle 유효함)에서 UIManager가 Shutdown되고, 
	// 이후 FStreamableManager::OnPreGarbageCollect()가 호출되며 FStreamableHandle 삭제됨.
	// 그리고 비동기로드에 연결된 람다함수가 호출되며 유효하지 않은 Tail Handle에 접근하며 뻑나는 현상
	if (bShutdown)
		return;
#endif // WITH_EDITOR

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	EWRUIPanelKind PanelKind = InWidgetInfo.GetPanelKind();
	FSoftObjectPath ObjectPath(WRUIPathManager::Get()->GetUIPath(PanelKind));
		

	// Load From Widget Data Table
// 	const FWRWidgetTable* FoundWidgetData = GetWidgetCreationTable(PanelKind);
// 	if (FoundWidgetData != nullptr)
// 	{
// 		FWRWidgetInfo& RefInfo = const_cast<FWRWidgetInfo&>(InWidgetInfo);
// 		RefInfo.SetWidgetData(FoundWidgetData);
// 	}


	auto On3DWidgetAsyncLoadCompleted = FStreamableDelegate::CreateLambda([=]()
	{
		TSharedPtr<FStreamableHandle> HandleTemp;
		StreamableHandleQueue.Dequeue(HandleTemp);

		// Async Load Complete
		if (InWidgetInfo.SaveCreatingState)
		{
			RemoveCreatingState(PanelKind, InWidgetInfo.AttachActor);
		}

		// Already Open State
		if ((IsMultiCreatableUI(PanelKind) == false) && UIMap.Contains(PanelKind))
		{
			UIMap[PanelKind].UIPanel->CancelFadeOutForClosePanel();

			// [ 2020-05-15 : vuvle ] 첫번째 Delegate 에서 해당 Widget 을 Close 하여 사용 유무에 따라 Map 에 존재 하지 않게 될 가능성이 있어 Delegate 를 호출하기 전에 Contains 검사를 한번 더 함.
			// UIMap[A] 의 경우 FindCheck 로 인해 Client 가 shutdown 된다.
			if (UIMap.Contains(PanelKind))
			{
				InWidgetInfo.GetLoadCompleteDelegate().ExecuteIfBound(UIMap[PanelKind]);
			}
			return;
		}

		// SeungYeop : 한캐릭터에 같은종류의 패널은 하나만 존재한다.
		WRUIHandle OpendWidgetHandle = IsWidgetOpened(InWidgetInfo.AttachActor, PanelKind);
		if (OpendWidgetHandle != INVALID_UI_HANDLE)
		{
			MultipleUIMap[OpendWidgetHandle].UIPanel->CancelFadeOutForClosePanel();
			if (MultipleUIMap.Contains(OpendWidgetHandle))
			{
				InWidgetInfo.GetLoadCompleteDelegate().ExecuteIfBound(MultipleUIMap[OpendWidgetHandle]);
			}
			return;
		}


		UWRUIPanel* CreatedPanel = nullptr;
		if (CreateUI(PanelKind, CreatedPanel) == false)
		{
			ensureMsgf(false, TEXT("Widget Create Failed"));
			return;
		}

		UWRWidgetComponentBase* WidgetComponentBase = nullptr;
		if (CreateWidgetComponent<TWidgetComponent>(WidgetComponentBase, CreatedPanel, PanelKind) == false)
		{
			ensureMsgf(false, TEXT("WidgetComponent Creation Failed"));
			return;
		}

		if (InWidgetInfo.AttachActor)
		{
			if (InWidgetInfo.AttachActor->IsValidLowLevel())
			{
				WidgetComponentBase->SetOwnerActor(InWidgetInfo.AttachActor);
				WidgetComponentBase->SetOffsetTransform(InWidgetInfo.GetTransform());
			}
			else
			{
				// OwnerCharacter가 이미 삭제되어있다면 생성 하지 않는다
				return;
			}
		}
		else
		{
			WidgetComponentBase->SetFixedTransform(InWidgetInfo.GetTransform());
		}

		// Add Information
		WidgetComponentBase->SetBitFlag(InWidgetInfo.GetFlags());
		WidgetComponentBase->SetPivot(InWidgetInfo.WidgetComponentPivot);
		WidgetComponentBase->SetOffsetScaleForFixedSize(InWidgetInfo.OffsetScaleForFixedSize);

		
		if(WRConsoleManager::Get()->GetBool(EWRConsole::TestYSY))
			WidgetComponentBase->UseStereoLayer();
		

		// Delegate
		const FWRCombinedPanelData& AddedPanelData = AddUIToContainer(PanelKind, CreatedPanel, WidgetComponentBase);
		InWidgetInfo.GetLoadCompleteDelegate().ExecuteIfBound(AddedPanelData);

	});

	TSharedPtr<FStreamableHandle> StreamHandle = StreamableManager.RequestAsyncLoad(ObjectPath, On3DWidgetAsyncLoadCompleted, 0);
	if (StreamHandle.IsValid())
	{
		StreamableHandleQueue.Enqueue(StreamHandle);

		// On Creating...
		if (InWidgetInfo.SaveCreatingState)
		{
			SavedCreatingStateList.AddTail(TPair<EWRUIPanelKind, const AActor*>(PanelKind, InWidgetInfo.AttachActor));
		}
	}
}
#endif // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604

template <class TWidget>
TWidget* WRUIManager::CreateUISync_Casted(FString& IN InPath)
{
	return Cast<TWidget>(CreateUISync(InPath));
}

template <class TWidget>
TWidget* WRUIManager::CreateUIFullPathSync_Casted(FString& IN InFullPath)
{
	return Cast<TWidget>(CreateUIFullPathSync(InFullPath));
}

template <class TWidget>
TWidget* WRUIManager::FindUI(const EWRUIPanelKind IN InUIKind)
{
	FWRCombinedPanelData* FoundData = UIMap.Find(InUIKind);
	if (FoundData != nullptr)
	{
		if (FoundData->UIPanel != nullptr)
		{
			return Cast<TWidget>(FoundData->UIPanel);
		}
	}
	return nullptr;
}

template <class TWidget>
TWidget* WRUIManager::FindUI(const AActor* IN InCharacter, const EWRUIPanelKind IN InUIKind)
{
	FWRCombinedPanelData* FoundData = FindUIData(InCharacter, InUIKind);
	if (FoundData != nullptr)
	{
		if (FoundData->UIPanel != nullptr)
		{
			return Cast<TWidget>(FoundData->UIPanel);
		}
	}
	return nullptr;
}