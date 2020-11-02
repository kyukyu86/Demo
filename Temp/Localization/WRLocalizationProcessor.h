// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enum/EWRLocalization.h"
#include "Utility/WREnumUtility.h"

/**
 * 
 */
class WR_API WRLocalizationProcessor
{
private:
	WRLocalizationProcessor();
	~WRLocalizationProcessor();
	static FName ConvertEnumToStringTableName(const EWRStringTableName IN InStringTableName);
	static FText GetLocalizedString(const FName IN InTableId, const FString& IN InKey, FFormatOrderedArguments&& IN InArguments);

public:
	template <typename... ArgTypes>
	static FText GetLocalizedString(const EWRStringTableName IN InStringTableName, const FString& IN key, ArgTypes... args);

	static void InitializeStringTable();
};

template <typename... ArgTypes>
FText WRLocalizationProcessor::GetLocalizedString(const EWRStringTableName IN InStringTableName, const FString& IN key, ArgTypes... args)
{
	return GetLocalizedString(ConvertEnumToStringTableName(InStringTableName), key, FFormatOrderedArguments{ MoveTemp(args)... });
}
