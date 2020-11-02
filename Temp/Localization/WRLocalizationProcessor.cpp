// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRLocalizationProcessor.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/StringTableCore.h"
#include "WRKoreanPostpositionProcessor.h"

WRLocalizationProcessor::WRLocalizationProcessor()
{
}

WRLocalizationProcessor::~WRLocalizationProcessor()
{
}

void WRLocalizationProcessor::InitializeStringTable()
{
	// [ 2020. 09. 21 : dudwo015] 새로운 스트링 테이블을 추가 하고 싶을 경우,
	// 1. 스트링 테이블로 읽을 csv 파일을 만들어준다. Key와 SourceString열이 있어야 한다.
	// 2. 해당 csv 파일의 이름과 경로를 아래와 같이 적어준다.
	//    (현지화 대시보드에서 소스 파일 자체를 읽기 떄문에 #define 을 사용하여 경로를 별로로 선언할 수 없다..)
	FStringTableRegistry::Get().UnregisterStringTable(TEXT("ClientErrorStringTable"));
	LOCTABLE_FROMFILE_GAME("ClientErrorStringTable", "ClientErrorStringTable", "Asset/Data/CSV/StringTable/ClientErrorStringTable.csv");

	FStringTableRegistry::Get().UnregisterStringTable(TEXT("CommonStringTable"));
	LOCTABLE_FROMFILE_GAME("CommonStringTable", "CommonStringTable", "Asset/Data/CSV/StringTable/CommonStringTable.csv");

	FStringTableRegistry::Get().UnregisterStringTable(TEXT("ItemStringTable"));
	LOCTABLE_FROMFILE_GAME("ItemStringTable", "ItemStringTable", "Asset/Data/CSV/StringTable/ItemStringTable.csv");

	FStringTableRegistry::Get().UnregisterStringTable(TEXT("MiscStringTable"));
	LOCTABLE_FROMFILE_GAME("MiscStringTable", "MiscStringTable", "Asset/Data/CSV/StringTable/MiscStringTable.csv");

	FStringTableRegistry::Get().UnregisterStringTable(TEXT("NetworkErrorStringTable"));
	LOCTABLE_FROMFILE_GAME("NetworkErrorStringTable", "NetworkErrorStringTable", "Asset/Data/CSV/StringTable/NetworkErrorStringTable.csv");

	FStringTableRegistry::Get().UnregisterStringTable(TEXT("NPCStringTable"));
	LOCTABLE_FROMFILE_GAME("NPCStringTable", "NPCStringTable", "Asset/Data/CSV/StringTable/NPCStringTable.csv");

	FStringTableRegistry::Get().UnregisterStringTable(TEXT("QuestStringTable"));
	LOCTABLE_FROMFILE_GAME("QuestStringTable", "QuestStringTable", "Asset/Data/CSV/StringTable/QuestStringTable.csv");

	FStringTableRegistry::Get().UnregisterStringTable(TEXT("TutorialStringTable"));
	LOCTABLE_FROMFILE_GAME("TutorialStringTable", "TutorialStringTable", "Asset/Data/CSV/StringTable/TutorialStringTable.csv");
}

FText WRLocalizationProcessor::GetLocalizedString(const FName IN InTableId, const FString& IN InKey, FFormatOrderedArguments&& IN InArguments)
{
	FText LocalizedText = FText::FromStringTable(InTableId, InKey);
	if (FTextInspector::GetSourceString(LocalizedText) == &FStringTableEntry::GetPlaceholderSourceString())
	{
		// 스트링 테이블에 키에 해당하는 값이 없을 경우 키를 그대로 리턴해준다.
		FTextFormat convertedText = FText::FromString(InKey);
		return FText::Format(convertedText, InArguments);
	}
	
	FString StringLocalizedText = LocalizedText.ToString();
	StringLocalizedText.ReplaceInline(TEXT("\\n"), TEXT("\n"));

	if (InArguments.Num() > 0)
	{
		// 한국어 조사 처리
		if (UKismetInternationalizationLibrary::GetCurrentCulture() == TEXT("ko"))
		{
			WRKoreanPostpositionProcessor::ReplaceKoreanPostpositionToArrumentModifier(StringLocalizedText);
		}
		FTextFormat ConvertedText = FText::FromString(StringLocalizedText);
		return FText::Format(ConvertedText, InArguments);
	}
	else
	{
		return FText::FromString(StringLocalizedText);
	}
}

FName WRLocalizationProcessor::ConvertEnumToStringTableName(const EWRStringTableName IN InStringTableName)
{
	FName StringTableName = WREnumUtility::EnumToName<EWRStringTableName>("EWRStringTableName", InStringTableName);
	FName RemovePrefixName(StringTableName.ToString().Replace(TEXT("EWRStringTableName::"), TEXT("")));
	return RemovePrefixName;
}
