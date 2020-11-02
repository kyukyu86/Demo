// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_TargetUI.h"
#include "Image.h"
#include "Define/WRUIDefine.h"

void UWRUIPanel_TargetUI::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_Lock);
	BIND_WIDGET(UImage, Image_Circle);
	bMulti = false;
	bLock = false;
}


void UWRUIPanel_TargetUI::OnTargetLockChanged(const bool IN bInLcok)
{
	bLock = bInLcok;
	if (bInLcok)
	{
		Image_Lock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Image_Lock->SetVisibility(ESlateVisibility::Collapsed);
	}

	UpdateColor();
}

void UWRUIPanel_TargetUI::SetMulti(const bool IN bInMulti)
{
	bMulti = bInMulti;

	UpdateColor();
}

void UWRUIPanel_TargetUI::UpdateColor()
{
	if (bLock)
		Image_Circle->SetColorAndOpacity(LockColor);
	else if (bMulti)
		Image_Circle->SetColorAndOpacity(MultiColor);
	else
		Image_Circle->SetColorAndOpacity(NormalColor);
}


//uint16 swap_uint16(uint16 val)
//{
//	return (val << 8) | (val >> 8);
//}
//
//int16 swap_int16(int16 val)
//{
//	return (val << 8) | ((val >> 8) & 0xFF);
//}
//
//uint32 swap_uint32(uint32 val)
//{
//	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
//	return (val << 16) | (val >> 16);
//}
//
//int32 swap_int32(int32 val)
//{
//	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
//	return (val << 16) | ((val >> 16) & 0xFFFF);
//}
//
//int64 swap_int64(int64 val)
//{
//	val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
//	val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
//	return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
//}
//
//uint64 swap_uint64(uint64 val)
//{
//	val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
//	val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
//	return (val << 32) | (val >> 32);
//}