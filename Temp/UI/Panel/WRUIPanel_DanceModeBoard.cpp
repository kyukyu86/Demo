// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_DanceModeBoard.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRAsyncLoadUObjectManager.h"

void UWRUIPanel_DanceModeBoard::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_Keep);
	BIND_WIDGET(UImage, Image_Start);

	RemainTime = 0.f;
}

void UWRUIPanel_DanceModeBoard::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (RemainTime > 0)
	{
		RemainTime -= InDeltaTime;
		if (RemainTime < 0)
		{
			RemainTime = 0.f;
			HideStartImg();
		}
	}
}

void UWRUIPanel_DanceModeBoard::NativeDestruct()
{
	if (AsyncLoadHashForStartTexture.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadHashForStartTexture);
		AsyncLoadHashForStartTexture.Empty();
	}


	if (AsyncLoadHashForKeepTexture.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadHashForKeepTexture);
		AsyncLoadHashForKeepTexture.Empty();
	}

	Super::NativeDestruct();
}

void UWRUIPanel_DanceModeBoard::SetData(UTexture2D* IN InStartTexture, UTexture2D* IN InKeepTexture)
{
	if (InStartTexture == nullptr)
	{
		Image_Start->SetVisibility(ESlateVisibility::Collapsed);
		RemainTime = 0.f;
	}
	else
	{
		WRUIUtility::SetResourceObject(Image_Start, Cast<UObject>(InStartTexture));
		Image_Start->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		RemainTime = 2.f;
	}
	
	if (InKeepTexture == nullptr)
	{
		Image_Keep->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		WRUIUtility::SetResourceObject(Image_Keep, Cast<UObject>(InKeepTexture));
		Image_Keep->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWRUIPanel_DanceModeBoard::SetData(const FString& IN InStartTexturePath, const FString& IN InKeepTexturePath)
{
	if (AsyncLoadHashForStartTexture.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadHashForStartTexture);
		AsyncLoadHashForStartTexture.Empty();
	}


	if (AsyncLoadHashForKeepTexture.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadHashForKeepTexture);
		AsyncLoadHashForKeepTexture.Empty();
	}

	AsyncLoadHashForStartTexture = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InStartTexturePath, FWRCompleteAsyncLoad::CreateUObject(this, &UWRUIPanel_DanceModeBoard::OnLoadCompletedForStartTexture));
	AsyncLoadHashForKeepTexture = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InKeepTexturePath, FWRCompleteAsyncLoad::CreateUObject(this, &UWRUIPanel_DanceModeBoard::OnLoadCompletedForKeepTexture));
	Image_Start->SetVisibility(ESlateVisibility::Collapsed);
	Image_Keep->SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_DanceModeBoard::HideStartImg()
{
	Image_Start->SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_DanceModeBoard::OnLoadCompletedForStartTexture(UObject* IN InStartTexture, FString IN InHash)
{
	AsyncLoadHashForStartTexture.Empty();
	if (InStartTexture == nullptr)
	{
		Image_Start->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	WRUIUtility::SetResourceObject(Image_Start, InStartTexture);
	Image_Start->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	RemainTime = 2.f;
}

void UWRUIPanel_DanceModeBoard::OnLoadCompletedForKeepTexture(UObject* IN InKeepTexture, FString IN InHash)
{
	AsyncLoadHashForKeepTexture.Empty();
	if (InKeepTexture == nullptr)
	{
		Image_Keep->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	WRUIUtility::SetResourceObject(Image_Keep, InKeepTexture);
	Image_Keep->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
