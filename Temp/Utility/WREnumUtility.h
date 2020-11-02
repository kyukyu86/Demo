// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"



/**
 * 
 */
class WR_API WREnumUtility
{
public:

	template <typename TEnumType>
	static FString EnumToString(const FString& InEnumTypeName, TEnumType InEnum);

	static int64 StringToEnum(const FString& InEnumTypeName, const FString& InString);

	template <typename TEnumType>
	static FName EnumToName(const FString& InEnumTypeName, TEnumType InEnum);

	template <typename TEnumType>
	static FString EnumToDisplayName(const FString& InEnumTypeName, TEnumType InEnum);

	static uint64 AddBit(uint64 InFrom, uint64 InTo);
	static uint64 RemoveBit(uint64 InFrom, uint64 InTo);
	static uint64 ClearBit();
	static bool IsBit(uint64 InFrom, uint64 InTo);
	static bool IsNone(uint64 InBit);

};

template <typename TEnumType>
FString WREnumUtility::EnumToString(const FString& InEnumTypeName, TEnumType InEnum)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *InEnumTypeName, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetNameStringByValue((int32)InEnum);

	//return (FindObject<UEnum>(ANY_PACKAGE, *InEnumTypeName, true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, *InEnumTypeName, true)->GetDisplayNameTextByIndex((int32)InEnum).ToString() : FString("Invalid - are you sure enum uses UENUM() macro?");
}

template <typename TEnumType>
FName WREnumUtility::EnumToName(const FString& InEnumTypeName, TEnumType InEnum)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *InEnumTypeName, true);
	if (!enumPtr)
	{
		return FName("Invalid");
	}

	return enumPtr->GetNameByValue((int32)InEnum);
}

template <typename TEnumType>
FString WREnumUtility::EnumToDisplayName(const FString& InEnumTypeName, TEnumType InEnum)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *InEnumTypeName, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetDisplayNameTextByValue((int32)InEnum).ToString();
}

