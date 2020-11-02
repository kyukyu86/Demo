// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

//#include "WRUIStruct.h"

#include "../WidgetComponent/WRWidgetComponentBase.h"
#include "UI/Base/WRUISlot.h"

#define INVALID_UI_HANDLE 0

struct FWRCombinedPanelData
{
private:
	FWRCombinedPanelData() {}
public:
	explicit FWRCombinedPanelData(UWRWidgetComponentBase* IN InWidgetComponentBase, UWRUIPanel* IN InUIPanel, class UWRUIStereoLayerComponent* IN InStereoLayerComponent, WRUIHandle IN InHandle) : WidgetComponentBase(InWidgetComponentBase), UIPanel(InUIPanel), StereoLayerComponent(InStereoLayerComponent), UIHandle(InHandle) {}
	UWRWidgetComponentBase* WidgetComponentBase				= nullptr;
	UWRUIPanel* UIPanel										= nullptr;
	class UWRUIStereoLayerComponent* StereoLayerComponent	= nullptr;
	WRUIHandle UIHandle										= INVALID_UI_HANDLE;

	bool Is3DWidget() const
	{
		if (WidgetComponentBase != nullptr && WidgetComponentBase->IsValidLowLevel())
			return true;

		return false;
	}

	bool IsStereoLayerExist() const { return StereoLayerComponent ? true : false; }
};

DECLARE_DELEGATE_OneParam(FWRUILoadCompletedDelegate, const FWRCombinedPanelData&);
DECLARE_DELEGATE_TwoParams(FWRSlotUILoadCompletedDelegate, UWRUISlot*, FString);

DECLARE_DELEGATE(FOnPopupButtonDelegate);
DECLARE_DELEGATE_OneParam(FOnPopupQuantityChangeDelegate, int32);