// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIManager.h"
#include "DMAsyncLoadManager.h"
#include "DMPathManager.h"
#include "../GameInstance/DMGameInstance.h"


DMUIManager::DMUIManager()
{

}

DMUIManager::~DMUIManager()
{
	
}

void DMUIManager::OnInit()
{

}

void DMUIManager::OnShutdown()
{
	for (auto& Each : PanelList)
	{
		FDMPanelData& PanelData = Each.Value;
		if (PanelData.PanelWidget->IsValidLowLevel())
		{
			PanelData.PanelWidget->RemoveFromViewport();
//			delete PanelData.PanelWidget;
		}
	}
	PanelList.Empty();
}

void DMUIManager::OpenPanel(FDMOpenWidgetInfo IN InWidgetInfo)
{
	if (IsAsyncLoadingPanel(InWidgetInfo.PanelKind))
		return;

	if (IsOpenedPanel(InWidgetInfo.PanelKind))
		return;

	auto AsyncComplete = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
	{
		FDMOpenWidgetInfo* FoundInfo = AsyncList.Find(InKey);
		if (FoundInfo == nullptr)
			return;

		UDMUIPanel* CastedPanel = Cast<UDMUIPanel>(InObject);
		if (CastedPanel == nullptr)
			return;

		CastedPanel->AddToViewport();

		FDMPanelData NewPanelData;
		NewPanelData.PanelKind = FoundInfo->PanelKind;
		NewPanelData.PanelWidget = CastedPanel;
		PanelList.Add(FoundInfo->PanelKind, NewPanelData);

		// Complete Delegate
		FoundInfo->CompleteDelegate.ExecuteIfBound(InObject, InKey);

		AsyncList.Remove(InKey);
	});
	FString strAsyncKey = DMAsyncLoadManager::Get()->AsyncCreateWidget(DMPathManager::Get()->GetUIPanelPath(InWidgetInfo.PanelKind), AsyncComplete);
	AsyncList.Add(strAsyncKey, InWidgetInfo);
}

void DMUIManager::OpenPanel(const EDMPanelKind IN InKind)
{	
	FDMOpenWidgetInfo TempInfo;
	TempInfo.PanelKind = InKind;
	OpenPanel(TempInfo);
}

void DMUIManager::ClosePanel(const EDMPanelKind IN InPanelKind)
{
	// Check Async List
	for (auto& AsyncElement : AsyncList)
	{
		if (AsyncElement.Value.PanelKind == InPanelKind)
		{
			DMAsyncLoadManager::Get()->CancelAsyncLoad(AsyncElement.Key);
			AsyncList.Remove(AsyncElement.Key);
			return;
		}
	}

	// Check Panel List
	FDMPanelData* FoundData = PanelList.Find(InPanelKind);
	if (FoundData == nullptr)
		return;

	if (FoundData->PanelWidget->IsValidLowLevel())
	{
		FoundData->PanelWidget->RemoveFromViewport();
	}
}

bool DMUIManager::IsOpenedPanel(const EDMPanelKind IN InKind)
{
	if (PanelList.Find(InKind) == nullptr)
		return false;

	return true;
}

bool DMUIManager::IsAsyncLoadingPanel(const EDMPanelKind IN InKind)
{
	for (auto Data : AsyncList)
	{
		if (Data.Value.PanelKind == InKind)
			return true;
	}

	return false;
}

UUserWidget* DMUIManager::CreateUISync(FString IN InPath)
{
	UUserWidget* pWidget = nullptr;
	FString strPath = "/Game/Blueprints/UI/Slot/";
	strPath += InPath + "." + InPath + "_C";
	FStringClassReference UIClassReference(*strPath);
	UClass* UIClass = nullptr;
	if (UIClassReference.TryLoadClass<UUserWidget>() != nullptr)
	{
		UIClass = UIClassReference.ResolveClass();
	}
	else
	{
		return nullptr;
	}
	if (UIClass)
	{
		pWidget = CreateWidget<UUserWidget>(UDMGameInstance::GetGameInstance()->GetWorld(), UIClass);
		if (pWidget == nullptr)
			return nullptr;
	}
	return pWidget;
}
