// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class THIRDPERSON_API DMEnumUtil
{
public:
	template <typename TEnumType>
	static FString EnumToString(const FString& InEnumTypeName, TEnumType InEnum);

};

template <typename TEnumType>
FString DMEnumUtil::EnumToString(const FString& InEnumTypeName, TEnumType InEnum)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *InEnumTypeName, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetNameStringByValue((int32)InEnum);
}


