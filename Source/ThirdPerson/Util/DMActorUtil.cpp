// Fill out your copyright notice in the Description page of Project Settings.


#include "DMActorUtil.h"


FString DMActorUtil::ResolvePath(const FString& InPath)
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