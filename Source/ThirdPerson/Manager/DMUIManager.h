// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMAsyncLoadManager.h"
#include "../UI/Base/DMUIPanel.h"
#include "../Enum/DMUIEnum.h"
#include "Singleton/DMSingleton.h"


struct FDMOpenWidgetInfo
{
	EDMPanelKind PanelKind;
	FDMCompleteAsyncLoad CompleteDelegate;
};

struct FDMPanelData
{
	EDMPanelKind PanelKind;
	UDMUIPanel* PanelWidget;
};


class THIRDPERSON_API DMUIManager : public DMSingleton<DMUIManager>
{
private:
	TMap<FString, FDMOpenWidgetInfo> AsyncList;
	TMap<EDMPanelKind, FDMPanelData> PanelList;

public:
	DMUIManager();
	virtual ~DMUIManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;

	void OpenPanel(FDMOpenWidgetInfo IN InWidgetInfo);
	void OpenPanel(const EDMPanelKind IN InKind);
	void ClosePanel(const EDMPanelKind IN InKind);
	bool IsOpenedPanel(const EDMPanelKind IN InKind);
	bool IsAsyncLoadingPanel(const EDMPanelKind IN InKind);

	UUserWidget* CreateUISync(FString IN InPath);
	UUserWidget* CreateUISyncFullPath(FString IN InPath);
	template<class T>
	T* CreateUISync_Casted(FString IN InPath);
	template<class T>
	T* CreateUISyncFullPath_Casted(FString IN InPath);
};


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
