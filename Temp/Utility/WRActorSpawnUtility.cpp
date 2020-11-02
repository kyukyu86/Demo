// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorSpawnUtility.h"

DEFINE_LOG_CATEGORY(ActorSpawnUtility)

// SVN Post Commit Hook Test 31
FString WRActorSpawnUtility::ResolvePath(const FString& InPath)
{
	FString ResolvedPath = InPath;
	int32 DotIndex = -1;
	ResolvedPath.FindLastChar('.', DotIndex);
	if (DotIndex != -1)
	{
		FString Check_C = InPath.Right(2);
		if (Check_C.ToLower() != TEXT("_c"))
		{
			ResolvedPath += TEXT("_C");
		}
	}
	else
	{
		int32 SlashIndex;
		InPath.FindLastChar('/', SlashIndex);
		FString FileName = InPath.Right(InPath.Len() - SlashIndex - 1);
		ResolvedPath += TEXT(".") + FileName + TEXT("_C");
	}

	return ResolvedPath;
}