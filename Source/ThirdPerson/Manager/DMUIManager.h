// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMAsyncLoadManager.h"
#include "../UI/Base/DMUIPanel.h"
#include "../Enum/DMUIEnum.h"


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


class THIRDPERSON_API DMUIManager
{
private:
	TMap<FString, FDMOpenWidgetInfo> AsyncList;
	TMap<EDMPanelKind, FDMPanelData> PanelList;

public:
	DMUIManager();
	~DMUIManager();

	void OpenPanel(FDMOpenWidgetInfo IN InWidgetInfo);
	void ClosePanel(const EDMPanelKind IN InPanelKind);
};
