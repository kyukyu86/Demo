// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_CustomText.h"	
#include "Define/WRUIDefine.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRCustomTextTable.h"
#include "Utility/WRUIUtility.h"

#include "Localization/WRLocalizationProcessor.h"


void UWRUIPanel_CustomText::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Content);
}

void UWRUIPanel_CustomText::ThrowParam(const int32 IN InParam)
{
	FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(InParam);
	if (CustomTextTable)
	{
		FString LocalKey = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
		WRTextBlock_Content->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey));
	}
}