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
	// [ 2020. 09. 21 : dudwo015] ���ο� ��Ʈ�� ���̺��� �߰� �ϰ� ���� ���,
	// 1. ��Ʈ�� ���̺�� ���� csv ������ ������ش�. Key�� SourceString���� �־�� �Ѵ�.
	// 2. �ش� csv ������ �̸��� ��θ� �Ʒ��� ���� �����ش�.
	//    (����ȭ ��ú��忡�� �ҽ� ���� ��ü�� �б� ������ #define �� ����Ͽ� ��θ� ���η� ������ �� ����..)
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
		// ��Ʈ�� ���̺� Ű�� �ش��ϴ� ���� ���� ��� Ű�� �״�� �������ش�.
		FTextFormat convertedText = FText::FromString(InKey);
		return FText::Format(convertedText, InArguments);
	}
	
	FString StringLocalizedText = LocalizedText.ToString();
	StringLocalizedText.ReplaceInline(TEXT("\\n"), TEXT("\n"));

	if (InArguments.Num() > 0)
	{
		// �ѱ��� ���� ó��
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
