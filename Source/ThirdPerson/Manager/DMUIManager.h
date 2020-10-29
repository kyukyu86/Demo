// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMAsyncLoadManager.h"
#include "../UI/Base/DMUIPanel.h"
#include "../Enum/DMUIEnum.h"
#include "Singleton/DMSingleton.h"
#include "../UI/WidgetComponent/DMWidgetComponentBase.h"
#include "../GameInstance/DMGameInstance.h"
#include <Engine/DataTable.h>


struct FDMOpenWidgetInfo
{
	FDMOpenWidgetInfo(
		EDMPanelKind IN InPanelKind
		, FVector IN InStdLocation = FVector::ZeroVector
		, FRotator IN InStdRotator = FRotator::ZeroRotator
		, FVector IN InStdScale = FVector::OneVector
		, EDMWidgetComponentFlag IN InAddFlags = EDMWidgetComponentFlag::None
		, UObject* IN InOwnerObject = nullptr);

	FDMOpenWidgetInfo(
		EDMPanelKind IN InPanelKind
		, FTransform IN InStdTransform = FTransform::Identity
		, EDMWidgetComponentFlag IN InAddFlags = EDMWidgetComponentFlag::None
		, UObject* IN InOwnerObject = nullptr);

	FDMOpenWidgetInfo(
		FString IN InWidgetPath
		, FTransform IN InStdTransform = FTransform::Identity
		, FVector IN InAddLocation = FVector::ZeroVector
		, FRotator IN InAddRotator = FRotator::ZeroRotator
		, FVector IN InAddScale = FVector::OneVector
		, EDMWidgetType IN InWidgetType = EDMWidgetType::None
		, EDMWidgetComponentFlag IN InAddFlags = EDMWidgetComponentFlag::None
		, UObject* IN InOwnerObject = nullptr);

public:
	EDMWidgetCreationType WidgetCreationType = EDMWidgetCreationType::None;
	EDMWidgetType WidgetType = EDMWidgetType::None;

	// for Widget by Path
	FString WidgetPath = "";

	// for Panel Type
	EDMPanelKind PanelKind = EDMPanelKind::None;

	// for Widget Component
	FTransform Transform = FTransform::Identity;
	EDMWidgetComponentFlag WidgetComponentFlags = EDMWidgetComponentFlag::None;

	FDMCompleteAsyncLoad CompleteDelegate;

	UObject* OwnerObject = nullptr;
};

struct FDMWidgetData
{	
	EDMWidgetType WidgetType = EDMWidgetType::None;

	EDMPanelKind PanelKind = EDMPanelKind::None;
	UUserWidget* Widget = nullptr;
	UDMWidgetComponentBase* WidgetComponent = nullptr;
	class ADMWidgetActorBase* WidgetActor = nullptr;

	UObject* OwnerObject = nullptr;
};


class THIRDPERSON_API DMUIManager : public DMSingleton<DMUIManager>
{
private:
	TMap<FString, FDMOpenWidgetInfo> AsyncList;
	TDoubleLinkedList<FDMWidgetData> WidgetDataList;
	TArray<EDMPanelKind> DisappearProgressList;


	TAssetPtr<UDataTable> WidgetTable = nullptr;
	FString strAsyncKey_WidgetTable = "";

private:
	template<class TWidgetComponent>
	bool CreateWidgetComponent(UDMWidgetComponentBase*& OUT OutWidgetComponent, UUserWidget* IN InWidget);
	void CloseWidgetAll();

	void LoadWidgetTable();

	void AddDisappearProgressList(const EDMPanelKind IN InKind);

public:
	virtual void OnInit() override;
	virtual void OnShutdown() override;

	FString OpenPanel(FDMOpenWidgetInfo IN InWidgetInfo);
	FString OpenPanel(const EDMPanelKind IN InKind);

	void ClosePanel(const EDMPanelKind IN InKind);
	void CloseWidget(UUserWidget* IN InUserWidget);
	void CloseWidget(UObject* IN InObject);

	bool IsOpenedPanel(const EDMPanelKind IN InKind);
	bool IsOpenedPanel(UObject* IN InObject);
	bool IsAsyncLoadingPanel(const EDMPanelKind IN InKind);
	bool IsAsyncLoadingWidget(const FString& IN InWidgetPath);
	bool IsAsyncLoadingWidget(UObject* IN InObject);

	template<class T>
	T* FindPanel(const EDMPanelKind IN InKind);
	template<class T>
	T* FindWidgetActor(const EDMPanelKind IN InKind);

	void RemoveDisappearProgressList(const EDMPanelKind IN InKind);
	bool IsDisappearProgress(const EDMPanelKind IN InKind);

	// Sync
	UUserWidget* CreateUISync(FString IN InPath);
	UUserWidget* CreateUISyncClass(UClass* InClass);
	template<class T>
	T* CreateUISync_Casted(FString IN InPath);
	template<class T>
	T* CreateUISyncClass_Casted(UClass* InClass);

	UUserWidget* CreateUISyncFullPath(FString IN InPath);
	template<class T>
	T* CreateUISyncFullPath_Casted(FString IN InPath);

	// Async
	FString CreateUIASyncFullPath(FString& IN InFullPath, const FDMCompleteAsyncLoad IN InDelegate);

	const struct FDMWidgetTable* GetWidgetCreationTable(const EDMPanelKind IN InPanelKind);

};


template<class T>
T* DMUIManager::FindPanel(const EDMPanelKind IN InKind)
{
	TDoubleLinkedList<FDMWidgetData>::TDoubleLinkedListNode* Node = WidgetDataList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		FDMWidgetData& WidgetData = Node->GetValue();
		if (WidgetData.PanelKind == InKind)
		{
			return Cast<T>(WidgetData.Widget);
		}
	}

	return nullptr;
}

template<class T>
T* DMUIManager::FindWidgetActor(const EDMPanelKind IN InKind)
{
	TDoubleLinkedList<FDMWidgetData>::TDoubleLinkedListNode* Node = WidgetDataList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		FDMWidgetData& WidgetData = Node->GetValue();
		if (WidgetData.PanelKind == InKind)
		{
			return Cast<T>(WidgetData.WidgetActor);
		}
	}

	return nullptr;
}

template<class T>
T* DMUIManager::CreateUISync_Casted(FString IN InPath)
{
	UUserWidget* CreatedWidget = CreateUISync(InPath);
	if (CreatedWidget == nullptr)
		return nullptr;

	return Cast<T>(CreatedWidget);
}

template<class T>
T* DMUIManager::CreateUISyncFullPath_Casted(FString IN InPath)
{
	UUserWidget* CreatedWidget = CreateUISyncFullPath(InPath);
	if (CreatedWidget == nullptr)
		return nullptr;

	return Cast<T>(CreatedWidget);
}

template<class T>
T* DMUIManager::CreateUISyncClass_Casted(UClass* InClass)
{
	UUserWidget* CreatedWidget = CreateUISyncClass(InClass);
	if (CreatedWidget == nullptr)
		return nullptr;

	return Cast<T>(CreatedWidget);
}

template<class TWidgetComponent>
bool DMUIManager::CreateWidgetComponent(UDMWidgetComponentBase*& OUT OutWidgetComponent, UUserWidget* IN InWidget)
{
	if (InWidget == nullptr)
	{
		ensureMsgf(false, TEXT("Panel must be exist"));
		return false;
	}

	OutWidgetComponent = nullptr;

	UDMWidgetComponentBase* CreatedWidgetComponentBase = NewObject<UDMWidgetComponentBase>();
	if (CreatedWidgetComponentBase == nullptr)
	{
		ensureMsgf(false, TEXT("Widget Component create failed."));
		return false;
	}

	if (CreatedWidgetComponentBase->IsRegistered() == false)
	{
		UDMGameInstance* GameInstance = UDMGameInstance::GetGameInstance();
		if (GameInstance == nullptr)
			return false;

		UWorld* World = GameInstance->GetWorld();
		if (World == nullptr)
			return false;

		CreatedWidgetComponentBase->RegisterComponentWithWorld(World);
	}

	CreatedWidgetComponentBase->AddToRoot();
	CreatedWidgetComponentBase->SetWidget(InWidget);
	CreatedWidgetComponentBase->SetActive(true);
	
	OutWidgetComponent = CreatedWidgetComponentBase;

	return true;
}
