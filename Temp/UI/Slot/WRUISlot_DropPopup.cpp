// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_DropPopup.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRUIManager.h"

#include "TextBlock.h"
#include "Table/WRItemTable.h"
#include "Table/Base/WRTableManager.h"
#include "Image.h"
#include "Manager/WRItemManager.h"

#include "Localization/WRLocalizationProcessor.h"

void UWRUISlot_DropPopup::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_Icon);
	BIND_WIDGET(UTextBlock, TextBlock_Name);
 	BIND_WIDGET(UTextBlock, TextBlock_AddCount);
 	BIND_WIDGET(UTextBlock, TextBlock_Total);
}

void UWRUISlot_DropPopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bShowOff)
		return;

	fShowDuringTime += InDeltaTime;
	if (fShowDuringTime >= ShowOnTime)
	{
		FWRDropInfo NextInfo;
		if (GetNextItem(NextInfo))
		{
			ShowOnItem(NextInfo);
		}
		else
		{
			ShowOffitem();
		}
	}
}

void UWRUISlot_DropPopup::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == Ani_ShowOn)
	{

	}
	else if (Animation == Ani_ShowOff)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}	
}

void UWRUISlot_DropPopup::AddItem(const WRTableID IN InTableID, const int32 IN InCount)
{
	FWRDropInfo AddInfo;
	AddInfo.TableID = InTableID;
	AddInfo.nCount = InCount;
	Drops.Enqueue(AddInfo);

	if (GetVisibility() == ESlateVisibility::Hidden
		|| IsAnimationPlaying(Ani_ShowOff))
	{
		FWRDropInfo NextInfo;
		if (GetNextItem(NextInfo))
		{
			ShowOnItem(NextInfo);
		}
	}
}

bool UWRUISlot_DropPopup::GetNextItem(FWRDropInfo& OUT OutInfo)
{
	if (Drops.IsEmpty())
		return false;

	Drops.Dequeue(OutInfo);
	return true;
}

void UWRUISlot_DropPopup::ShowOnItem(const FWRDropInfo IN InInfo)
{
	bShowOff = false;
	fShowDuringTime = 0.0f;

	FWRItemTableBase* pItemTable = WRTableManager::Get()->FindRowInItemTable(InInfo.TableID);
	if (pItemTable == nullptr)
	{
		FWRDropInfo NextInfo;
		if (GetNextItem(NextInfo))
		{
			ShowOnItem(NextInfo);
		}
		return;
	}
	
	WRUIUtility::SetResourceObject(Image_Icon, pItemTable->IconPath);
	FString NameLocalKey = pItemTable->NameLocalKey.ToString();
	WRUIUtility::SetTextSafe(TextBlock_Name, WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, NameLocalKey).ToString());

	if (InInfo.nCount >= 1)
	{
		TextBlock_AddCount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		WRUIUtility::SetTextSafe(TextBlock_AddCount, FString::Printf(TEXT("+%d"), InInfo.nCount));
	}
	else
	{
		TextBlock_AddCount->SetVisibility(ESlateVisibility::Collapsed);
	}

	TextBlock_Total->SetVisibility(ESlateVisibility::Collapsed);
	//WRUIUtility::SetTextSafe(TextBlock_Total, FString::FormatAsNumber(WRItemManager::Get()->FindItemTotalQuantity(InInfo.TableID)));

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(Ani_ShowOn);
}

void UWRUISlot_DropPopup::ShowOffitem()
{
	bShowOff = true;
	PlayAnimation(Ani_ShowOff);
}
