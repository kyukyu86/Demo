// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel.h"
#include "WidgetBlueprintLibrary.h"
#include "Core/WRGameInstance.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRConsoleManager.h"

void UWRUIPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bFadingOut)
	{
		SetRenderOpacity(GetRenderOpacity() - (1.f / WRConsoleManager::Get()->GetFloat(EWRConsole::UI_FadeOutTime_ClosePanel)) * InDeltaTime);
		if (GetRenderOpacity() <= 0)
		{
			SetRenderOpacity(0);
			OnFadeOutFinished();
		}
	}
}

void UWRUIPanel::OnFadeOutFinished()
{
	WRUIManager::Get()->CloseUI(UIHandle, false);
}

void UWRUIPanel::ClosePanelAfterFadeOut()
{
	if (bFadingOut == true)
		return;

	bFadingOut = true;
}

void UWRUIPanel::CancelFadeOutForClosePanel()
{
	if (bFadingOut == false)
		return;

	bFadingOut = false;
	SetRenderOpacity(1.f);
}
