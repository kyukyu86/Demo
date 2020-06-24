// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIManager.h"
#include "DMAsyncLoadManager.h"
#include "DMPathManager.h"


DMUIManager::DMUIManager()
{
}

DMUIManager::~DMUIManager()
{
}

void DMUIManager::OpenPanel(FDMOpenWidgetInfo IN InWidgetInfo)
{
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