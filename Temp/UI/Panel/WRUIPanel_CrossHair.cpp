// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_CrossHair.h"
#include "Image.h"
#include "CanvasPanel.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Manager/WRInputSystemManager.h"
#include "Manager/WRConsoleManager.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Engine/Texture.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"

void UWRUIPanel_CrossHair::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_HookCrossHair);
	BIND_WIDGET(UImage, Image_Arrow);
	BIND_WIDGET(UImage, Image_Loading);
	if (Image_Loading)
	{
		Mat_Loading = WRUIUtility::GetDynamicMaterial(Image_Loading);
		if (Mat_Loading)
		{
			Mat_Loading->SetScalarParameterValue("RotateValue", 0);
			Mat_Loading->SetTextureParameterValue("RotateTexture", Texture_Lading);
		}	
	}

	BIND_WIDGET(UCanvasPanel, CanvasPanel_PressCircleButton);

	TopAttackDegree = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TopAttack_Degree);
	BottomAttackDegree = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_BottomAttack_Degree);
	LoadingMaxTime = 0.f;
	LoadingTime = 0.f;
}

void UWRUIPanel_CrossHair::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr || MyCharacter->IsInBattle() == false)
	{
		Image_Arrow->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		float Pitch = WRInputSystemManager::Get()->GetHMDRotation().Pitch;
		if (Pitch >= TopAttackDegree)
		{
			TopAttackUI();
		}
		else if (Pitch > BottomAttackDegree)
		{
			Image_Arrow->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			BottomAttackUI();
		}

		if (LoadingMaxTime > 0)
		{
			LoadingTime += InDeltaTime;
			if (Mat_Loading)
				Mat_Loading->SetScalarParameterValue("RotateValue", LoadingTime / LoadingMaxTime);

			if (LoadingTime > LoadingMaxTime)
				CompleteLoading();
		}
	}
}

void UWRUIPanel_CrossHair::ShowCrossHairPanel()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UWRUIPanel_CrossHair::HideCrossHairPanel()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_CrossHair::ShowHookCrossHair()
{
	Image_HookCrossHair->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UWRUIPanel_CrossHair::HideHookCrossHair()
{
	Image_HookCrossHair->SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_CrossHair::TopAttackUI()
{
	Image_Arrow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Image_Arrow->SetRenderTransformAngle(0);
	Image_Arrow->SetRenderTranslation(FVector2D(0, -10));
}

void UWRUIPanel_CrossHair::BottomAttackUI()
{
	Image_Arrow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Image_Arrow->SetRenderTransformAngle(180);
	Image_Arrow->SetRenderTranslation(FVector2D(0, 10));
}

void UWRUIPanel_CrossHair::StartLoading(const float IN InLoadingMaxTime, const FWRTargetLoadingCompletedDelegate& IN InDelegate)
{
	LoadingMaxTime = InLoadingMaxTime;
	LoadingTime = 0.f;

	TargetLoadingCompletedDelegate = InDelegate;
}

void UWRUIPanel_CrossHair::StopLoading()
{
	LoadingTime = 0.f;
	LoadingMaxTime = 0.f;
	TargetLoadingCompletedDelegate = nullptr;

	if (Mat_Loading)
		Mat_Loading->SetScalarParameterValue("RotateValue", 0.f);
}

void UWRUIPanel_CrossHair::OpenPressCircleButton()
{
	CanvasPanel_PressCircleButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UWRUIPanel_CrossHair::ClosePressCircleButton()
{
	CanvasPanel_PressCircleButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_CrossHair::CompleteLoading()
{
	LoadingTime = 0.f;
	LoadingMaxTime = 0.f;

	if (TargetLoadingCompletedDelegate.IsBound())
	{
		TargetLoadingCompletedDelegate.Execute();
		TargetLoadingCompletedDelegate = nullptr;
	}

	if (Mat_Loading)
		Mat_Loading->SetScalarParameterValue("RotateValue", 0.f);
}
