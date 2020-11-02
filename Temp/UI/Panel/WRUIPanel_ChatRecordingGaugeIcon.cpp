// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_ChatRecordingGaugeIcon.h"
#include "Define/WRUIDefine.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Utility/WRUIUtility.h"

void UWRUIPanel_ChatRecordingGaugeIcon::NativeConstruct()
{
	Super::NativeConstruct();

	//BIND_WIDGET(UImage, Image_DisabledIcon);
	BIND_WIDGET(UImage, Image_Icon);
	BIND_WIDGET(UImage, Image_Gauging);
	if (Image_Gauging)
	{
		Mat_Gauging = WRUIUtility::GetDynamicMaterial(Image_Gauging);
		if (Mat_Gauging)
		{
			Mat_Gauging->SetScalarParameterValue("RotateValue", 0);
			Mat_Gauging->SetTextureParameterValue("RotateTexture", Texture_Gauging);
		}	
	}
	
	GaugingMaxTime = 0.f;
	GaugingTime = 0.f;
}

void UWRUIPanel_ChatRecordingGaugeIcon::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GaugingMaxTime > 0)
	{
		GaugingTime += InDeltaTime;
		if (Mat_Gauging)
			Mat_Gauging->SetScalarParameterValue("RotateValue", GaugingTime / GaugingMaxTime);

		if (GaugingTime > GaugingMaxTime)
			CompleteGauging();
	}
}


void UWRUIPanel_ChatRecordingGaugeIcon::StartGauging(const float IN InGaugingMaxTime, const FWRGaugingCompletedDelegate& IN InDelegate)
{
	GaugingMaxTime = InGaugingMaxTime;
	GaugingTime = 0.f;

	GaugingCompletedDelegate = InDelegate;
}

void UWRUIPanel_ChatRecordingGaugeIcon::StopGauging()
{
	GaugingTime = 0.f;
	GaugingMaxTime = 0.f;
	GaugingCompletedDelegate = nullptr;

	if (Mat_Gauging)
		Mat_Gauging->SetScalarParameterValue("RotateValue", 0.f);
}

void UWRUIPanel_ChatRecordingGaugeIcon::ShowDisabledIcon()
{
	Image_Icon->SetIsEnabled(false);
	//Image_Gauging->SetVisibility(ESlateVisibility::Collapsed);

	//Image_DisabledIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UWRUIPanel_ChatRecordingGaugeIcon::ShowEnabledIcon()
{
	Image_Icon->SetIsEnabled(true);
	//Image_Gauging->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	//Image_DisabledIcon->SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_ChatRecordingGaugeIcon::CompleteGauging()
{
	GaugingTime = 0.f;
	GaugingMaxTime = 0.f;

	if (GaugingCompletedDelegate.IsBound())
	{
		GaugingCompletedDelegate.Execute();
		GaugingCompletedDelegate = nullptr;
	}

	if (Mat_Gauging)
		Mat_Gauging->SetScalarParameterValue("RotateValue", 0.f);
}
